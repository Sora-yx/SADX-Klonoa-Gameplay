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
extern uint8_t difficulty;

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



enum klonoaAnimDLL //dll is for the array on init
{
	anmID_stand,
	anmID_death,
	anmID_fall,
	anmID_walk,
	anmID_fly,
	anmID_hover,
	anmID_run,
	anmID_idle,
	anmID_jump,
	anmID_holdOne,
	anmID_holdObj,
	anmID_victory,
	anmID_victory2,
	anmID_victoryStd,
	anmID_battleStd,
	anmID_bulletStart,
	animID_bulletMidAir,
	animID_bulletEnd,
	animID_holdStd,
	anmID_holdWalk,
	anmID_holdRun,
	anmID_holdJump,
	anmID_holdFall,
	anmID_throw,
	anmID_throwAir,
	animID_Brake,
	animID_Landing,
	animID_Board,
	animID_Crouch,
	animID_FallBoard,
	animID_trick1,
	animID_trick2,
	animID_trick3,
	animID_BoardLeft,
	animID_BoardRight,
	animID_BoardJump,
	anmID_flyStd,
	anmID_cart,
	anmID_holdIcicle,
	anmID_Climbing,
	anmID_spinning,
	anmID_jmpPanel,
	anmID_push,
	anmID_hurt,
	anmID_holdRocketV,
	anmID_shower,
	anmID_shower2,
	anmID_whistle,
	anmID_PetSet,
	anmID_shake,
};

enum klonoaAnim //regular ID used to play anim on the character
{
	anm_std,
	anm_bStance0,
	anm_bStance1,
	anm_walk = 9,
	anm_walk2,
	anm_walk3,
	anm_run,
	anm_run2,
	anm_jump = 14,
	anm_fall = 18,
	anm_brake = 20,
	anm_hurt = 23,
	anm_turnAround = 30,
	anm_fly = 64,
	anm_cart,
	anm_climb = 69,
	anm_board = 102,
	anm_brakeBoard,
	anm_brakeLeftBoard,
	anm_fallBoardJump,
	anm_BoardJump,
	anm_BoardFall = 112,
	anm_boardStd = 114,
	anm_trick01 = 116,
	anm_trick02,
	anm_trick03,
	anm_trick04,
	anm_crouchBoard = 124,
	anm_setPet = 130,
	anm_pet,
	anm_petStop,
	anm_whistle,
	anm_hover = 147,
	anm_windBullet,
	anm_windBulletAir,
	anm_windBulletEnd,
	anm_holdStd,
	anm_holdWalk,
	anm_holdRun,
	anm_holdJump,
	anm_holdFall,
	anm_throwStd,
	anm_throwAir,
	anm_flyStd,
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

enum difficultyE
{
	easy,
	normal,
	hard
};

void ReadConfig(const char* path, const HelperFunctions& helperFunctions);
void init_Audio();
void initKlonoa();
void init_EnemiesHack();
void Init_KlonoaAnim();
void init_BossesHacks();
void PickDrop_Patches();
void init_Hud();
void initKlonoaHP();
void Tornado_init();
void InitSuperKlonoa(HMODULE h);
void KlonoaManageVictoryMotion(playerwk* co2);