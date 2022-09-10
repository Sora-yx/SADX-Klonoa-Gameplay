#pragma once

enum VoicesE
{
	kl_speech1 = 4000,
	kl_speech2,
	kl_speech3,
	kl_speech4,
	kl_speech5,

};

enum klonoaVoice
{
	kl_wahoo,
	kl_wahoo1,
	kl_wahoo2,
	kl_SuperJump0,
	kl_SuperJump1,
	kl_grab,
	kl_throw,
	kl_pain,
	kl_pain2,
	kl_deathFall,
	kl_cannonshot,
	kl_drown
};

enum soundEffect
{
	jump = 100,
	hover,
	shot,
	pickEnemy,
	alarmClock,
	dreamStn,
	bubbleHit,
	death,
};

void Sounds_Init();
void RunCustomSounds();

#define Status_KillSound 0x4000


void RunCustomSounds();
DWORD LoadSoundSteam_LoadFromFile(std::string path);
void ResumeCustomSounds();
void PauseCustomSounds();
void FreeAllCustomSounds();

void PlayCustomSound(int ID);
void PlayCustomSoundVolume(int ID, float volume);
void PlayDelayedCustomSound(int ID, int time, float volumeoverride);
void PlayCustomSound_Entity(int ID, task* obj, float dist, bool loop);
void PlayCustomSound_EntityAndPos(int ID, task* obj, NJS_VECTOR* pos, float dist, float volume, bool loop);
void PlayCustomSound_EntityAndVolume(int ID, task* obj, float dist, float volume, bool loop);
void PlayCustomSound_Pos(int ID, NJS_VECTOR* pos, float dist, float volume, bool loop);


