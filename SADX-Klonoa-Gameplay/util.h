#pragma once

ModelInfo* LoadBasicModel(const char* name);

ModelInfo* LoadChunkModel(const char* name);

void LoadModelListFuncPtr(const char** names, int count, ModelInfo** mdls, ModelInfo* (*func)(const char*));

void FreeMDL(ModelInfo* pointer);

void LoadAnimation(AnimationFile** info, const char* name, const HelperFunctions& helperFunctions);

AnimationFile* LoadEventAnim(const char* name);

inline AnimationFile* LoadANM(const char* type, const char* name);
AnimationFile* LoadObjectAnim(const char* name);
void LookAt(NJS_VECTOR* unit, Angle* outx, Angle* outy);
void LookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy);
void PlayerLookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy);

int getKlonoaPlayer();

bool isKlonoa(char pnum);
void njCnkAction_Queue(NJS_ACTION* action, float frame, QueuedModelFlagsB flags);
void njCnkAction(NJS_ACTION* action, float frame);
void SetupWorldMatrix();

void njNullAction(NJS_ACTION* action, float frame);

void njRotateX_(Angle x);
void njRotateY_(Angle y);
void njRotateZ_(Angle z);

void njCnkActionLink(NJS_ACTION_LINK* action, float frame, int flag);

float GetSquare(NJS_VECTOR* orig, NJS_VECTOR* dest);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
bool IsPointInsideSphere(NJS_VECTOR* center, NJS_VECTOR* pos, float radius);
int IsPlayerInsideSphere_(NJS_VECTOR* center, float radius);
void SetVectorDiff(NJS_VECTOR* des);
bool isKlonoaHold(char pnum);
void RegularPhysicsFunctions(taskwk* data, motionwk2* data2, playerwk* co2);
float kloGetPosYDiff(int curAnim);
bool hasLightShoes(char pnum);
bool isBossLevel();
void SetupChunkModelRender();
void ResetChunkModelRender();
bool isSuper(char pnum);
AnimData_t* GetKlonoaAnimList();
void ReplaceSonicAnimPtr();
bool isTailsRace(char pnum);
colaround GetClosestEnemy(NJS_VECTOR* pos);
