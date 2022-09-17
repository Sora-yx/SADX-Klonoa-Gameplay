#include "pch.h"

AnimationFile* KlonoaANM[50] = { 0 };
AnimationFile* KlonoaEvANM[50] = { 0 };
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

	KlonoaAnimList[7] = KlonoaAnimList[0];
	KlonoaAnimList[8] = KlonoaAnimList[0];

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

	//spring
	KlonoaAnimList[16].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	KlonoaAnimList[16].Property = 3;
	KlonoaAnimList[16].NextAnim = 16;;
	KlonoaAnimList[16].TransitionSpeed = 0.25f;
	KlonoaAnimList[16].AnimationSpeed = 0.5f;	

	//landing
	KlonoaAnimList[19].Animation->motion = KlonoaANM[animID_Landing]->getmotion();
	KlonoaAnimList[19].Property = 4;
	KlonoaAnimList[19].NextAnim = 0;
	KlonoaAnimList[19].TransitionSpeed = 0.25f;
	KlonoaAnimList[19].AnimationSpeed = 0.5f;

	//brake
	KlonoaAnimList[20].Animation->motion = KlonoaANM[animID_Brake]->getmotion();
	KlonoaAnimList[20].Property = 4;
	KlonoaAnimList[20].NextAnim = 0;
	KlonoaAnimList[20].TransitionSpeed = 0.25f;
	KlonoaAnimList[20].AnimationSpeed = 0.8f;
	
	//pick obj (bullet for now)
	KlonoaAnimList[33].Animation->motion = KlonoaANM[anmID_bulletStart]->getmotion();
	KlonoaAnimList[33].Property = 4;
	KlonoaAnimList[33].NextAnim = 37;
	KlonoaAnimList[33].TransitionSpeed = 0.25f;
	KlonoaAnimList[33].AnimationSpeed = 0.4f;

	//obj run
	KlonoaAnimList[34].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	KlonoaAnimList[34].NextAnim = 34;
	KlonoaAnimList[34].AnimationSpeed = 0.5f;
	KlonoaAnimList[34].Property = 3;

	//obj stand
	KlonoaAnimList[37].Animation->motion = KlonoaANM[animID_holdStd]->getmotion();
	KlonoaAnimList[37].Property = 3;
	KlonoaAnimList[37].NextAnim = 37;
	KlonoaAnimList[37].TransitionSpeed = 0.25f;
	KlonoaAnimList[37].AnimationSpeed = 0.5f;

	//obj walk
	KlonoaAnimList[38].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	KlonoaAnimList[38].NextAnim = 38;
	KlonoaAnimList[38].AnimationSpeed = 0.3f;
	KlonoaAnimList[38].Property = 3;

	//trampoline
	KlonoaAnimList[39].Animation->motion = KlonoaANM[anmID_jump]->getmotion();
	KlonoaAnimList[39].Property = 3;
	KlonoaAnimList[39].NextAnim = 16;
	KlonoaAnimList[39].TransitionSpeed = 0.25f;
	KlonoaAnimList[39].AnimationSpeed = 0.5f;

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


	//object jump
	KlonoaAnimList[55].Animation->motion = KlonoaANM[anmID_holdJump]->getmotion();
	KlonoaAnimList[55].NextAnim = 55;
	KlonoaAnimList[55].AnimationSpeed = 0.5f;
	KlonoaAnimList[55].Property = 3;


	//object fall

	KlonoaAnimList[56].Animation->motion = KlonoaANM[anmID_holdFall]->getmotion();
	KlonoaAnimList[56].NextAnim = 57;
	KlonoaAnimList[56].AnimationSpeed = 0.5f;
	KlonoaAnimList[56].Property = 4;

	KlonoaAnimList[57].Animation->motion = KlonoaANM[anmID_holdFall]->getmotion();
	KlonoaAnimList[57].NextAnim = 57;
	KlonoaAnimList[57].AnimationSpeed = 0.5f;
	KlonoaAnimList[57].Property = 3;

	//Throw object
	KlonoaAnimList[59].Animation->motion = KlonoaANM[anmID_throw]->getmotion();
	KlonoaAnimList[59].NextAnim = anm_std;
	KlonoaAnimList[59].AnimationSpeed = 0.2f;
	KlonoaAnimList[59].Property = 4;

	KlonoaAnimList[60] = KlonoaAnimList[59];

	KlonoaAnimList[61] = KlonoaAnimList[59];
	KlonoaAnimList[61].NextAnim = 18;


	//hold obj
	KlonoaAnimList[62].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[62].Property = 4;
	KlonoaAnimList[62].NextAnim = 18;
	KlonoaAnimList[62].TransitionSpeed = 0.5;
	KlonoaAnimList[62].AnimationSpeed = 0.5f;

	KlonoaAnimList[63] = KlonoaAnimList[59];

	//Light Dash
	KlonoaAnimList[64].Animation->motion = KlonoaANM[anmID_fly]->getmotion();
	KlonoaAnimList[64].NextAnim = 31;
	KlonoaAnimList[64].TransitionSpeed = 0.125;
	KlonoaAnimList[64].AnimationSpeed = 1.0f;
	KlonoaAnimList[64].Property = 3;


	//shot from cannon
	KlonoaAnimList[70].Animation->motion = KlonoaANM[anmID_fly]->getmotion();
	KlonoaAnimList[70].NextAnim = 70;
	KlonoaAnimList[70].TransitionSpeed = 0.125;
	KlonoaAnimList[70].AnimationSpeed = 1.0f;
	KlonoaAnimList[70].Property = 3;


	//victory.
	KlonoaAnimList[75].Animation->motion = KlonoaANM[anmID_victory]->getmotion();
	KlonoaAnimList[75].Property = 5;
	KlonoaAnimList[75].NextAnim = 75;
	KlonoaAnimList[75].TransitionSpeed = 0.5f;
	KlonoaAnimList[75].AnimationSpeed = 0.5f;

	//victory standing.
	KlonoaAnimList[76].Animation->motion = KlonoaANM[anmID_victoryStd]->getmotion();
	KlonoaAnimList[76].NextAnim = 0;
	KlonoaAnimList[76].Property = 3;
	KlonoaAnimList[76].TransitionSpeed = 0.5f;
	KlonoaAnimList[76].AnimationSpeed = 0.5f;

	//hold rocket
	KlonoaAnimList[80].Animation->motion = KlonoaANM[anmID_holdObj]->getmotion();
	KlonoaAnimList[80].NextAnim = 80;
	KlonoaAnimList[80].TransitionSpeed = 0.125;
	KlonoaAnimList[80].AnimationSpeed = 1.0f;

	//begin death
	KlonoaAnimList[86].Animation->motion = KlonoaANM[anmID_death]->getmotion();
	KlonoaAnimList[86].TransitionSpeed = 0.125;
	KlonoaAnimList[86].AnimationSpeed = 1.0f;
	KlonoaAnimList[86].Property = 4;

	//death
	KlonoaAnimList[87].Animation->motion = KlonoaANM[anmID_death]->getmotion();
	KlonoaAnimList[87].NextAnim = 87;
	KlonoaAnimList[87].TransitionSpeed = 0.125;
	KlonoaAnimList[87].AnimationSpeed = 1.0f;
	KlonoaAnimList[87].Property = 5;

	//board
	KlonoaAnimList[102].Animation->motion = KlonoaANM[animID_Board]->getmotion();
	KlonoaAnimList[102].NextAnim = 102;
	KlonoaAnimList[102].TransitionSpeed = 0.125;
	KlonoaAnimList[102].AnimationSpeed = 1.0f;
	KlonoaAnimList[102].Property = 3;


	//board brake right
	KlonoaAnimList[103].Animation->motion = KlonoaANM[animID_BoardRight]->getmotion();
	KlonoaAnimList[103].NextAnim = 103;
	KlonoaAnimList[103].TransitionSpeed = 0.125;
	KlonoaAnimList[103].AnimationSpeed = 0.5f;
	KlonoaAnimList[103].Property = 5;

	//board brake left
	KlonoaAnimList[104].Animation->motion = KlonoaANM[animID_BoardLeft]->getmotion();
	KlonoaAnimList[104].NextAnim = 104;
	KlonoaAnimList[104].TransitionSpeed = 0.125;
	KlonoaAnimList[104].AnimationSpeed = 0.5f;
	KlonoaAnimList[104].Property = 5;

	KlonoaAnimList[114] = KlonoaAnimList[102];

	//falling board
	KlonoaAnimList[105].Animation->motion = KlonoaANM[animID_FallBoard]->getmotion();
	KlonoaAnimList[105].NextAnim = 111;
	KlonoaAnimList[105].TransitionSpeed = 0.125;
	KlonoaAnimList[105].AnimationSpeed = 0.2f;
	KlonoaAnimList[105].Property = 4;

	//jump board
	KlonoaAnimList[106].Animation->motion = KlonoaANM[animID_BoardJump]->getmotion();
	KlonoaAnimList[106].NextAnim = 105;
	KlonoaAnimList[106].TransitionSpeed = 0.125;
	KlonoaAnimList[106].AnimationSpeed = 0.5f;
	KlonoaAnimList[106].Property = 4;

	//board right side
	KlonoaAnimList[107].Animation->motion = KlonoaANM[animID_BoardRight]->getmotion();
	KlonoaAnimList[107].NextAnim = 107;
	KlonoaAnimList[107].TransitionSpeed = 0.125;
	KlonoaAnimList[107].AnimationSpeed = 0.3f;
	KlonoaAnimList[107].Property = 12;

	//board left side
	KlonoaAnimList[108].Animation->motion = KlonoaANM[animID_BoardLeft]->getmotion();
	KlonoaAnimList[108].NextAnim = 108;
	KlonoaAnimList[108].TransitionSpeed = 0.125;
	KlonoaAnimList[108].AnimationSpeed = 0.3f;
	KlonoaAnimList[108].Property = 12;

	KlonoaAnimList[111] = KlonoaAnimList[105];
	KlonoaAnimList[112] = KlonoaAnimList[105];

	//losing balance, don't have a counterpart so stick to neutral
	KlonoaAnimList[115] = KlonoaAnimList[102]; 

	//trick 1
	KlonoaAnimList[116].Animation->motion = KlonoaANM[animID_trick1]->getmotion();
	KlonoaAnimList[116].NextAnim = 112;
	KlonoaAnimList[116].TransitionSpeed = 0.125;
	KlonoaAnimList[116].AnimationSpeed = 0.5f;
	KlonoaAnimList[116].Property = 4;

	//trick 2
	KlonoaAnimList[117].Animation->motion = KlonoaANM[animID_trick2]->getmotion();
	KlonoaAnimList[117].NextAnim = 112;
	KlonoaAnimList[117].TransitionSpeed = 0.125;
	KlonoaAnimList[117].AnimationSpeed = 0.5f;
	KlonoaAnimList[117].Property = 4;

	//trick 3
	KlonoaAnimList[118].Animation->motion = KlonoaANM[animID_trick3]->getmotion();
	KlonoaAnimList[118].NextAnim = 112;
	KlonoaAnimList[118].TransitionSpeed = 0.125;
	KlonoaAnimList[118].AnimationSpeed = 0.5f;
	KlonoaAnimList[118].Property = 4;

	KlonoaAnimList[119] = KlonoaAnimList[118];
	KlonoaAnimList[120] = KlonoaAnimList[118];
	KlonoaAnimList[121] = KlonoaAnimList[117];

	//crouch
	KlonoaAnimList[124].Animation->motion = KlonoaANM[animID_Crouch]->getmotion();
	KlonoaAnimList[124].NextAnim = 124;
	KlonoaAnimList[124].TransitionSpeed = 0.125;
	KlonoaAnimList[124].AnimationSpeed = 0.3f;
	KlonoaAnimList[124].Property = 3;

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
	KlonoaAnimList[anm_holdWalk].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	KlonoaAnimList[anm_holdWalk].NextAnim = anm_holdWalk;
	KlonoaAnimList[anm_holdWalk].AnimationSpeed = 0.3f;
	KlonoaAnimList[anm_holdWalk].Property = 3;

	//hold enemy run
	KlonoaAnimList[anm_holdRun].Animation->motion = KlonoaANM[anmID_holdRun]->getmotion();
	KlonoaAnimList[anm_holdRun].NextAnim = anm_holdRun;
	KlonoaAnimList[anm_holdRun].AnimationSpeed = 0.5f;
	KlonoaAnimList[anm_holdRun].Property = 3;

	//hold enemy jump
	KlonoaAnimList[anm_holdJump].Animation->motion = KlonoaANM[anmID_holdJump]->getmotion();
	KlonoaAnimList[anm_holdJump].NextAnim = anm_holdJump;
	KlonoaAnimList[anm_holdJump].AnimationSpeed = 0.5f;
	KlonoaAnimList[anm_holdJump].Property = 3;

	//hold enemy fall
	KlonoaAnimList[anm_holdFall].Animation->motion = KlonoaANM[anmID_holdFall]->getmotion();
	KlonoaAnimList[anm_holdFall].NextAnim = anm_holdFall;
	KlonoaAnimList[anm_holdFall].AnimationSpeed = 0.5f;
	KlonoaAnimList[anm_holdFall].Property = 3;

	//throw enemy
	KlonoaAnimList[anm_throwStd].Animation->motion = KlonoaANM[anmID_throw]->getmotion();
	KlonoaAnimList[anm_throwStd].NextAnim = anm_std;
	KlonoaAnimList[anm_throwStd].AnimationSpeed = 0.7f;
	KlonoaAnimList[anm_throwStd].Property = 4;
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
	KlonoaANM[24] = LoadObjectAnim("throw"); //TO DO ADD THROW AIR
	KlonoaANM[25] = LoadObjectAnim("Brake");
	KlonoaANM[26] = LoadObjectAnim("Landing");
	KlonoaANM[27] = LoadObjectAnim("Board");
	KlonoaANM[28] = LoadObjectAnim("Crouch");
	KlonoaANM[29] = LoadObjectAnim("FallBoard");
	KlonoaANM[30] = LoadObjectAnim("trick1");
	KlonoaANM[31] = LoadObjectAnim("trick2");
	KlonoaANM[32] = LoadObjectAnim("trick3");
	KlonoaANM[33] = LoadObjectAnim("BoardLeft");
	KlonoaANM[34] = LoadObjectAnim("BoardRight");
	KlonoaANM[35] = LoadObjectAnim("BoardJump");
}

void InitKlonoaCharSelAnim()
{
	SonicCharSelAnim[0] = KlonoaAnimList[11].Animation;
	SonicCharSelAnim[1] = KlonoaAnimList[75].Animation;
	SonicCharSelAnim[2] = KlonoaAnimList[76].Animation;
	SonicCharSelAnim[3] = 0;
}

void LoadKlonoaEventAnims()
{
	KlonoaEvANM[0] = LoadEventAnim("Upgrade0");
}

void Init_KlonoaAnim()
{
	LoadKlonoa_AnimFiles();
	SetKlonoaAnims();
	LoadKlonoaEventAnims();

	WriteJump((void*)0x49AB47, SetAnimList);
	WriteData((short*)0x49ACD8, (short)0x9090);
	WriteData<2>((void*)0x4916A5, 0x90u); // disable metal's weird tilting thing
	WriteData((char*)0x49BE22, (char)0xEB);
	WriteJump(InitSonicCharSelAnims, InitKlonoaCharSelAnim);
}