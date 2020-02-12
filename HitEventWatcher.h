#pragma once
#include "ModifiedSKSE.h"
#include <skse64\GameEvents.h>
#include <chrono>
#include <list>
#include <string>
#include <unordered_map>
using std::unordered_map;

class HitEventWatcher : public BSTEventSink<TESHitEvent> {
protected:
	static HitEventWatcher* instance;
	static std::string className;
	static std::list<UInt64> actors;
	static unordered_map<TESObjectREFR*, unordered_map<TESObjectREFR*, std::chrono::system_clock::time_point>> lastHitEvent;
public:
	HitEventWatcher() {
		if (instance)
			delete(instance);
		instance = this;
		_MESSAGE((className + std::string(" instance created.")).c_str());
		((EventDispatcherListEx*)GetEventDispatcherList())->hitEventDispatcher.AddEventSink(this);
	}

	virtual ~HitEventWatcher() {
		_MESSAGE((className + std::string(" instance destroyed.")).c_str());
	}

	static HitEventWatcher* GetInstance() {
		return instance;
	}

	static void ResetEventTimer() {
		lastHitEvent.clear();
	}

	virtual EventResult ReceiveEvent(TESHitEvent* evn, EventDispatcher<TESHitEvent>* src) override;

	static void InitWatcher();

};