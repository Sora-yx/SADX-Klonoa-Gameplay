#include "pch.h"

D3DMATRIX WorldMatrixBackup;

FastcallFunctionPointer(void, DrawChunkModel_, (Sint32* a1, Sint16* a2), 0x7917F0);

ModelInfo* LoadBasicModel(const char* name) {
	PrintDebug("Loading basic model: %s... ", name);

	std::string fullPath = "system\\models\\";
	fullPath = fullPath + name + ".sa1mdl";

	ModelInfo* mdl = new ModelInfo(HelperFunctionsGlobal.GetReplaceablePath(fullPath.c_str()));

	if (mdl->getformat() != ModelFormat_Basic) {
		PrintDebug("Failed!\n");
		delete mdl;
		return nullptr;
	}

	PrintDebug("Done.\n");
	return mdl;
}

ModelInfo* LoadChunkModel(const char* name) {
	PrintDebug("[Klonoa Mod] Loading chunk model: %s... ", name);

	std::string fullPath = "system\\models\\";
	fullPath = fullPath + name + ".sa2mdl";

	ModelInfo* mdl = new ModelInfo(HelperFunctionsGlobal.GetReplaceablePath(fullPath.c_str()));

	if (mdl->getformat() != ModelFormat_Chunk) {
		PrintDebug("Failed!\n");
		delete mdl;
		return nullptr;
	}

	PrintDebug("Done.\n");
	return mdl;
}

void LoadModelListFuncPtr(const char** names, int count, ModelInfo** mdls, ModelInfo* (*func)(const char*)) {
	for (int i = 0; i < count; ++i) {
		mdls[i] = func(names[i]);
	}
}

void FreeMDL(ModelInfo* pointer) {
	if (pointer) {
		PrintDebug("[Klonoa Mod] Freeing model: %s... \n", pointer->getdescription().c_str());
		delete(pointer);
		pointer = nullptr;
	}
}

// Animation file functions

void LoadAnimation(AnimationFile** info, const char* name, const HelperFunctions& helperFunctions) {
	std::string fullPath = "system\\anims\\";
	fullPath = fullPath + name + ".saanim";

	AnimationFile* anm = new AnimationFile(helperFunctions.GetReplaceablePath(fullPath.c_str()));

	if (anm->getmodelcount() == 0) {
		delete anm;
		*info = nullptr;
	}
	else {
		*info = anm;
	}
};

inline AnimationFile* LoadANM(const char* type, const char* name) {
	std::string fullPath = "system\\";
	fullPath = fullPath + type + "\\" + name + ".saanim";

	AnimationFile* temp = new AnimationFile(HelperFunctionsGlobal.GetReplaceablePath(fullPath.c_str()));

	if (temp->getmodelcount()) {
		PrintDebug("Done.\n");
		return temp;
	}
	else {
		PrintDebug("Failed.\n");
		delete temp;
		return nullptr;
	}
}

AnimationFile* LoadObjectAnim(const char* name) {
	PrintDebug("[Klonoa Mod] Loading object animation: %s... ", name);
	return LoadANM("anims", name);
}

AnimationFile* LoadEventAnim(const char* name) {
	PrintDebug("[Klonoa Mod] Loading event animation: %s... ", name);
	return LoadANM("anims\\events", name);
}

void DrawChunkModel(NJS_CNK_MODEL* model) {
	DrawChunkModel_(model->vlist, model->plist);
}

void njCnkAction_Queue(NJS_ACTION* action, float frame, QueuedModelFlagsB flags) {
	DisplayAnimationFrame(action, frame, flags, 0, (void(__cdecl*)(NJS_MODEL_SADX*, int, int))DrawChunkModel);
}

void njCnkAction(NJS_ACTION* action, float frame) {
	DisplayAnimationFrame(action, frame, (QueuedModelFlagsB)0, 0, (void(__cdecl*)(NJS_MODEL_SADX*, int, int))DrawChunkModel);
}

void NullModel(NJS_MODEL_SADX*, int, int)
{

}

void njNullAction(NJS_ACTION* action, float frame)
{
	DisplayAnimationFrame(action, frame, (QueuedModelFlagsB)0, 0, NullModel);
}

void LookAt(NJS_VECTOR* unit, Angle* outx, Angle* outy) {
	if (outy) {
		*outy = static_cast<Angle>(atan2f(unit->x, unit->z) * 65536.0f * 0.1591549762031479f);
	}

	if (outx) {
		Float len = 1.0f / squareroot(unit->z * unit->z + unit->x * unit->x + unit->y * unit->y);

		*outx = static_cast<Angle>((acos(len * 3.3499999f) * 65536.0f * 0.1591549762031479f)
			- (acos(-(len * unit->y)) * 65536.0f * 0.1591549762031479f));
	}
}


void LookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy) {
	if (!from || !to)
		return;

	NJS_VECTOR unit = *to;

	njSubVector(&unit, from);

	if (outy) {
		*outy = static_cast<Angle>(atan2f(unit.x, unit.z) * 65536.0f * 0.1591549762031479f);
	}

	if (outx) {
		if (from->y == to->y) {
			*outx = 0;
		}
		else {
			Float len = 1.0f / squareroot(unit.z * unit.z + unit.x * unit.x + unit.y * unit.y);

			*outx = static_cast<Angle>((acos(len * 3.3499999f) * 65536.0f * 0.1591549762031479f)
				- (acos(-(len * unit.y)) * 65536.0f * 0.1591549762031479f));
		}
	}
}

void PlayerLookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy) {
	LookAt(from, to, outx, outy);

	if (outy) {
		*outy = -(*outy) + 0x4000;
	}
}

void njRotateX_(Angle x) {
	if (x) {
		njRotateX(_nj_current_matrix_ptr_, x);
	}
}

void njRotateY_(Angle y) {
	if (y) {
		njRotateY(_nj_current_matrix_ptr_, y);
	}
}

void njRotateZ_(Angle z) {
	if (z) {
		njRotateZ(_nj_current_matrix_ptr_, z);
	}
}

static const void* const actionLinkExPtr = (void*)0x406C40;
static inline void late_ActionLinkEx_(NJS_ACTION_LINK* action, float frameNumber, QueuedModelFlagsB flags, float scale, void(__cdecl* callback)(NJS_MODEL_SADX*, int, int))
{
	__asm
	{
		push[callback]
		push[scale]
		push dword ptr[flags]
		push[frameNumber]
		mov eax, [action]
		call actionLinkExPtr
		add esp, 16
	}
}

void njCnkActionLink(NJS_ACTION_LINK* action, float frame, int flag)
{
	late_ActionLinkEx_(action, frame, (QueuedModelFlagsB)flag, 0.0f, (void(__cdecl*)(NJS_MODEL_SADX*, int, int))DrawChunkModel);
}

float GetSquare(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return powf(dest->x - orig->x, 2) + powf(dest->y - orig->y, 2) + powf(dest->z - orig->z, 2);
}

float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return sqrtf(GetSquare(orig, dest));
}


bool IsPointInsideSphere(NJS_VECTOR* center, NJS_VECTOR* pos, float radius) {
	return GetDistance(center, pos) <= radius;
}

int IsPlayerInsideSphere_(NJS_VECTOR* center, float radius) {
	for (uint8_t player = 0; player < 8; ++player) {
		if (!EntityData1Ptrs[player]) 
			continue;

		if (IsPointInsideSphere(center, &EntityData1Ptrs[player]->Position, radius)) {
			return player + 1;
		}
	}

	return 0;
}

void SetVectorDiff(NJS_VECTOR* des)
{
	des->x *= 0.2f;
	des->y *= 0.2f;
	des->z *= 0.2f;
}

void RegularPhysicsFunctions(taskwk* data, motionwk2* data2, playerwk* co2)
{
	PGetSpeed(data, data2, co2);
	PSetPosition(data, data2, co2);
	PResetPosition(data, data2, co2);
}

bool isOnSnowBoard(int curAnim)
{
	if (curAnim >= 103 && curAnim <= 124)
	{
		return true;
	}

	return false;
}

//fucking kill me
const float stdPos = 5.2f;
float kloGetPosYDiff(int curAnim)
{
	if (curAnim == 102)
	{
		return 6.8f;
	}
	else if (isOnSnowBoard(curAnim))
	{
		return 8.8f;
	}

	return stdPos;
}

bool hasLightShoes(char pnum)
{
	if (!playerpwp[pnum])
		return false;

	return (playerpwp[pnum]->equipment & Upgrades_LightShoes) != 0;
}

bool isBossLevel()
{
	return (CurrentLevel >= LevelIDs_Chaos0 && CurrentLevel <= LevelIDs_E101R);
}

void SetupWorldMatrix() {
	WorldMatrixBackup = WorldMatrix;
	ProjectToWorldSpace();
	Direct3D_SetWorldTransform();
	memcpy(_nj_current_matrix_ptr_, &ViewMatrix, sizeof(NJS_MATRIX));
}

void SetupChunkModelRender()
{
	SetupWorldMatrix();
	Direct3D_SetChunkModelRenderState();
}

void ResetChunkModelRender()
{
	WorldMatrix = WorldMatrixBackup;
	Direct3D_ResetWorldTransform();
	Direct3D_UnsetChunkModelRenderState();
}

bool isSuper(char pnum)
{
	if (!isKlonoa(pnum) || !playerpwp[pnum])
		return false;

	return playerpwp[pnum]->item & Upgrades_SuperSonic || CurrentLevel == LevelIDs_PerfectChaos;
}