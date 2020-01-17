#include "ActiveEffectWatcher.h"
#include "AnimEventWatcher.h"
#include "Utils.h"
#include <skse64\GameReferences.h>
#include <skse64\GameRTTI.h>

std::string ActiveEffectWatcher::className = "ActiveEffectWatcher";
ActiveEffectWatcher* ActiveEffectWatcher::instance = nullptr;

void ActiveEffectWatcher::InitWatcher() {
	if (instance)
		delete(instance);
	instance = new ActiveEffectWatcher();
	_MESSAGE((className + std::string(" initialized.")).c_str());
}

void ActiveEffectWatcher::DestroyWatcher() {
	GetEventDispatcherList()->unkB0.RemoveEventSink(instance);
	delete(instance);
}

EventResult ActiveEffectWatcher::ReceiveEvent(TESActiveEffectApplyRemoveEvent* evn, EventDispatcher<TESActiveEffectApplyRemoveEvent>* src) {
	return kEvent_Continue;
}