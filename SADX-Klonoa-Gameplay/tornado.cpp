#include "pch.h"

static ModelInfo* TornadoPlaneMDL = nullptr;
static ModelInfo* TornadoTransfoPlaneMDL = nullptr;

DataPointer(NJS_OBJECT, Tornado1_Object, 0x28B1DA0);
DataPointer(NJS_OBJECT, Tornado2Before_Object, 0x27EB198);
DataPointer(NJS_OBJECT, Tornado2Transfo_Object, 0x2863E20);
DataPointer(NJS_OBJECT, Tornado2Change_Object, 0x280C158);

void Tornado_init()
{
	TornadoPlaneMDL = LoadBasicModel("Klonoa_Tornado");
	TornadoTransfoPlaneMDL = LoadBasicModel("tornado_transfo");

	if (TornadoPlaneMDL)
	{
		NJS_OBJECT* model = TornadoPlaneMDL->getmodel();

		if (model)
		{
			Tornado1_Object.child->sibling->sibling->sibling->child = model;
			Tornado2Before_Object.child->sibling->sibling->child = model;
			Tornado2Change_Object.child->sibling->sibling->sibling->sibling->child = model;
			Tornado2Transfo_Object.child->child->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling = TornadoTransfoPlaneMDL->getmodel();
		}
	}
}