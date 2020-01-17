#pragma once
#include <skse64\GameEvents.h>
#include <list>
#include <string>

class ObjectLoadWatcher : public BSTEventSink<TESObjectLoadedEvent> {
protected:
	static ObjectLoadWatcher* instance;
	static std::string className;
	static std::list<UInt64> actors;

public:
	ObjectLoadWatcher() {
		if (instance)
			delete(instance);
		instance = this;
		_MESSAGE((className + std::string(" instance created.")).c_str());
		GetEventDispatcherList()->objectLoadedDispatcher.AddEventSink(instance);
	}

	~ObjectLoadWatcher() {
		_MESSAGE((className + std::string(" instance destroyed.")).c_str());
	}

	static ObjectLoadWatcher* GetInstance() {
		return instance;
	}

	virtual EventResult ReceiveEvent(TESObjectLoadedEvent* evn, EventDispatcher<TESObjectLoadedEvent>* src) override;

	static void InitWatcher();

};