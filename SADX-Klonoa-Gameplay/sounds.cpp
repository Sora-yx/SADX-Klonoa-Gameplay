#include "pch.h"
#include "bass_vgmstream.h"


typedef struct {
	int           id;
	int           flags;
	int           timer;
	float         dist;
	float         volumeA;
	float         volumeB;
	float         pitch;
	NJS_VECTOR* position;
	HSTREAM       stream;
	task* obj;
} CUSTOM_SOUNDENTRY;

static CUSTOM_SOUNDENTRY SoundListEntries[256];

static FunctionHook<void> PauseSound_t((intptr_t)0x40D060);
static FunctionHook<void> ResumeSound_t((intptr_t)0x40D0A0);

void PauseCustomSounds() {
	for (int i = 0; i < LengthOfArray(SoundListEntries); ++i) {
		BASS_ChannelPause(SoundListEntries[i].stream);
	}
}

void ResumeCustomSounds() {
	for (int i = 0; i < LengthOfArray(SoundListEntries); ++i) {
		BASS_ChannelPlay(SoundListEntries[i].stream, false);
	}
}

void PauseSound_j() {
	PauseSound_t.Original();
	PauseCustomSounds();
}

void ResumeSound_j() {
	ResumeSound_t.Original();
	ResumeCustomSounds();
}

static void __stdcall FreeSoundStream(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	BASS_ChannelStop(channel);
	BASS_StreamFree(channel);
}

int GetSoundEntryByStream(HCHANNEL channel)
{
	for (int i = 0; i < LengthOfArray(SoundListEntries); ++i)
	{
		if (SoundListEntries[i].stream == channel) return i;
	}

	return -1;
}

static void __stdcall FreeSoundStreamQueue(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	int entryID = GetSoundEntryByStream(channel);

	if (entryID > -1)
	{
		SoundListEntries[entryID].stream = NULL;
	}

	FreeSoundStream(handle, channel, data, user);
}

void FreeSoundEntry(CUSTOM_SOUNDENTRY& entry)
{
	if (entry.stream)
	{
		FreeSoundStreamQueue(NULL, entry.stream, NULL, NULL);
		entry.stream = NULL;
	}

	entry.flags = 0;
	entry.dist = 0;
	entry.flags = 0;
	entry.obj = nullptr;
	entry.pitch = 0;
	entry.volumeA = 0.4f;
	entry.volumeB = 0;
	entry.position = nullptr;
}

CUSTOM_SOUNDENTRY* GetFreeSoundEntry()
{
	for (auto& entry : SoundListEntries)
	{
		if (entry.stream == NULL)
		{
			return &entry;
		}
	}

	return nullptr;
}

CUSTOM_SOUNDENTRY* GetSoundEntryByTask(task* obj)
{
	for (auto& entry : SoundListEntries)
	{
		if (entry.obj == obj)
		{
			return &entry;
		}
	}

	return nullptr;
}

void FreeAllCustomSounds()
{
	for (auto& entry : SoundListEntries)
	{
		FreeSoundStream(NULL, entry.stream, NULL, NULL);
		FreeSoundEntry(entry);
	}
}

DWORD LoadSoundSteam_LoadFromFile(std::string path)
{
	const char* filename = HelperFunctionsGlobal.GetReplaceablePath(path.c_str());

	DWORD channel = BASS_VGMSTREAM_StreamCreate(filename, 0);

	if (channel == 0)
	{
		channel = BASS_StreamCreateFile(false, filename, 0, 0, 0);
	}

	return channel;
}

DWORD LoadSoundStream(int ID) 
{	
	std::string filename = "sounds\\";
	

	std::string str = filename + std::to_string(ID) + ".wav";

	DWORD channel = LoadSoundSteam_LoadFromFile(str);

	if (channel == 0) {
		str = filename + std::to_string(ID) + ".adx";
		channel = LoadSoundSteam_LoadFromFile(str);
	}

	return channel;
}

float GetVolumeRange(NJS_VECTOR* pos, float dist)
{
	if (Camera_Data1)
	{
		float playerdist = GetDistance(&Camera_Data1->Position, pos);
		return 0.4f - (0.4f * playerdist / dist);
	}

	return 0.0f;
}

void PlaySoundChannelQueue(int ID, CUSTOM_SOUNDENTRY* entry, bool loop)
{
	HSTREAM channel = LoadSoundStream(ID);

	if (channel != 0)
	{
		BASS_ChannelPlay(channel, false);
		BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, entry->volumeA);

		if (loop) {
			BASS_ChannelFlags(channel, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
		}
		else {
			BASS_ChannelSetSync(channel, BASS_SYNC_END, 0, FreeSoundStreamQueue, nullptr);
		}

		entry->id = ID;
		entry->stream = channel;
	}
}

