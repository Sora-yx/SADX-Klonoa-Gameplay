#pragma once

FunctionPointer(void, PlayIdleVoice, (EntityData1* a1), 0x442360);
FunctionPointer(void, DrawCharacterShadow, (EntityData1* a1, struct_a3* a2), 0x49F0B0);
FunctionPointer(void, PGetAccelerationGlik, (taskwk* a1, motionwk2* a2, playerwk* a3), 0x448000);
FunctionPointer(void, PGetSpeedGlik, (taskwk* a1, motionwk2* a2, playerwk* a3), 0x0444580);
FunctionPointer(void, PResetPosition, (taskwk* a1, motionwk2* a2, playerwk* a3), 0x43EE70);
FunctionPointer(int, PResetAccelerationAir, (taskwk* a1, motionwk2* a2, playerwk* a3), 0x44BD70);



DataArray(NJS_ACTION*, SonicCharSelAnim, 0x3C5FF94, 4);
FunctionPointer(void, DrawEventAction, (taskwk* data), 0x4187D0);