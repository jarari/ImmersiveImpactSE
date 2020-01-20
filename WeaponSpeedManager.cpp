#include "ActorManager.h"
#include "ConfigManager.h"
#include "WeaponSpeedManager.h"
#include "Utils.h"
#include "INILibrary\SimpleIni.h"
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

void WeaponSpeedManager::CompatibilityPatch() {
	DataHandler* dh = DataHandler::GetSingleton();
	CSimpleIniA* ini = ConfigManager::GetINI();
	CSimpleIniA::TNamesDepend sections;
	ini->GetAllSections(sections);
	for (CSimpleIniA::TNamesDepend::iterator s_it = sections.begin(); s_it != sections.end(); s_it++) {
		const ModInfo* mod = dh->LookupModByName(s_it->pItem);
		if (mod) {
			CSimpleIniA::TNamesDepend keys;
			ini->GetAllKeys(s_it->pItem, keys);
			for (CSimpleIniA::TNamesDepend::iterator k_it = keys.begin(); k_it != keys.end(); k_it++) {
				if (std::stoi(ini->GetValue(s_it->pItem, k_it->pItem)) - 1 < 0) {
					_MESSAGE("Index start is 1! Check ini settings.");
					continue;
				}
				UInt32 LowerID = std::stoi(k_it->pItem, 0, 16);
				UInt32 formID = mod->GetFormID(LowerID);
				TESForm* form = LookupFormByID(formID);
				if (form->formType == kFormType_Spell) {
					auto effect = ((SpellItem*)form)->effectItemList[std::stoi(ini->GetValue(s_it->pItem, k_it->pItem)) - 1];
					effect->magnitude += 1.0f;
					_MESSAGE("Modified effect %s. New Magnitude : %f", Utils::GetName(effect->mgef), effect->magnitude);
				}
			}
		}
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
	float offset_r = 0;
	float offset_l = 0;
	//Calculate offset
	ActorManager::SetCurrentAV(a, "WeaponSpeedMult", 1);
	ActorManager::SetCurrentAV(a, "LeftWeaponSpeedMult", 1);
	offset_r = 1 - ActorManager::GetAV(a, "WeaponSpeedMult");
	offset_l = 1 - ActorManager::GetAV(a, "LeftWeaponSpeedMult");

	tList<ActiveEffect>* list_ae = a->magicTarget.GetActiveEffects();
	tList<ActiveEffect>::Iterator it = list_ae->Begin();
	while (!it.End()) {
		ActiveEffect* ae = it.Get();
		//Instead of looking for specific for ids, look for what actorvalue does this ActiveEffect change.
		if ((ae->flags & ActiveEffect::kFlag_Inactive) != ActiveEffect::kFlag_Inactive && 
			ae->actorValue == ActorManager::GetActorValuePointerFromMap("WeaponSpeedMult")) {
			float mag = ae->magnitude;
			if (mag >= 1)
				mag -= 1.0f;
			offset_r += mag;
			offset_l += mag;
		}
		++it;
	}
	int weptype_r = Utils::GetWeaponType(((TESObjectWEAP*)a->GetEquippedObject(false)));
	int weptype_l = Utils::GetWeaponType(((TESObjectWEAP*)a->GetEquippedObject(true)));

	float speed_r = ConfigManager::GetConfig()[weptype_r * 5 + evn + 1].value;
	float speed_l = ConfigManager::GetConfig()[weptype_l * 5 + evn + 1].value;

	if (weptype_r != iWepType::None)
		ActorManager::SetCurrentAV(a, "WeaponSpeedMult", speed_r + offset_r);
	if (weptype_l != iWepType::None)
		ActorManager::SetCurrentAV(a, "LeftWeaponSpeedMult", speed_l + offset_l);
}
