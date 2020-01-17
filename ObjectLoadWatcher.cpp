#include "AnimEventWatcher.h"
#include "AddressManager.h"
#include "ObjectLoadWatcher.h"
#include "Utils.h"
#include <skse64\GameReferences.h>
#include <skse64\GameRTTI.h>

std::string ObjectLoadWatcher::className = "ObjectLoadWatcher";
ObjectLoadWatcher* ObjectLoadWatcher::instance = nullptr;

void ObjectLoadWatcher::InitWatcher() {
	if (!instance)
		instance = new ObjectLoadWatcher();
	_MESSAGE((className + std::string(" initialized.")).c_str());
}

EventResult ObjectLoadWatcher::ReceiveEvent(TESObjectLoadedEvent* evn, EventDispatcher<TESObjectLoadedEvent>* src) {
	TESForm* form = LookupFormByID(evn->formId);
	if (form && form->GetFormType() == kFormType_Character && form != (*g_thePlayer)) {
		if (evn->loaded == 1) {
			if (form->GetFormType() == kFormType_Character) {
				TESFullName* pname = DYNAMIC_CAST(((Actor*)form)->baseForm, TESForm, TESFullName);
				const char* name;
				if (pname)
					name = pname->name.data;
				_MESSAGE("Ptr : %llx, Name : %s", form, name);
				AnimEventWatcher* ae = static_cast<AnimEventWatcher*>(&((Actor*)form)->animGraphEventSink);
				ae->HookSink();
			}
		}
		else if (((Actor*)form)->loadedState) {
			AnimEventWatcher* ae = static_cast<AnimEventWatcher*>(&((Actor*)form)->animGraphEventSink);
			ae->RemoveHook();
			_MESSAGE("Hook removed from %llx", form);
		}
	}
	return kEvent_Continue;
}