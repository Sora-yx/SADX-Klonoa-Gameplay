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
};

struct CharInfo
{
	const intptr_t* pointersArray;
	int pointersLength;
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
			arrayptrandlengthT(sonicWeldPointers, int),
		}
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

void Init_BasicWeightedHack()
{
	weightFuncs = HelperFunctionsGlobal.Weights;

	auto mdl = KlonoaMDL.get();

	if (!mdl)
		return;

	auto obj = mdl->getmodel();

	if (!obj)
		return;

	charInfos.at(Characters_Sonic).modelWeights.insert_or_assign(obj, ModelWeightInfo{ mdl->getweightinfo() });


	auto superKlo = SuperKlonoaMDL.get();

	charInfos.at(Characters_Sonic).modelWeights.insert_or_assign(superKlo->getmodel(), ModelWeightInfo{superKlo->getweightinfo()});

	for (int i = 0; i < charInfos.at(Characters_Sonic).pointersLength; i++)
		WriteData((decltype(CharInfo::modelWeights)**)charInfos.at(Characters_Sonic).pointersArray[i], &charInfos.at(Characters_Sonic).modelWeights);


	if (charInfos[Characters_Sonic].modelWeights.size() > 0)
	{
		WriteData<2>((void*)0x49BE22, 0x90u); // enable welds for sonic's spin model
	}

	ProcessVertexWelds_h.Hook(ProcessVertexWelds_Check);
}
