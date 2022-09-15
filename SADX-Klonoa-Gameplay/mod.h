#pragma once

extern HelperFunctions HelperFunctionsGlobal;
extern uint8_t klonoaPnum;

extern std::string modpath;

extern ModelInfo* KlonoaMDL;

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())
#define charID counter.b[1]
#define charIndex counter.b[0]
#define TaskHook static FunctionHook<void, task*>

extern bool allowHomingAttack;
extern bool allowSpinDash;
extern bool hud;
extern bool obj;

extern const NJS_VECTOR KLScaleDiff;

struct klonoawk
{
	int generalTimer;
	int hoverTimer;
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
};

enum klonoaAnim //regular ID used to play anim on the character
{
	anm_std,
	anm_jump = 14,
	anm_fall = 18,
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
};

enum enemyState
{
	captured = 15,
	drop,
	throwSetup,
	threw,
	dead

};

void ReadConfig(const char* path, const HelperFunctions& helperFunctions);
void init_Audio();
void initKlonoa();
void init_EnemiesHack();
void Init_KlonoaAnim();
void init_BossesHacks();
void ObjectAnimAdjust();