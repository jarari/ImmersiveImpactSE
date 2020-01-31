#include "AnimEventWatcher.h"
#include "AddressManager.h"
#include "CharacterMoveEventWatcher.h"
#include "ObjectLoadWatcher.h"
#include "PhysicsManager.h"
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

void HookWatchers(Actor* a) {
	AnimEventWatcher* ae = static_cast<AnimEventWatcher*>(&a->animGraphEventSink);
	ae->HookSink();
	if (PhysicsManager::physHooked) {
		CharacterMoveEventWatcher* cme = (CharacterMoveEventWatcher*)(&a->characterMoveFinishEvent);
		cme->HookSink();
	}
}

bool ActorHooked = false;
bool CharacterHooked = false;
bool PlayerCharacterHooked = false;
EventResult ObjectLoadWatcher::ReceiveEvent(TESObjectLoadedEvent* evn, EventDispatcher<TESObjectLoadedEvent>* src) {
	TESForm* form = LookupFormByID(evn->formId);
	if (form && evn->loaded) {
		Actor* _a = dynamic_cast<Actor*>(form);
		Character* _c = dynamic_cast<Character*>(form);
		PlayerCharacter* _p = dynamic_cast<PlayerCharacter*>(form);
		if (_c && !_p && !CharacterHooked) {
			_MESSAGE("Character Ptr : %llx, Name : %s", form, Utils::GetName(((Actor*)form)->baseForm));
			HookWatchers((Actor*)form);
			CharacterHooked = true;
		}
		else if (_p && !PlayerCharacterHooked) {
			_MESSAGE("PlayerCharacter Ptr : %llx, Name : %s", form, Utils::GetName(((Actor*)form)->baseForm));
			HookWatchers((Actor*)form);
			PlayerCharacterHooked = true;
		}
		/*else if (_a && !_c && !_p && !ActorHooked) {
			_MESSAGE("Actor Ptr : %llx, Name : %s", form, Utils::GetName(((Actor*)form)->baseForm));
			HookWatchers((Actor*)form);
			ActorHooked = true;
		}*/
		if (CharacterHooked && PlayerCharacterHooked)
			RemoveWatcher();
	}
	return kEvent_Continue;
}