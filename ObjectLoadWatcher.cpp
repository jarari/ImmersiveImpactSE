#include "AnimEventWatcher.h"
#include "AddressManager.h"
#include "ObjectLoadWatcher.h"
#include "Utils.h"
#include <skse64\GameReferences.h>
#include <skse64\GameRTTI.h>
#include <typeinfo.h>

std::string ObjectLoadWatcher::className = "ObjectLoadWatcher";
ObjectLoadWatcher* ObjectLoadWatcher::instance = nullptr;

void ObjectLoadWatcher::InitWatcher() {
	if (!instance)
		instance = new ObjectLoadWatcher();
	_MESSAGE((className + std::string(" initialized.")).c_str());
}

bool ActorHooked = false;
bool CharacterHooked = false;
EventResult ObjectLoadWatcher::ReceiveEvent(TESObjectLoadedEvent* evn, EventDispatcher<TESObjectLoadedEvent>* src) {
	TESForm* form = LookupFormByID(evn->formId);
	if (form && evn->loaded) {
		Actor* _a = dynamic_cast<Actor*>(form);
		Character* _c = dynamic_cast<Character*>(form);
		if (_c && !CharacterHooked) {
			TESFullName* pname = DYNAMIC_CAST(((Actor*)form)->baseForm, TESForm, TESFullName);
			const char* name;
			if (pname)
				name = pname->name.data;
			_MESSAGE("Character Ptr : %llx, Name : %s", form, name);
			AnimEventWatcher* ae = static_cast<AnimEventWatcher*>(&((Actor*)form)->animGraphEventSink);
			ae->HookSink();
			CharacterHooked = true;
		}
		else if (_a && !_c && !ActorHooked) {
			TESFullName* pname = DYNAMIC_CAST(((Actor*)form)->baseForm, TESForm, TESFullName);
			const char* name;
			if (pname)
				name = pname->name.data;
			_MESSAGE("Actor Ptr : %llx, Name : %s", form, name);
			AnimEventWatcher* ae = static_cast<AnimEventWatcher*>(&((Actor*)form)->animGraphEventSink);
			ae->HookSink();
			ActorHooked = true;
		}
		if (ActorHooked && CharacterHooked)
			RemoveWatcher();
	}
	return kEvent_Continue;
}