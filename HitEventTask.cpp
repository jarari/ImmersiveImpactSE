#include "ActorManager.h"
#include "AddressManager.h"
#include "ConfigManager.h"
#include "HitEventTask.h"
#include "ModifiedSKSE.h"
#include "StaggerTask.h"
#include <common\IMemPool.h>
#include <skse64\GameReferences.h>
#define _USE_MATH_DEFINES
#include <math.h>
IThreadSafeBasicMemPool<HitEventTask, 256>	s_HitEventTaskDelegatePool;

HitEventTask* HitEventTask::Create(Actor* target, Actor* attacker, ActiveEffect* ae, float staggerMul, bool isArrow, int wepType) {
	HitEventTask* cmd = s_HitEventTaskDelegatePool.Allocate();
	if (cmd) {
		cmd->target = target;
		cmd->attacker = attacker;
		cmd->ae = ae;
		cmd->staggerMul = staggerMul;
		cmd->isArrow = isArrow;
		cmd->wepType = wepType;
	}
	return cmd;
}

void HitEventTask::UpdateDamage(float dmg) {
	damage = dmg;
}

void HitEventTask::Run() {
	if (!attacker ||
		ActorManager::IsInKillmove(attacker) || //killmove
		attacker->IsDead(1) ||
		!target ||
		ActorManager::IsInKillmove(target) || //killmove
		target->IsDead(1) ||
		damage > -0.5f)
		return;
	if (ae->magnitude >= ConfigManager::GetConfig()[iConfigType::StaggerLimit].value) {
		ActorManager::deflectAttack(target, ae, isArrow, false);
		return;
	}
	else {
		/*CALL_MEMBER_FN(&(target->animGraphHolder), SetAnimationVariableFloat)(BSFixedString("staggerDirection"), 0.5f - (target->rot.z - attacker->rot.z) / M_PI);
		CALL_MEMBER_FN(&(target->animGraphHolder), SetAnimationVariableFloat)(BSFixedString("staggerMagnitude"), staggerMagnitude);*/
		float bowDivider = isArrow ? 4.0f : 1.0f;
		float staggerMagnitude = min(max(-damage, 5.0f) / ConfigManager::GetConfig()[iConfigType::StaggerDamageMax].value * staggerMul, 1) / bowDivider / (ae->magnitude + 1);
		float staggerDir = 0.5f - (target->rot.z - attacker->rot.z) / M_PI;
		StaggerTask* cmd = StaggerTask::Create(target, staggerDir, staggerMagnitude);
		if (cmd) {
			((IAnimationGraphManagerHolderEx*)& target->animGraphHolder)->SendAnimationEvent("staggerStop");
			StaggerPool::AddTask(cmd);
		}
	}
	float mulIfDagger = wepType == TESObjectWEAP::GameData::kType_OneHandDagger || wepType == TESObjectWEAP::GameData::kType_1HD ? 0.75f : 1.0f;
	ae->magnitude += 1 * mulIfDagger;
}

void HitEventTask::Dispose() {
	s_HitEventTaskDelegatePool.Free(this);
}



list<HitEventTask*> HitEventPool::tasks = list<HitEventTask*>();

void HitEventPool::AddTask(HitEventTask* t) {
	tasks.push_back(t);
}

void HitEventPool::ProcessTask(Actor* attacker, Actor* target, float damage) {
	if (tasks.size() == 0 || !attacker || !target) return;
	for (list<HitEventTask*>::iterator it = tasks.begin(); it != tasks.end(); it++) {
		if ((*it)->attacker == attacker && (*it)->target == target) {
			(*it)->UpdateDamage(damage);
			(*it)->Run();
			(*it)->Dispose();
			it = tasks.erase(it);
		}
		else if (!(*it)->attacker || !(*it)->target) {
			(*it)->Dispose();
			it = tasks.erase(it);
		}
	}
}

void HitEventPool::ResetPool() {
	tasks.clear();
	s_HitEventTaskDelegatePool.Reset();
}
