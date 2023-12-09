#include "pch.h"
#include "IniFile.hpp"
#include "ModelInfo.h"
#include "FunctionHook.h"
#include "UsercallFunctionHandler.h"
#include <map>
#include <vector>

//This whole page adds 'Weights' support to the "Basic Model" type, the format SADX originally use for the models in the game.
//Originally, this format doesn't support 'Weights', which means you cannot import rigged characters with their animations since they couldn't render properly.
//This hacks here add weights so they can render fine, this is all possible by MainMemory who worked really hard to add support for that and the tools, big thanks to her!

using std::map;
using std::vector;
using std::string;

struct ModelWeightInfo
{
	WeightInfo* weights;
	int rightHandNode = -1;
	int leftHandNode = -1;
	int rightFootNode = -1;
	int leftFootNode = -1;
	int user0Node = -1;
	int user1Node = -1;
	int rightHandDir;
	int leftHandDir;
	int rightFootDir;
	int leftFootDir;
	int user0Dir;
	int user1Dir;
};

struct CharInfo
{
	ModelInfo** mdlList = nullptr;
	int lengthMdlList = 0;
	const intptr_t* pointersArray = nullptr;
	int pointersLength = 0;
	map<NJS_OBJECT*, ModelWeightInfo> modelWeights;
};

const intptr_t sonicWeldPointers[] = {
	0x49AB7E,
	0x49ABAC,
	0x49ACB6,
	0x7D27BA,
	0x7D30A0,
	0x7D58B1,
	0x7D5D21
};


map<int, CharInfo> charInfos = {
	{
		Characters_Sonic,
		{
			nullptr,
			2,
			arrayptrandlengthT(sonicWeldPointers, int),
		},
	},
};

const BasicWeightFuncs* weightFuncs;

NJS_MATRIX matrix;
void ProcessWeights(playerwk* pwp, NJS_OBJECT*& object, NJS_MOTION* motion, float frame)
{
	map<NJS_OBJECT*, ModelWeightInfo>* weightinfo = (map<NJS_OBJECT*, ModelWeightInfo>*)pwp->mj.pljvptr;

	switch (pwp->mj.jvmode)
	{
	case 0:
		for (auto& nodeweights : *weightinfo)
			weightFuncs->Init(nodeweights.second.weights, nodeweights.first);
		pwp->mj.jvmode = 1;
		break;
	case 1:
	{
		NJS_ACTION action = { object, motion };
		auto nodeweights = weightinfo->find(object);
		if (nodeweights != weightinfo->end())
		{
			weightFuncs->Apply(nodeweights->second.weights, &action, frame);
			{
				int* nodeidx = &nodeweights->second.rightHandNode;
				int* dir = &nodeweights->second.rightHandDir;
				auto co2 = (CharObj2*)pwp;
				for (int i = 0; i < 6; i++)
					if (*nodeidx != -1)
					{
						NJS_VECTOR pos{};
						NJS_VECTOR norm{};
						(&norm.x)[dir[i]] = 1;
						SetInstancedMatrix(nodeidx[i], matrix);
						njCalcPoint(matrix, &pos, &co2->SoManyVectors[i]);
						njCalcVector(matrix, &norm, &co2->SoManyVectors[i + 6]);
					}
			}
		}
	}
	break;
	default:
		for (auto& nodeweights : *weightinfo)
			weightFuncs->DeInit(nodeweights.second.weights, nodeweights.first);
		break;
	}
}

FunctionHook<void, taskwk*, motionwk2*, playerwk*> ProcessVertexWelds_h(PJoinVertexes);

void __cdecl ProcessVertexWelds_Check(taskwk* twp, motionwk2* mwp, playerwk* pwp)
{
	uint8_t id = twp->counter.b[1];

	if (id == 0 && MetalSonicFlag)
		id = Characters_MetalSonic;

	auto charinf = charInfos.find(id);

	if (charinf != charInfos.end() && charinf->second.modelWeights.size() > 0)
	{
		NJS_OBJECT* object;
		NJS_MOTION* motion;

		if (pwp->mj.mtnmode == 2)
		{
			object = pwp->mj.actwkptr->object;
			motion = pwp->mj.actwkptr->motion;
		}
		else
		{
			object = pwp->mj.plactptr[(unsigned __int16)pwp->mj.reqaction].actptr->object;
			motion = pwp->mj.plactptr[(unsigned __int16)pwp->mj.reqaction].actptr->motion;
		}

		ProcessWeights(pwp, object, motion, pwp->mj.nframe);
	}
	else
	{
		ProcessVertexWelds_h.Original(twp, mwp, pwp);
	}
}


void Init_BasicWeightedModels()
{

	weightFuncs = HelperFunctionsGlobal.Weights;

	if (KlonoaWeightedModels)
		charInfos.at(Characters_Sonic).mdlList = KlonoaWeightedModels;

	for (auto& charinf : charInfos)
	{
		for (uint16_t i = 0; i < charinf.second.lengthMdlList; i++)
		{
			if (charinf.second.mdlList == nullptr)
				continue;

			auto mdl = charinf.second.mdlList[i];

			if (!mdl)
				continue;

			auto obj = mdl->getmodel();

			if (!obj)
				continue;

			charinf.second.modelWeights.insert_or_assign(obj, ModelWeightInfo{ mdl->getweightinfo() });
		}

		for (int i = 0; i < charinf.second.pointersLength; i++)
			WriteData((decltype(CharInfo::modelWeights)**)charinf.second.pointersArray[i], &charinf.second.modelWeights);
	}

	if (charInfos[Characters_Sonic].modelWeights.size() > 0)
	{
		WriteData((char*)0x49BE77, (char)0xEB); // disable crystal ring swap
		WriteData((char*)0x493500, (char)0xC3); // disable stretchy shoes
		WriteData<2>((void*)0x49BE22, 0x90u); // enable welds for sonic's spin model
	}

	ProcessVertexWelds_h.Hook(ProcessVertexWelds_Check);
}