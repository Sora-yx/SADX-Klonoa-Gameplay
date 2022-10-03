#pragma once


void hover_Physics(taskwk* data, motionwk2* data2, playerwk* co2);
signed int hover_CheckInput(taskwk* data, playerwk* co2, klonoawk* klwk);

signed int KlonoaSJump_CheckInput(taskwk* data, playerwk* co2, klonoawk* klwk);
signed int KlonoaWBullet_CheckInput(taskwk* data, playerwk* co2, klonoawk* klwk);
void Klonoa_ManageBullet(taskwk* data, playerwk* co2, klonoawk* klwk);
signed int HoldEnemy_CheckJump(taskwk* data, playerwk* co2);
void ResetKlonoaGrab(klonoawk* klwk);

void DropEnemy(klonoawk* klwk);
signed int ThrowEnemy_CheckInput(taskwk* data, playerwk* co2, klonoawk* klwk);
void CheckKlonoaEnemyPtr(klonoawk* klwk, taskwk* data, playerwk* co2);
void SuperKlonoa_Fly(playerwk* co2, taskwk* data, motionwk2* data2);
signed int Fly_CheckInput(taskwk* data, playerwk* co2);
void Fly_ManageMotion(taskwk* data, playerwk* co2);