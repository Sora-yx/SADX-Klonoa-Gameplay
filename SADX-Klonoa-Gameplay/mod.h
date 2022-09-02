#pragma once

void ReadConfig(const char* path, const HelperFunctions& helperFunctions);

extern HelperFunctions HelperFunctionsGlobal;

void init_Audio();
void initKlonoa();
void init_KlonoaModelsAnim();
void FreeObjModels();

extern std::string modpath;

extern ModelInfo* KlonoaMDL;

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())
#define charID counter.b[1]
#define charIndex counter.b[0]

extern bool allowHomingAttack;
extern bool allowSpinDash;

struct klonoawk
{
	int generalTimer;
	int hoverTimer;
	char bulletTimer;
	char superJumpCount;
	task* enemyGrabPtr;
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
	animID_bulletEnd

};

enum klonoaAnim //regular ID used to play anim on the character
{
	anm_fall = 18,
	anm_hover = 147,
	anm_windBullet,
	anm_windBulletAir,
	anm_windBulletEnd
};

enum klonoaAction
{
	act_stnd = 1,
	act_fall = 12,
	act_hover = 110,
	act_windBullet,
	act_windBulletAir,
	act_super_jump,
};