#include "pch.h"

AnimationFile* KlonoaANM[30] = { 0 };
#define AnimCount 160
#define boneCount 48
AnimData_t KlonoaAnimList[AnimCount] = { 0 };
AnimData_t MetalAnimList[AnimCount];

int loc_49AB51 = 0x49AB51;
__declspec(naked) void SetAnimList()
{
	__asm
	{
		mov eax, [MetalSonicFlag]
		mov al, [eax]
		test al, al
		jnz metal
		mov dword ptr[ebp + 140h], offset KlonoaAnimList
		jmp loc_49AB51
		metal :
		mov dword ptr[ebp + 140h], offset MetalAnimList
			jmp loc_49AB51
	}
}

void SetKlonoaAnims()
{
	//Init new anim list, put falling animation as a placeholder / failsafe for animation that don't have a SADX counterpart
	for (int i = 0; i < LengthOfArray(KlonoaAnimList); i++)
	{
		KlonoaAnimList[i].Animation = new NJS_ACTION;
		KlonoaAnimList[i].Animation->object = KlonoaMDL->getmodel();
		KlonoaAnimList[i].Animation->motion = KlonoaANM[anmID_fall]->getmotion();
		KlonoaAnimList[i].Instance = KlonoaANM[anmID_fall]->getmodelcount();
		KlonoaAnimList[i].Property = 4;
		KlonoaAnimList[i].NextAnim = 18;
		KlonoaAnimList[i].TransitionSpeed = 0.25f;
		KlonoaAnimList[i].AnimationSpeed = 0.5f;
	}


	KlonoaAnimList[0].Animation->motion = KlonoaANM[anmID_stand]->getmotion();
	KlonoaAnimList[0].AnimationSpeed = 0.5f;
	KlonoaAnimList[0].NextAnim = 0;
	KlonoaAnimList[0].Property = 3;
	KlonoaAnimList[0].TransitionSpeed = 0.0625f;

	//surf stance
	KlonoaAnimList[1].Animation->motion = KlonoaANM[anmID_battleStd]->getmotion();
	KlonoaAnimList[1].TransitionSpeed = 0.25f;
	KlonoaAnimList[1].AnimationSpeed = 0.50f;
	KlonoaAnimList[1].Property = 4;
	KlonoaAnimList[1].NextAnim = 2;

	KlonoaAnimList[2].Animation->motion = KlonoaANM[anmID_battleStd]->getmotion();
	KlonoaAnimList[2].TransitionSpeed = 0.25f;
	KlonoaAnimList[2].AnimationSpeed = 0.50f;
	KlonoaAnimList[2].Property = 4;
	KlonoaAnimList[2].NextAnim = 2;

	//walk
	KlonoaAnimList[9].Animation->motion = KlonoaANM[anmID_walk]->getmotion();
	KlonoaAnimList[9].AnimationSpeed = 1.5f;
	KlonoaAnimList[9].NextAnim = 9;
	KlonoaAnimList[9].Property = 10;
	KlonoaAnimList[9].TransitionSpeed = 0.15f;

	//speed walk
	KlonoaAnimList[10].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	KlonoaAnimList[10].AnimationSpeed = 0.3f;
	KlonoaAnimList[10].NextAnim = 10;
	KlonoaAnimList[10].Property = 10;
	KlonoaAnimList[10].TransitionSpeed = 0.20f;

	//jogging
	KlonoaAnimList[11].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	KlonoaAnimList[11].Property = 10;
	KlonoaAnimList[11].NextAnim = 11;
	KlonoaAnimList[11].TransitionSpeed = 0.25f;
	KlonoaAnimList[11].AnimationSpeed = 0.2f;

	//run
	KlonoaAnimList[12].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	KlonoaAnimList[12].Property = 10;
	KlonoaAnimList[12].NextAnim = 12;
	KlonoaAnimList[12].TransitionSpeed = 0.25f;
	KlonoaAnimList[12].AnimationSpeed = 0.2f;

	//sprint
	KlonoaAnimList[13].Animation->motion = KlonoaANM[anmID_run]->getmotion();
	KlonoaAnimList[13].Property = 9;
	KlonoaAnimList[13].NextAnim = 13;
	KlonoaAnimList[13].TransitionSpeed = 0.5f;
	KlonoaAnimList[13].AnimationSpeed = 0.2f;

	//falling
	KlonoaAnimList[18].Animation->motion = KlonoaANM[anmID_fall]->getmotion();
	KlonoaAnimList[18].Property = 3;
	KlonoaAnimList[18].NextAnim = 18;
	KlonoaAnimList[18].TransitionSpeed = 0.25f;
	KlonoaAnimList[18].AnimationSpeed = 0.5f;

	//idle
	KlonoaAnimList[4].Animation->motion = KlonoaANM[anmID_idle]->getmotion();
	KlonoaAnimList[4].Property = 4;
	KlonoaAnimList[4].NextAnim = 5;
	KlonoaAnimList[4].TransitionSpeed = 0.25f;
	KlonoaAnimList[4].AnimationSpeed = 0.5f;

	KlonoaAnimList[5].Animation->motion = KlonoaANM[anmID_idle]->getmotion();
	KlonoaAnimList[5].Property = 4;
	KlonoaAnimList[5].NextAnim = 6;
	KlonoaAnimList[5].TransitionSpeed = 0.25f;
	KlonoaAnimList[5].AnimationSpeed = 0.5f;

	KlonoaAnimList[6].Animation->motion = KlonoaANM[anmID_idle]->getmotion();
	KlonoaAnimList[6].Property = 4;
	KlonoaAnimList[6].NextAnim = 0;
	KlonoaAnimList[6].TransitionSpeed = 0.25f;
	KlonoaAnimList[6].AnimationSpeed = 1.0f;

	//jump
	KlonoaAnimList[14].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	KlonoaAnimList[14].Property = 10;
	KlonoaAnimList[14].NextAnim = 14;
	KlonoaAnimList[14].TransitionSpeed = 0.25f;
	KlonoaAnimList[14].AnimationSpeed = 0.07f;

	//hang pulley / heli
	KlonoaAnimList[47].Animation->motion = KlonoaANM[anmID_holdOne]->getmotion();
	KlonoaAnimList[47].Property = 3;
	KlonoaAnimList[47].NextAnim = 47;
	KlonoaAnimList[47].TransitionSpeed = 0.5f;
	KlonoaAnimList[47].AnimationSpeed = 0.5f;

	//idle hold tree
	KlonoaAnimList[54].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[54].Property = 3;
	KlonoaAnimList[54].NextAnim = 54;
	KlonoaAnimList[54].TransitionSpeed = 0.5f;
	KlonoaAnimList[54].AnimationSpeed = 0.5f;

	//hold obj
	KlonoaAnimList[62].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[62].Property = 4;
	KlonoaAnimList[62].NextAnim = 18;
	KlonoaAnimList[62].TransitionSpeed = 0.5;
	KlonoaAnimList[62].AnimationSpeed = 0.5f;

	//hold rocket
	KlonoaAnimList[80].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[80].NextAnim = 80;
	KlonoaAnimList[80].TransitionSpeed = 0.125;
	KlonoaAnimList[80].AnimationSpeed = 1.0f;

	//hover
	KlonoaAnimList[anm_hover].Animation->motion = KlonoaANM[anmID_hover]->getmotion();
	KlonoaAnimList[anm_hover].Property = 3;
	KlonoaAnimList[anm_hover].NextAnim = anm_hover;
	KlonoaAnimList[anm_hover].TransitionSpeed = 0.25f;
	KlonoaAnimList[anm_hover].AnimationSpeed = 0.5f;

	//bullet start
	KlonoaAnimList[anm_windBullet].Animation->motion = KlonoaANM[anmID_bulletStart]->getmotion();
	KlonoaAnimList[anm_windBullet].Property = 4;
	KlonoaAnimList[anm_windBullet].NextAnim = anm_windBulletEnd;
	KlonoaAnimList[anm_windBullet].TransitionSpeed = 0.25f;
	KlonoaAnimList[anm_windBullet].AnimationSpeed = 0.5f;

	//bullet mid air
	KlonoaAnimList[anm_windBulletAir].Animation->motion = KlonoaANM[animID_bulletMidAir]->getmotion();
	KlonoaAnimList[anm_windBulletAir].Property = 4;
	KlonoaAnimList[anm_windBulletAir].NextAnim = anm_fall;
	KlonoaAnimList[anm_windBulletAir].TransitionSpeed = 0.25f;
	KlonoaAnimList[anm_windBulletAir].AnimationSpeed = 0.5f;

	//bullet end
	KlonoaAnimList[anm_windBulletEnd].Animation->motion = KlonoaANM[animID_bulletEnd]->getmotion();
	KlonoaAnimList[anm_windBulletEnd].Property = 4;
	KlonoaAnimList[anm_windBulletEnd].NextAnim = 0;
	KlonoaAnimList[anm_windBulletEnd].TransitionSpeed = 0.25f;
	KlonoaAnimList[anm_windBulletEnd].AnimationSpeed = 0.5f;

	//hold enemy std
	KlonoaAnimList[anm_holdStd].Animation->motion = KlonoaANM[animID_holdStd]->getmotion();
	KlonoaAnimList[anm_holdStd].Property = 3;
	KlonoaAnimList[anm_holdStd].NextAnim = anm_holdStd;
	KlonoaAnimList[anm_holdStd].TransitionSpeed = 0.25f;
	KlonoaAnimList[anm_holdStd].AnimationSpeed = 0.5f;

	//hold enemy wlk
	KlonoaAnimList[anm_holdWalk].Animation->motion = KlonoaANM[anmID_holdWalk]->getmotion();
	KlonoaAnimList[anm_holdWalk].NextAnim = anm_holdWalk;
	KlonoaAnimList[anm_holdWalk].AnimationSpeed = 0.3f;
	KlonoaAnimList[anm_holdWalk].NextAnim = anm_holdWalk;
	KlonoaAnimList[anm_holdWalk].Property = 10;

	//hold enemy run
	KlonoaAnimList[anm_holdRun].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	KlonoaAnimList[anm_holdRun].NextAnim = anm_holdRun;
	KlonoaAnimList[anm_holdRun].AnimationSpeed = 0.2f;
	KlonoaAnimList[anm_holdRun].NextAnim = anm_holdRun;
	KlonoaAnimList[anm_holdRun].Property = 10;

	//hold enemy jump
	KlonoaAnimList[anm_holdJump].Animation->motion = KlonoaANM[anmID_holdJump]->getmotion();
	KlonoaAnimList[anm_holdJump].NextAnim = anm_holdJump;
	KlonoaAnimList[anm_holdJump].AnimationSpeed = 0.1f;
	KlonoaAnimList[anm_holdJump].NextAnim = anm_holdJump;
	KlonoaAnimList[anm_holdJump].Property = 10;

	//hold enemy fall
	KlonoaAnimList[anm_holdFall].Animation->motion = KlonoaANM[anmID_holdFall]->getmotion();
	KlonoaAnimList[anm_holdFall].NextAnim = anm_holdFall;
	KlonoaAnimList[anm_holdFall].AnimationSpeed = 0.1f;
	KlonoaAnimList[anm_holdFall].NextAnim = anm_holdFall;
	KlonoaAnimList[anm_holdFall].Property = 3;

	//throw enemy
	KlonoaAnimList[anm_throwStd].Animation->motion = KlonoaANM[anmID_throw]->getmotion();
	KlonoaAnimList[anm_throwStd].NextAnim = anm_throwStd;
	KlonoaAnimList[anm_throwStd].AnimationSpeed = 0.7f;
	KlonoaAnimList[anm_throwStd].NextAnim = anm_std;
	KlonoaAnimList[anm_throwStd].Property = 4;

	//victory.
	KlonoaAnimList[75].Animation->motion = KlonoaANM[anmID_victory]->getmotion();
	KlonoaAnimList[75].Property = 4;
	KlonoaAnimList[75].NextAnim = 76;
	KlonoaAnimList[75].TransitionSpeed = 0.5f;
	KlonoaAnimList[75].AnimationSpeed = 0.5f;

	//victory standing.
	KlonoaAnimList[76].Animation->motion = KlonoaANM[anmID_victoryStd]->getmotion();
	KlonoaAnimList[76].NextAnim = 0;
	KlonoaAnimList[76].Property = 3;
	KlonoaAnimList[76].TransitionSpeed = 0.5f;
	KlonoaAnimList[76].AnimationSpeed = 0.5f;
}

