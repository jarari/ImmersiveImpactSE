#include "AddressManager.h"
#include "ConfigManager.h"
#include "HitStopManager.h"
#include "HitStopThread.h"
#include "ModifiedSKSE.h"
#include "Utils.h"
#include "WeaponSpeedManager.h"
#include <skse64\GameData.h>
#include <skse64\GameReferences.h>
#include <skse64\PapyrusVM.h>
#include <thread>
using std::thread;

void HitStopManager::FindBlurEffect() {
	DataHandler* dh = DataHandler::GetSingleton();
	const ModInfo* iimpt = dh->LookupModByName("ImmersiveImpact.esp");
	if (!iimpt)
		_MESSAGE("ImmersiveImpact.esp not found!");
	blurModifier = (TESImageSpaceModifier*)LookupFormByID(iimpt->GetFormID(0x0000A42E));
	if (blurModifier)
		_MESSAGE("Found the blur modifier at %llx", blurModifier);
}

void HitStopManager::EvaluateEvent(TESHitEvent* evn) {
	PlayerCharacter* pc = *g_thePlayer;

	TESObjectWEAP* wep = (TESObjectWEAP*)LookupFormByID(evn->sourceFormID);
	if (!wep || wep->formType != kFormType_Weapon)
		return;

	bool powerattack = evn->flags == TESHitEvent::kFlag_PowerAttack;
	bool bash = evn->flags == TESHitEvent::kFlag_Bash;
	float strengthMul = min(powerattack + bash + 1.0f, 1.0f);
	if ((wep->type() == TESObjectWEAP::GameData::kType_Bow
		 || wep->type() == TESObjectWEAP::GameData::kType_Bow2
		 || wep->type() == TESObjectWEAP::GameData::kType_CrossBow
		 || wep->type() == TESObjectWEAP::GameData::kType_CBow)
		&& !bash) {
		return;
	}

	int weptype = Utils::GetWeaponType(wep);

	if (ConfigManager::GetConfig()[iConfigType::EnableHitStop].value) {
		if ((evn->target && evn->caster == pc && evn->target != pc) ||
			(evn->target && evn->target == pc && ConfigManager::GetConfig()[iConfigType::HitStop_OnPlayerHit].value) ||
			(!evn->target && evn->caster == pc && ConfigManager::GetConfig()[iConfigType::HitStop_OnObjectHit].value)) {
			thread* t;
			switch (weptype) {
				case iWepType::Fist:
					t = new thread([](thread* t)-> void {
						(*(UInt32*)(ptr_UnknownDataHolder + 0x160))++;
						std::this_thread::sleep_for(std::chrono::milliseconds((int)floor(ConfigManager::GetConfig()[iConfigType::HitStop_Fist].value * 1000)));
						(*(UInt32*)(ptr_UnknownDataHolder + 0x160))--;
					}, t);
					break;
				case iWepType::Dagger:
					t = new thread([](thread* t)-> void {
						(*(UInt32*)(ptr_UnknownDataHolder + 0x160))++;
						std::this_thread::sleep_for(std::chrono::milliseconds((int)floor(ConfigManager::GetConfig()[iConfigType::HitBlur_Dagger].value * 1000)));
						(*(UInt32*)(ptr_UnknownDataHolder + 0x160))--;
					}, t);
					break;
				case iWepType::OneH:
					t = new thread([](thread* t)-> void {
						(*(UInt32*)(ptr_UnknownDataHolder + 0x160))++;
						std::this_thread::sleep_for(std::chrono::milliseconds((int)floor(ConfigManager::GetConfig()[iConfigType::HitStop_1H].value * 1000)));
						(*(UInt32*)(ptr_UnknownDataHolder + 0x160))--;
					}, t);
					break;
				case iWepType::TwoH:
					t = new thread([](thread* t)-> void {
						(*(UInt32*)(ptr_UnknownDataHolder + 0x160))++;
						std::this_thread::sleep_for(std::chrono::milliseconds((int)floor(ConfigManager::GetConfig()[iConfigType::HitStop_2H].value * 1000)));
						(*(UInt32*)(ptr_UnknownDataHolder + 0x160))--;
					}, t);
					break;
			}
			t->detach();
		}
	}

	if (ConfigManager::GetConfig()[iConfigType::EnableHitShakeController].value) {
		if ((evn->target && evn->caster == pc && evn->target != pc) ||
			(evn->target && evn->target == pc && ConfigManager::GetConfig()[iConfigType::HitShakeController_OnPlayerHit].value) ||
			(!evn->target && evn->caster == pc && ConfigManager::GetConfig()[iConfigType::HitShakeController_OnObjectHit].value)) {
			float mag;
			switch (weptype) {
				case iWepType::Fist:
					mag = ConfigManager::GetConfig()[iConfigType::HitShakeController_Fist].value * strengthMul;
					break;
				case iWepType::Dagger:
					mag = ConfigManager::GetConfig()[iConfigType::HitShakeController_Dagger].value * strengthMul;
					break;
				case iWepType::OneH:
					mag = ConfigManager::GetConfig()[iConfigType::HitShakeController_1H].value * strengthMul;
					break;
				case iWepType::TwoH:
					mag = ConfigManager::GetConfig()[iConfigType::HitShakeController_2H].value * strengthMul;
					break;
			}
			ShakeController(false, mag, mag, 0.25f * strengthMul);
		}
	}

	if (ConfigManager::GetConfig()[iConfigType::EnableHitShakeCam].value) {
		if ((evn->target && evn->caster == pc && evn->target != pc)  ||
			(evn->target && evn->target == pc && ConfigManager::GetConfig()[iConfigType::HitShakeCam_OnPlayerHit].value) ||
			(!evn->target && evn->caster == pc && ConfigManager::GetConfig()[iConfigType::HitShakeCam_OnObjectHit].value)) {
			VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();
			if (registry) {
				float mag;
				switch (weptype) {
					case iWepType::Fist:
						mag = ConfigManager::GetConfig()[iConfigType::HitShakeCam_Fist].value * strengthMul;
						break;
					case iWepType::Dagger:
						mag = ConfigManager::GetConfig()[iConfigType::HitShakeCam_Dagger].value * strengthMul;
						break;
					case iWepType::OneH:
						mag = ConfigManager::GetConfig()[iConfigType::HitShakeCam_1H].value * strengthMul;
						break;
					case iWepType::TwoH:
						mag = ConfigManager::GetConfig()[iConfigType::HitShakeCam_2H].value * strengthMul;
						break;
				}
				ShakeCamera_Native(registry, 0, 0, pc, mag, 0.25f * strengthMul);
			}
		}
	}

	if (ConfigManager::GetConfig()[iConfigType::EnableHitBlur].value) {
		if ((evn->target && evn->caster == pc && evn->target != pc) ||
			(evn->target && evn->target == pc && ConfigManager::GetConfig()[iConfigType::HitBlur_OnPlayerHit].value) ||
			(!evn->target && evn->caster == pc && ConfigManager::GetConfig()[iConfigType::HitBlur_OnObjectHit].value)) {
			float mag;
			switch (weptype) {
				case iWepType::Fist:
					mag = ConfigManager::GetConfig()[iConfigType::HitBlur_Fist].value * strengthMul;
					break;
				case iWepType::Dagger:
					mag = ConfigManager::GetConfig()[iConfigType::HitBlur_Dagger].value * strengthMul;
					break;
				case iWepType::OneH:
					mag = ConfigManager::GetConfig()[iConfigType::HitBlur_1H].value * strengthMul;
					break;
				case iWepType::TwoH:
					mag = ConfigManager::GetConfig()[iConfigType::HitBlur_2H].value * strengthMul;
					break;
			}
			ApplyImageSpaceModifier(blurModifier, mag, NULL);
		}
	}
}
