#include "ActorManager.h"
#include "ConfigManager.h"
#include "WeaponSpeedManager.h"
#include "Utils.h"
#include <skse64\GameData.h>

static bool movementRestrained = false;
static bool viewRestrained = false;

void WeaponSpeedManager::ResetRestraintChecker() {
	if (movementRestrained) {
		movementRestrained = false;
		ActorManager::RestrainPlayerMovement(false);
	}
	if (viewRestrained) {
		viewRestrained = false;
		if((*g_thePlayer)->actorState.IsWeaponDrawn())
			ActorManager::RestrainPlayerView(false);
	}
}

void WeaponSpeedManager::EvaluateEvent(Actor* a, int evn) {
	if (a == *g_thePlayer && a->actorState.IsWeaponDrawn()) {
		if (evn == iSwingState::PrePre || evn == iSwingState::Pre) {
			if (ConfigManager::GetConfig()[iConfigType::RestrainMovement].value) {
				ActorManager::RestrainPlayerMovement(true);
				movementRestrained = true;
			}
			if (ConfigManager::GetConfig()[iConfigType::RestrainAim].value) {
				ActorManager::RestrainPlayerView(true);
				viewRestrained = true;
			}
		}
		else if (evn == iSwingState::Hit) {
			if (ConfigManager::GetConfig()[iConfigType::EnableDash].value && !ActorManager::IsInKillmove(a)) {
				NiPoint3 fwd;
				Utils::GetRefForward(a->rot.x, -a->rot.z, 0, &fwd);
				fwd = fwd * 18.0f;
				ActorManager::TranslateTo(a, a->pos.x + fwd.x, a->pos.y + fwd.y, a->pos.z + fwd.z, a->rot.x, a->rot.y, a->rot.z, 500, 0);
			}
		}
		else if (evn == iSwingState::End) {
			if (ConfigManager::GetConfig()[iConfigType::RestrainMovement].value && movementRestrained) {
				ActorManager::RestrainPlayerMovement(false);
				movementRestrained = false;
			}
			if (ConfigManager::GetConfig()[iConfigType::RestrainAim].value && viewRestrained) {
				ActorManager::RestrainPlayerView(false);
				viewRestrained = false;
			}
		}
	}
	if (!ConfigManager::GetConfig()[iConfigType::EnableWeaponSpeed].value)
		return;
	ActiveEffect* offset_r = Utils::GetActiveEffectFromActor(a, "BingleWeaponSpeedOffset_R");
	ActiveEffect* offset_l = Utils::GetActiveEffectFromActor(a, "BingleWeaponSpeedOffset_L");
	if (!offset_r || !offset_l) {
		_MESSAGE("Weapon speed effects not found in actor %s", Utils::GetName(a->baseForm));
		return;
	}
	//Calculate offset
	ActorManager::SetCurrentAV(a, "WeaponSpeedMult", 1);
	ActorManager::SetCurrentAV(a, "LeftWeaponSpeedMult", 1);
	offset_r->magnitude = 1 - ActorManager::GetAV(a, "WeaponSpeedMult");
	offset_l->magnitude = 1 - ActorManager::GetAV(a, "LeftWeaponSpeedMult");

	tList<ActiveEffect>* list_ae = a->magicTarget.GetActiveEffects();
	tList<ActiveEffect>::Iterator it = list_ae->Begin();
	while (!it.End()) {
		ActiveEffect* ae = it.Get();
		//Instead of looking for specific for ids, look for what actorvalue does this ActiveEffect change.
		if (ae->actorValue == ActorManager::GetActorValuePointerFromMap("WeaponSpeedMult")) {
			float mag = ae->magnitude;
			if (ae->item && ae->item->formID < 0x06000000)
				mag -= 1.0f;
			offset_r->magnitude += mag;
			offset_l->magnitude += mag;
		}
		++it;
	}
	int weptype_r = Utils::GetWeaponType(((TESObjectWEAP*)a->GetEquippedObject(false)));
	int weptype_l = Utils::GetWeaponType(((TESObjectWEAP*)a->GetEquippedObject(true)));

	float speed_r = ConfigManager::GetConfig()[weptype_r * 5 + evn + 1].value;
	float speed_l = ConfigManager::GetConfig()[weptype_l * 5 + evn + 1].value;

	if (weptype_r != iWepType::None)
		ActorManager::SetCurrentAV(a, "WeaponSpeedMult", speed_r + offset_r->magnitude);
	if (weptype_l != iWepType::None)
		ActorManager::SetCurrentAV(a, "LeftWeaponSpeedMult", speed_l + offset_l->magnitude);
}