void PlayCustomSound(int ID) {
	auto entry = GetFreeSoundEntry();

	if (entry != nullptr)
	{
		PlaySoundChannelQueue(ID, entry, 0);
	}
}

void PlayCustomSoundVolume(int ID, float volume) {
	auto entry = GetFreeSoundEntry();

	if (entry != nullptr)
	{
		entry->volumeB = volume;
		PlaySoundChannelQueue(ID, entry, 0);
	}
}

CUSTOM_SOUNDENTRY* PlayCustomSoundQueue(int ID, task* obj, NJS_VECTOR* pos, float dist, bool loop, float volume, float pitch) {
	auto entry = GetFreeSoundEntry();

	if (entry != nullptr)
	{
		entry->obj = obj;
		entry->dist = dist;

		//The first frame is always wrong for 3d volume, skip it
		if (obj || pos) entry->volumeA = 0;

		if (volume) entry->volumeB = volume;
		if (pitch) entry->pitch = pitch;
		if (pos) entry->position = pos;

		PlaySoundChannelQueue(ID, entry, loop);
	}

	return entry;
}

void PlayCustomSound_Entity(int ID, task* obj, float dist, bool loop) {
	dist *= 2;
	PlayCustomSoundQueue(ID, obj, NULL, dist, loop, NULL, NULL);
}

void PlayCustomSound_EntityAndVolume(int ID, task* obj, float dist, float volume, bool loop) {
	dist *= 2;
	PlayCustomSoundQueue(ID, obj, NULL, dist, loop, volume, 0.0f);
}

void PlayCustomSound_EntityAndPos(int ID, task* obj, NJS_VECTOR* pos, float dist, float volume, bool loop) {
	PlayCustomSoundQueue(ID, obj, pos, dist, loop, volume, 0.0f);
}

void PlayCustomSound_Pos(int ID, NJS_VECTOR* pos, float dist, float volume, bool loop) {
	PlayCustomSoundQueue(ID, nullptr, pos, dist, loop, volume, 0.0f);
}


void DelayedCustomSound(task* obj) {

	auto data = obj->twp;

	if (--data->scl.y <= 0) {
		if (data->scl.z != 0) {
			PlayCustomSoundVolume(data->scl.x, data->scl.z);
		}
		else {
			PlayCustomSound(data->scl.x);
		}

		FreeTask(obj);
	}
}

void PlayDelayedCustomSound(int ID, int time, float volumeoverride) {
	task* temp = CreateElementalTask(LoadObj_Data1, 1, DelayedCustomSound);
	temp->twp->scl.x = ID;
	temp->twp->scl.y = time;
	temp->twp->scl.z = volumeoverride;
}

void RunCustomSounds() {
	if (GameState < 15 || GameState > 16)
		return;

	for (auto& entry : SoundListEntries)
	{
		if (entry.stream != NULL)
		{
			HSTREAM stream = entry.stream;

			// Ignore if the sound is paused
			if (BASS_ChannelIsActive(stream) == BASS_ACTIVE_PAUSED) {
				continue;
			}

			if (BASS_ChannelIsActive(stream) == BASS_ACTIVE_STOPPED ||
				BASS_ChannelIsActive(stream) == BASS_ACTIVE_STALLED) {
				FreeSoundEntry(entry);
				continue;
			}

			if (entry.obj && (!entry.obj->twp || entry.obj->twp->flag & Status_KillSound)) {
				FreeSoundEntry(entry);
				continue;
			}


			// Get the volume of the sound based on the distance from the entity or the static position
			if (entry.dist > 0) {
				if (entry.position) {
					if (!IsPlayerInsideSphere_(entry.position, entry.dist)) {
						entry.volumeA = 0;
					}
					else {
						entry.volumeA = GetVolumeRange(entry.position, entry.dist);
					}
				}
				else if (entry.obj) {
					if (!IsPlayerInsideSphere_(&entry.obj->twp->pos, entry.dist))
					{
						entry.volumeA = 0;
					}
					else {
						entry.volumeA = GetVolumeRange(&entry.obj->twp->pos, entry.dist);
					}
				}

				// If volume B is set (static volume), multiply Volume A (3d volume here) by it.
				if (entry.volumeB) {
					entry.volumeA *= entry.volumeB;
				}

				// Set the new volume
				BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, entry.volumeA);
			}
			else if (entry.volumeB) {
				// If the sound is not 3d and Volume B is set (static volume), set it.
				BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, entry.volumeB);
			}
		}
	}
}

void Sounds_Init() {

	PauseSound_t.Hook(PauseSound_j);
	ResumeSound_t.Hook(ResumeSound_j);
}