#include "CharacterMoveEventWatcher.h"
#include "Utils.h"
#include "PhysicsManager.h"
#include "WeaponSpeedManager.h"
#include <skse64\GameReferences.h>
#include <inttypes.h>
#include <chrono>

std::string CharacterMoveEventWatcher::className = "CharacterMoveEventWatcher";
unordered_map<UInt64, CharacterMoveEventWatcher::FnReceiveEvent> CharacterMoveEventWatcher::fnHash;

void CharacterMoveEventWatcher::HookSink() {
	FnReceiveEvent fn = Utils::SafeWrite64Alt(*(UInt64*)this + 0x8, &CharacterMoveEventWatcher::ReceiveEventHook);
	fnHash.insert(std::pair<UInt64, FnReceiveEvent>(*(UInt64*)this, fn));
	_MESSAGE("%s, To: %llx, FnNew : %llx, FnOld : %llx", (className + std::string(" hooked to the sink")).c_str(),
			 *(UInt64*)this + 0x8, &CharacterMoveEventWatcher::ReceiveEventHook, fn);
}

EventResult CharacterMoveEventWatcher::ReceiveEventHook(bhkCharacterMoveFinishEvent* evn, EventDispatcher<bhkCharacterMoveFinishEvent>* dispatcher) {
	Character* c = (Character*)((UInt64)this - 0xD0);
	if (c && c->loadedState && c->loadedState->node && !c->IsDead(1)) {
		if (!PhysicsManager::Simulate(c)) {
			PhysicsManager::InitializeData(c);
		}
	}

	FnReceiveEvent fn = fnHash.at(*(UInt64*)this);
	return fn ? (this->*fn)(evn, dispatcher) : kEvent_Continue;
}