void LoadKlonoa_AnimFiles()
{
	KlonoaANM[0] = LoadObjectAnim("stand");
	KlonoaANM[1] = LoadObjectAnim("death");
	KlonoaANM[2] = LoadObjectAnim("fall");
	KlonoaANM[3] = LoadObjectAnim("walk");
	KlonoaANM[4] = LoadObjectAnim("fly");
	KlonoaANM[5] = LoadObjectAnim("hover");
	KlonoaANM[6] = LoadObjectAnim("run");
	KlonoaANM[7] = LoadObjectAnim("idle");
	KlonoaANM[8] = LoadObjectAnim("jump");
	KlonoaANM[9] = LoadObjectAnim("holdOneHand");
	KlonoaANM[10] = LoadObjectAnim("holdObj");
	KlonoaANM[11] = LoadObjectAnim("Victory");
	KlonoaANM[12] = LoadObjectAnim("Victory2");
	KlonoaANM[13] = LoadObjectAnim("standVictory");
	KlonoaANM[14] = LoadObjectAnim("standBattle");
	KlonoaANM[15] = LoadObjectAnim("bulletStart");
	KlonoaANM[16] = LoadObjectAnim("bulletMidAir");
	KlonoaANM[17] = LoadObjectAnim("bulletEnd");
	KlonoaANM[18] = LoadObjectAnim("holdStand");
	KlonoaANM[19] = LoadObjectAnim("holdWalk");
	KlonoaANM[20] = LoadObjectAnim("holdRun");
	KlonoaANM[21] = LoadObjectAnim("holdJump");
	KlonoaANM[22] = LoadObjectAnim("holdFall");
	KlonoaANM[23] = LoadObjectAnim("throw");
}

void InitKlonoaCharSelAnim()
{
	SonicCharSelAnim[0] = KlonoaAnimList[11].Animation;
	SonicCharSelAnim[1] = KlonoaAnimList[75].Animation;
	SonicCharSelAnim[2] = KlonoaAnimList[76].Animation;
	SonicCharSelAnim[3] = 0;
}

void Init_KlonoaAnim()
{
	LoadKlonoa_AnimFiles();
	SetKlonoaAnims();
	WriteJump((void*)0x49AB47, SetAnimList);
	WriteData((short*)0x49ACD8, (short)0x9090);
	WriteData<2>((void*)0x4916A5, 0x90u); // disable metal's weird tilting thing
	WriteData((char*)0x49BE22, (char)0xEB);
	WriteJump(InitSonicCharSelAnims, InitKlonoaCharSelAnim);
}