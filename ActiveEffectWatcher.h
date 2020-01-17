#pragma once
#include <skse64\GameEvents.h>
#include <string>

class ActiveEffectWatcher : public BSTEventSink<TESActiveEffectApplyRemoveEvent> {
protected:
	static ActiveEffectWatcher* instance;
	static std::string className;

public:
	ActiveEffectWatcher() {
		if (instance)
			delete(instance);
		instance = this;
		_MESSAGE((className + std::string(" instance created.")).c_str());
		GetEventDispatcherList()->unkB0.AddEventSink(instance);
	}

	~ActiveEffectWatcher() {
		_MESSAGE((className + std::string(" instance destroyed.")).c_str());
	}

	static ActiveEffectWatcher* GetInstance() {
		return instance;
	}

	virtual EventResult ReceiveEvent(TESActiveEffectApplyRemoveEvent* evn, EventDispatcher<TESActiveEffectApplyRemoveEvent>* src) override;

	static void InitWatcher();

	static void DestroyWatcher();

};