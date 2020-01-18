#pragma once
#include "ModifiedSKSE.h"
#include <skse64\GameEvents.h>
#include <list>
#include <string>

class HitEventWatcher : public BSTEventSink<TESHitEvent> {
protected:
	static HitEventWatcher* instance;
	static std::string className;
	static std::list<UInt64> actors;
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

	virtual EventResult ReceiveEvent(TESHitEvent* evn, EventDispatcher<TESHitEvent>* src) override;

	static void InitWatcher();

};