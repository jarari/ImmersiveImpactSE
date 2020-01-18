#include "ActorManager.h"
#include "AddressManager.h"
#include "ModifiedSKSE.h"
#include "StaggerTask.h"
#include <common\IMemPool.h>
#include <skse64\GameReferences.h>
IThreadSafeBasicMemPool<StaggerTask, 256>	s_StaggerTaskDelegatePool;

StaggerTask* StaggerTask::Create(Actor* target, float dir, float mag) {
	StaggerTask* cmd = s_StaggerTaskDelegatePool.Allocate();
	if (cmd) {
		cmd->target = target;
		cmd->dir = dir;
		cmd->mag = mag;
		cmd->counter = 0;
	}
	return cmd;
}

int framedelay = 6;
void StaggerTask::Run() {
	if (!target ||
		ActorManager::IsInKillmove(target) || //killmove
		target->IsDead(1)) {
		counter = 999;
		return;
	}
	counter++;
	if (counter < framedelay)
		return;
	bool dontMove = false;//*(UInt32*)((UInt32)target->processManager + 0x5C) & 0x4 == 0x4;
	bool isBleedingOut = false;
	((IAnimationGraphManagerHolderEx*)& target->animGraphHolder)->GetAnimationVariableBool("IsBleedingOut", isBleedingOut);
	if (dontMove || isBleedingOut)
		return;
	SetAnimationVariableFloat(& target->animGraphHolder, "staggerDirection", dir);
	SetAnimationVariableFloat(& target->animGraphHolder, "staggerMagnitude", mag);
	((IAnimationGraphManagerHolderEx*)& target->animGraphHolder)->SendAnimationEvent("staggerStart");
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
		if ((*it)->counter >= framedelay) {
			(*it)->Dispose();
			it = tasks.erase(it);
		}
		else {
			(*it)->Run();
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
