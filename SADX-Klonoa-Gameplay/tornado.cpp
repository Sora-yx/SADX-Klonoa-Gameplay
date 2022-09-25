#include "pch.h"

static ModelInfo* TornadoPlaneMDL = nullptr;
static ModelInfo* TornadoTransfoPlaneMDL = nullptr;

DataPointer(NJS_OBJECT, Tornado1_Object, 0x28B1DA0);
DataPointer(NJS_OBJECT, Tornado2Before_Object, 0x27EB198);
DataPointer(NJS_OBJECT, Tornado2Transfo_Object, 0x2863E20);
DataPointer(NJS_OBJECT, Tornado2Change_Object, 0x280C158);

DataPointer(NJS_OBJECT, Tornado2Change_Saturn, 0x100D743C);
DataPointer(NJS_OBJECT, Tornado2Before_Saturn, 0x100E047C);
DataPointer(NJS_OBJECT, Tornado1_Saturn, 0x100E99A4);

DataPointer(NJS_OBJECT, Tornado2Change_Object_SA1, 0x100B0A30);
DataPointer(NJS_OBJECT, Tornado2Before_SA1, 0x100BCF94);
DataPointer(NJS_OBJECT, Tornado1_SA1, 0x100C9A14);

static bool CheckIfKlonoaPlane(NJS_MODEL_SADX* model)
{
	return TornadoPlaneMDL->getlabel(model) != "" || TornadoTransfoPlaneMDL->getlabel(model) != "";
}

static void __cdecl TornadoCallBack(NJS_MODEL_SADX* model, int blend, int idk)
{
	NonStaticFunctionPointer(void, sub_407FC0, (NJS_MODEL_SADX*, int), 0x407FC0);

	// If the currently drawn model is Klonoa, we use the character's texlist instead
	if (CheckIfKlonoaPlane(model))
	{
		auto tex_orig = CurrentTexList;

		njSetTexture(&KlonoaTexList);
		sub_407FC0(model, blend);
		njSetTexture(tex_orig);
	}
	else
	{
		sub_407FC0(model, blend);
	}
}

static void __cdecl njAction_Queue_Tornado(NJS_ACTION* action, float frame, QueuedModelFlagsB flags)
{
	DisplayAnimationFrame(action, frame, flags, 0.0, TornadoCallBack);
}

void Tornado_init()
{
	TornadoPlaneMDL = LoadBasicModel("Klonoa_Tornado");
	TornadoTransfoPlaneMDL = LoadBasicModel("tornado_transfo");

	if (TornadoPlaneMDL)
	{
		NJS_OBJECT* model = TornadoPlaneMDL->getmodel();

		if (model)
		{
			// We only replace Sonic's part of the plane
			HMODULE sa1Char = GetModuleHandle(L"SA1_Chars");

			if (sa1Char)
			{

			}

			Tornado1_Object.child->sibling->sibling->sibling->child = model;
			Tornado2Before_Object.child->sibling->sibling->child = model;
			Tornado2Change_Object.child->sibling->sibling->sibling->sibling->child = model;
			Tornado2Transfo_Object.child->child->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling = TornadoTransfoPlaneMDL->getmodel();

			// Hack to use our own texture for Klonoa's part of the plane, allowing compatibility with dc mods.
			WriteCall((void*)0x62753A, njAction_Queue_Tornado);
			WriteCall((void*)0x62AC30, njAction_Queue_Tornado); //event plane transfo
		}
	}
}