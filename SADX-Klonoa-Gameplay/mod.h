#pragma once

void ReadConfig(const char* path, const HelperFunctions& helperFunctions);

extern HelperFunctions HelperFunctionsGlobal;

void init_Audio();
void initKlonoa();
void init_KlonoaModelsAnim();
void FreeObjModels();

extern ModelInfo* KlonoaMDL;


#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())
#define charID counter.b[1]
#define charIndex counter.b[0]

enum klonoaAnim
{
	anm_stand,
	anm_death,
	anm_fall,
	anm_walk,
	anm_fly,
	anm_hover,
	anm_run,
	anm_idle,
	anm_jump,
	anm_holdOne,
	anm_holdObj,
	anm_victory,
	anm_victory2,
	anm_victoryStd,
	anm_battleStd,

};

enum klonoaAction
{
	act_hover = 110,
	act_super_jump
};