#include "pch.h"

static ModelInfo* TornadoPlaneMDL = nullptr;
static ModelInfo* TornadoShotMDL = nullptr;
static ModelInfo* Tornado2TransfoPlaneMDL = nullptr;
static ModelInfo* Tornado1EvMdl = nullptr; //copy of regular model with just different tex ids because I'm lazy. Eventually it would be nice to have code that fix the tex id directly
static ModelInfo* Tornado2EvMdl = nullptr; //same
										
DataPointer(NJS_OBJECT, Tornado1_Object, 0x28B1DA0);
DataPointer(NJS_OBJECT, Tornado1_EventShot, 0x2920FC0);
DataPointer(NJS_OBJECT, Tornado2Before_Object, 0x27EB198);
DataPointer(NJS_OBJECT, Tornado2Transfo_Object, 0x2863E20);
DataPointer(NJS_OBJECT, Tornado2Change_Object, 0x280C158);
DataPointer(NJS_OBJECT, Tornado1_EventObj, 0x32611F8);
DataPointer(NJS_OBJECT, Tornado2_EventObj, 0x32E9D28);

void Tornado_init()
{
	TornadoPlaneMDL = LoadBasicModel("Klonoa_Tornado");
	TornadoShotMDL = LoadBasicModel("tornado1_shot");
	Tornado2TransfoPlaneMDL = LoadBasicModel("tornado2_transfo");
	Tornado1EvMdl = LoadBasicModel("Klonoa_Tornado1_EV");
	Tornado2EvMdl = LoadBasicModel("Klonoa_Tornado2_EV");

	if (TornadoPlaneMDL)
	{
		NJS_OBJECT* model = TornadoPlaneMDL->getmodel();

		if (model)
		{
			Tornado1_Object.child->sibling->sibling->sibling->child = model;
			Tornado1_EventShot.child->child->sibling->sibling->sibling->sibling->sibling->child = TornadoShotMDL->getmodel()->child;
			Tornado1_EventObj.child->sibling->sibling->sibling->child = Tornado1EvMdl->getmodel();

			Tornado2Before_Object.child->sibling->sibling->child = model;
			Tornado2Change_Object.child->sibling->sibling->sibling->sibling->child = model;
			Tornado2Transfo_Object.child->child->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling = Tornado2TransfoPlaneMDL->getmodel();
			Tornado2_EventObj.child->sibling->sibling->sibling->sibling->child = Tornado2EvMdl->getmodel();
		}
	}
}