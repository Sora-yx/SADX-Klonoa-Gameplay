#pragma once

extern HelperFunctions HelperFunctionsGlobal;
extern bool klonoaPnum[];
extern bool MultiModEnabled;
extern std::string modpath;

extern ModelInfo* KlonoaMDL;
extern ModelInfo* SuperKlonoaMDL;

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())
#define charID counter.b[1]
#define charIndex counter.b[0]
#define TaskHook FunctionHook<void, task*>
constexpr char PMax = 8;

extern bool allowKlonoaMoves;
extern bool hud;
extern bool obj;
extern bool useHP;
extern bool infiniteHover;
extern uint8_t hpMaxConfig;

extern const NJS_VECTOR KLScaleDiff;

extern NJS_TEXLIST KlonoaTexList;
extern const char enemyArraySize;

struct klonoawk
{
	int generalTimer;
	int hoverTimer;
	bool hoverUsed;
	bool bulletShot;
	char superJumpCount;
	task* currentBulletPtr;
	task* enemyGrabPtr;
	NJS_VECTOR ringPos;
	NJS_VECTOR ringVec;
};


enum klonoaAction
{
	act_stnd = 1,
	act_walk,
	act_jump = 8,
	act_fall = 12,
	act_brake,
	act_hurt = 16,
	act_hover = 110,
	act_windBullet,
	act_windBulletAir,
	act_super_jump,
	act_holdStd,
	act_holdRun,
	act_holdJump,
	act_holdFall,
	act_throwStd,
	act_throwAir,
	act_flyStd,
	act_fly
};

enum enemyState
{
	captured = 15,
	dropSetup,
	drop,
	throwSetup,
	threw,
	dead
};

enum charBossState
{
	Bcaptured = 100,
	Bcaptured2,
	Bdrop,
	BthrowSetup,
	Bthrew,
	bDone,
};

void ReadConfig(const char* path, const HelperFunctions& helperFunctions);
void init_Audio();
void initKlonoa();
void init_EnemiesHack();
void Init_KlonoaAnim();
void init_BossesHacks();
void init_Patches();
void init_Hud();
void initKlonoaHP();
void Tornado_init();
void InitSuperKlonoa(HMODULE h);
void KlonoaManageVictoryMotion(playerwk* co2);