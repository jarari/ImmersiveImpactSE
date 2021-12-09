#include "CharacterMoveEventWatcher.h"
#include "Utils.h"
#include "PhysicsManager.h"
#include "WeaponSpeedManager.h"
#include <skse64\GameReferences.h>
#include <inttypes.h>
#include <chrono>

std::string CharacterMoveEventWatcher::className = "CharacterMoveEventWatcher";
unordered_map<UInt64, CharacterMoveEventWatcher::FnReceiveEvent> CharacterMoveEventWatcher::fnHash;

void CharacterMoveEventWatcher::HookSink(uintptr_t ptr) {
	FnReceiveEvent fn = Utils::SafeWrite64Alt(ptr + 0x8, &CharacterMoveEventWatcher::ReceiveEventHook);
	fnHash.insert(std::pair<UInt64, FnReceiveEvent>(ptr, fn));
	_MESSAGE("%s, To: %llx, FnNew : %llx, FnOld : %llx", (className + std::string(" hooked to the sink")).c_str(),
			 ptr + 0x8, &CharacterMoveEventWatcher::ReceiveEventHook, fn);
}

EventResult CharacterMoveEventWatcher::ReceiveEventHook(bhkCharacterMoveFinishEvent* evn, EventDispatcher<bhkCharacterMoveFinishEvent>* dispatcher) {
	Actor* a = (Actor*)((UInt64)this - 0xD0);
	if (a) {
		PhysicsManager::Simulate(a);
	}

	FnReceiveEvent fn = fnHash.at(*(UInt64*)this);
	return fn ? (this->*fn)(evn, dispatcher) : kEvent_Continue;
}