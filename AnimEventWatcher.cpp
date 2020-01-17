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
	/*_MESSAGE("%s, To: %llx, FnNew : %llx, FnOld : %llx", (className + std::string(" hooked to the sink")).c_str(),
			 *(UInt64*)this + 0x8, &AnimEventWatcher::ReceiveEventHook, fn);*/
}

void AnimEventWatcher::RemoveHook() {
	FnReceiveEvent fn = fnHash.at(*(UInt64*)this);
	if (!fn) return;
	Utils::SafeWrite64Alt(*(UInt64*)this + 0x8, fn);
	fnHash.erase(*(UInt64*)this);
}

void AnimEventWatcher::ResetHook() {
	for (unordered_map<UInt64, FnReceiveEvent>::iterator it = fnHash.begin(); it != fnHash.end(); it++) {
		Utils::SafeWrite64Alt(it->first + 0x8, it->second);
	}
	fnHash.clear();
}


EventResult AnimEventWatcher::ReceiveEventHook(BSAnimationGraphEvent* evn, EventDispatcher<BSAnimationGraphEvent>* dispatcher) {
	//_MESSAGE("Event : %llx, dispatcher: %llx, event name : %s", evn, dispatcher, evn->eventname);
	Actor* a = *(Actor * *)((UInt64)evn + 0x8);
	if (strcmp(evn->eventname, "PowerAttack_Start_end") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::PrePre);
	}
	else if (strcmp(evn->eventname, "preHitFrame") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::Pre);
	}
	else if (strcmp(evn->eventname, "weaponSwing") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::Swing);
	}
	else if (strcmp(evn->eventname, "weaponLeftSwing") == 0) {
		WeaponSpeedManager::EvaluateEvent(a, iSwingState::SwingL);
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
	FnReceiveEvent fn = fnHash.at(*(UInt64*)this);
	return fn ? (this->*fn)(evn, dispatcher) : kEvent_Continue;
}