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
		cmd->taskran = false;
		cmd->counter = 0;
	}
	return cmd;
}

void StaggerTask::Run() {
	counter++;
	if (counter <= 2)
		return;
	bool isStaggering = false;
	bool dontMove = (target->actorState.flags04 & 0x1200000) == 0x1200000;
	bool isBleedingOut = false;
	((IAnimationGraphManagerHolderEx*)& target->animGraphHolder)->GetAnimationVariableBool("IsStaggering", isStaggering);
	((IAnimationGraphManagerHolderEx*)& target->animGraphHolder)->GetAnimationVariableBool("IsBleedingOut", isBleedingOut);
	if ((isStaggering && counter < 9) || dontMove || isBleedingOut || !target ||
		(attacker && ActorManager::IsInKillmove(attacker)) || //killmove
		ActorManager::IsInKillmove(target) || //killmove
		target->IsDead(1))
		return;
	SetAnimationVariableFloat(& target->animGraphHolder, "staggerDirection", dir);
	SetAnimationVariableFloat(& target->animGraphHolder, "staggerMagnitude", mag);
	((IAnimationGraphManagerHolderEx*)& target->animGraphHolder)->SendAnimationEvent("staggerStart");
	taskran = true;
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
		if (!(*it)->taskran) {
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
