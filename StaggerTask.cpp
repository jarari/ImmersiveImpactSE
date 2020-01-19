#include "ActorManager.h"
#include "AddressManager.h"
#include "ModifiedSKSE.h"
#include "StaggerTask.h"
#include <common\IMemPool.h>
#include <skse64\GameReferences.h>
#include "Utils.h"
IThreadSafeBasicMemPool<StaggerTask, 256>	s_StaggerTaskDelegatePool;

StaggerTask* StaggerTask::Create(Actor* attacker, Actor* target, float dir, float mag) {
	StaggerTask* cmd = s_StaggerTaskDelegatePool.Allocate();
	if (cmd) {
		cmd->attacker = attacker;
		cmd->target = target;
		cmd->dir = dir;
		cmd->mag = mag;
		cmd->counter = 0;
	}
	return cmd;
}

int framedelay = 6;
void StaggerTask::Run() {
	counter++;
	if (counter < framedelay || counter > framedelay)
		return;
	bool dontMove = false;//*(UInt32*)((UInt32)target->processManager + 0x5C) & 0x4 == 0x4;
	bool isBleedingOut = false;
	((IAnimationGraphManagerHolderEx*)& target->animGraphHolder)->GetAnimationVariableBool("IsBleedingOut", isBleedingOut);
	if (dontMove || isBleedingOut || !target ||
		(attacker && ActorManager::IsInKillmove(attacker)) || //killmove
		ActorManager::IsInKillmove(target) || //killmove
		target->IsDead(1))
		return;
	SetAnimationVariableFloat(& target->animGraphHolder, "staggerDirection", dir);
	SetAnimationVariableFloat(& target->animGraphHolder, "staggerMagnitude", mag);
	((IAnimationGraphManagerHolderEx*)& target->animGraphHolder)->SendAnimationEvent("staggerStart");
	_MESSAGE("%s -> %s stagger", Utils::GetName(attacker->baseForm), Utils::GetName(target->baseForm));
}

void StaggerTask::Dispose() {
	s_StaggerTaskDelegatePool.Free(this);
}



list<StaggerTask*> StaggerPool::tasks = list<StaggerTask*>();

void StaggerPool::AddTask(StaggerTask* t) {
	tasks.push_back(t);
}

void StaggerPool::ProcessTasks() {
	if (tasks.size() == 0) return;
	for (list<StaggerTask*>::iterator it = tasks.begin(); it != tasks.end(); it++) {
		if ((*it)->counter < framedelay) {
			(*it)->Run();
		}
		else {
			(*it)->Dispose();
			it = tasks.erase(it);
		}
	}
}

void StaggerPool::ResetPool() {
	tasks.clear();
	s_StaggerTaskDelegatePool.Reset();
}

StaggerHelper* StaggerHelper::instance;
void StaggerHelper::Render() {
	StaggerPool::ProcessTasks();
}
