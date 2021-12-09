#pragma once
#include <string>
#include <unordered_map>
#include <skse64\GameEvents.h>
using std::unordered_map;

class BSAnimationGraphEvent {
public:
	const BSFixedString eventname;
};

class AnimEventWatcher : public BSTEventSink<BSAnimationGraphEvent> {
public:
	typedef EventResult (AnimEventWatcher::* FnReceiveEvent)(BSAnimationGraphEvent* evn, EventDispatcher<BSAnimationGraphEvent>* dispatcher);
	AnimEventWatcher() {
		_MESSAGE((className + std::string(" instance created.")).c_str());
	}

	EventResult ReceiveEventHook(BSAnimationGraphEvent* evn, EventDispatcher<BSAnimationGraphEvent>* src);

	static void HookSink(uintptr_t ptr);

protected:
	static std::string className;
	static unordered_map<UInt64, FnReceiveEvent> fnHash;
};