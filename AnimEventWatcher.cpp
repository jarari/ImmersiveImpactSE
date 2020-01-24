#include "AnimEventWatcher.h"
#include "Utils.h"
#include "WeaponSpeedManager.h"
#include <skse64\GameReferences.h>
#include <inttypes.h>

std::string AnimEventWatcher::className = "AnimEventWatcher";
unordered_map<UInt64, AnimEventWatcher::FnReceiveEvent> AnimEventWatcher::fnHash;

void AnimEventWatcher::HookSink() {
	FnReceiveEvent fn = Utils::SafeWrite64Alt(*(UInt64*)this + 0x8, &AnimEventWatcher::ReceiveEventHook);
	fnHash.insert(std::pair<UInt64, FnReceiveEvent>(*(UInt64*)this, fn));
	_MESSAGE("%s, To: %llx, FnNew : %llx, FnOld : %llx", (className + std::string(" hooked to the sink")).c_str(),
			 *(UInt64*)this + 0x8, &AnimEventWatcher::ReceiveEventHook, fn);
}


EventResult AnimEventWatcher::ReceiveEventHook(BSAnimationGraphEvent* evn, EventDispatcher<BSAnimationGraphEvent>* dispatcher) {
	Actor* a = *(Actor * *)((UInt64)evn + 0x8);
	/*if(a == *g_thePlayer)
		_MESSAGE("Event : %llx, dispatcher: %llx, event name : %s, animGraphEventSink %llx", evn, dispatcher, evn->eventname, this);*/
	if (strlen(evn->eventname) > 20){
		std::string compare = "PowerAttack_Start_end";
		compare[18] = evn->eventname[18];
		if (strcmp(evn->eventname, compare.c_str()) == 0) {
			WeaponSpeedManager::EvaluateEvent(a, iSwingState::PrePre);
		}
	}
	else if (strcmp(evn->eventname, "preHitFrame") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::Pre);
	}
	else if (strcmp(evn->eventname, "weaponSwing") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::Swing);
	}
	else if (strcmp(evn->eventname, "weaponLeftSwing") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::Swing);
	}
	else if (strcmp(evn->eventname, "HitFrame") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::Hit);
	}
	else if (strcmp(evn->eventname, "AttackWinStart") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::End);
	}
	else if (strcmp(evn->eventname, "AttackWinStartLeft") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::End);
	}
	else if (strcmp(evn->eventname, "attackStop") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::End);
	}
	else if (strcmp(evn->eventname, "bashStop") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::End);
	}
	FnReceiveEvent fn = fnHash.at(*(UInt64*)this);
	return fn ? (this->*fn)(evn, dispatcher) : kEvent_Continue;
}