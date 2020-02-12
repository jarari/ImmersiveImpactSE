#include "AddressManager.h"
#include "ConfigManager.h"
#include "HitStopManager.h"
#include "ModifiedSKSE.h"
#include "Utils.h"
#include "WeaponSpeedManager.h"
#include <skse64_common\SafeWrite.h>
#include <skse64\GameData.h>
#include <skse64\GameReferences.h>
#include <skse64\PapyrusVM.h>
#include <thread>
using std::thread;
TESImageSpaceModifier* HitStopManager::blurModifier;
std::chrono::system_clock::time_point HitStopManager::lastEffect = std::chrono::system_clock::now();
ICriticalSection HitStopThreadManager::threadQueue_Lock;
ICriticalSection HitStopThreadManager::gamePause_Lock;
bool HitStopThreadManager::running;
queue<thread*> HitStopThreadManager::threadQueue;
thread* HitStopThreadManager::lastThread;

void HitStopThreadFunc(int duration, int sync) {
	HitStopThreadManager::gamePause_Lock.Enter();
	std::this_thread::sleep_for(std::chrono::milliseconds(sync)); //Hit frame sync

	UIManager* ui = UIManager::GetSingleton();
	PlayerCameraEx* pCam = (PlayerCameraEx*)PlayerCamera::GetSingleton();
	(*(UInt32*)(ptr_UnknownDataHolder + 0x160))++;
	//CALL_MEMBER_FN(ui, AddMessage)(&BSFixedString("BingleHitStopHelper"), UIMessage::kMessage_Open, nullptr);
	int dur = max(duration, 15);
	int slept = 0;
	int sleepPerCall = dur / 15;
	float fovStep = ConfigManager::GetConfig()[iConfigType::HitStop_FovStep].value;
	float fovDiff = 0;
	while (slept < dur) {
		if (slept < dur / 2) {
			pCam->worldFOV -= fovStep;
			pCam->firstPersonFOV -= fovStep;
			fovDiff -= fovStep;
		}
		else {
			pCam->worldFOV += fovStep;
			pCam->firstPersonFOV += fovStep;
			fovDiff += fovStep;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepPerCall));
		slept += sleepPerCall;
	}
	pCam->worldFOV -= fovDiff;
	pCam->firstPersonFOV -= fovDiff;
	(*(UInt32*)(ptr_UnknownDataHolder + 0x160))--;
	//CALL_MEMBER_FN(ui, AddMessage)(&BSFixedString("BingleHitStopHelper"), UIMessage::kMessage_Close, nullptr);
	HitStopThreadManager::running = false;
	HitStopThreadManager::gamePause_Lock.Leave();
	HitStopThreadManager::RequestLaunch();
}

void HitStopManager::FindBlurEffect() {
	DataHandler* dh = DataHandler::GetSingleton();
	const ModInfo* iimpt = dh->LookupModByName("ImmersiveImpact.esp");
	if (!iimpt)
		_MESSAGE("ImmersiveImpact.esp not found!");
	blurModifier = (TESImageSpaceModifier*)LookupFormByID(iimpt->GetFormID(0x0000A42E));
	if (blurModifier)
		_MESSAGE("Found the blur modifier at %llx", blurModifier);
}

void HitStopManager::UnleashCameraShakeLimit() {
	vector<BYTE> patch1 = { 0x0F, 0x28, 0xD8, 0x90, 0x90, 0x90, 0x90, 0x90 };
	for (int i = 0; i < patch1.size(); i++)
		SafeWrite8(ptr_ShakeCameraNative + 0x12 + i, patch1[i]);

	SInt32 offset = *(SInt32*)(ptr_ShakeCameraNative + 0x89);
	uintptr_t patch2Func = ptr_ShakeCameraNative + offset + 0x15A; // +0x88 + 0x5 + 0xCD
	vector<BYTE> patch2 = { 0x0F, 0x28, 0xD3, 0x90, 0x90, 0x90, 0x90, 0x90 };
	for (int i = 0; i < patch2.size(); i++)
		SafeWrite8(patch2Func + i, patch2[i]);
}

void HitStopManager::EvaluateEvent(TESHitEvent* evn) {
	std::chrono::system_clock::time_point last = lastEffect;
	lastEffect = std::chrono::system_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last).count() < 15) {
		return;
	}
	PlayerCharacter* pc = *g_thePlayer;

	TESObjectWEAP* wep = (TESObjectWEAP*)LookupFormByID(evn->sourceFormID);
	if (!wep || wep->formType != kFormType_Weapon)
		return;

	bool powerattack = (evn->flags & TESHitEvent::kFlag_PowerAttack) == TESHitEvent::kFlag_PowerAttack;
	bool bash = (evn->flags & TESHitEvent::kFlag_Bash) == TESHitEvent::kFlag_Bash;
	if ((wep->type() == TESObjectWEAP::GameData::kType_Bow
		 || wep->type() == TESObjectWEAP::GameData::kType_Bow2
		 || wep->type() == TESObjectWEAP::GameData::kType_CrossBow
		 || wep->type() == TESObjectWEAP::GameData::kType_CBow)
		&& !bash) {
		return;
	}

	int weptype = Utils::GetWeaponType(wep);

	if (ConfigManager::GetConfig()[iConfigType::EnableHitStop].value) {
		if ((evn->target && evn->target->formType == kFormType_Character && evn->caster == pc && evn->target != pc) ||
			(evn->target && evn->target == pc && ConfigManager::GetConfig()[iConfigType::HitStop_OnPlayerHit].value) ||
			((evn->target && evn->target->formType != kFormType_Character || !evn->target) && evn->caster == pc && ConfigManager::GetConfig()[iConfigType::HitStop_OnObjectHit].value)) {
			thread* t;
			void (*threadFunc)(int, int) = HitStopThreadFunc;
			switch (weptype) {
				case iWepType::Fist:
					t = new thread(threadFunc, floor(ConfigManager::GetConfig()[iConfigType::HitStop_Fist].value * 1000),
								   floor(ConfigManager::GetConfig()[iConfigType::HitStop_SyncFist].value * 1000));
					break;
				case iWepType::Dagger:
					t = new thread(threadFunc, floor(ConfigManager::GetConfig()[iConfigType::HitStop_Dagger].value * 1000),
								   floor(ConfigManager::GetConfig()[iConfigType::HitStop_SyncDagger].value * 1000));
					break;
				case iWepType::OneH:
					t = new thread(threadFunc, floor(ConfigManager::GetConfig()[iConfigType::HitStop_1H].value * 1000),
								   floor(ConfigManager::GetConfig()[iConfigType::HitStop_Sync1H].value * 1000));
					break;
				case iWepType::TwoH:
					t = new thread(threadFunc, floor(ConfigManager::GetConfig()[iConfigType::HitStop_2H].value * 1000),
								   floor(ConfigManager::GetConfig()[iConfigType::HitStop_Sync2H].value * 1000));
					break;
			}
			HitStopThreadManager::threadQueue.push(t);
			HitStopThreadManager::RequestLaunch();
		}
	}

	if (ConfigManager::GetConfig()[iConfigType::EnableHitShakeController].value) {
		if ((evn->target && evn->target->formType == kFormType_Character && evn->caster == pc && evn->target != pc) ||
			(evn->target && evn->target == pc && ConfigManager::GetConfig()[iConfigType::HitShakeController_OnPlayerHit].value) ||
			((evn->target && evn->target->formType != kFormType_Character || !evn->target) && evn->caster == pc && ConfigManager::GetConfig()[iConfigType::HitShakeController_OnObjectHit].value)) {
			float mag;
			float dur;
			float strengthMul = powerattack ? ConfigManager::GetConfig()[iConfigType::HitShakeController_PowerAttackMult].value : 1.0f;
			switch (weptype) {
				case iWepType::Fist:
					mag = ConfigManager::GetConfig()[iConfigType::HitShakeController_Fist].value * strengthMul;
					dur = ConfigManager::GetConfig()[iConfigType::HitShakeController_Fist_Duration].value * strengthMul;
					break;
				case iWepType::Dagger:
					mag = ConfigManager::GetConfig()[iConfigType::HitShakeController_Dagger].value * strengthMul;
					dur = ConfigManager::GetConfig()[iConfigType::HitShakeController_Dagger_Duration].value * strengthMul;
					break;
				case iWepType::OneH:
					mag = ConfigManager::GetConfig()[iConfigType::HitShakeController_1H].value * strengthMul;
					dur = ConfigManager::GetConfig()[iConfigType::HitShakeController_1H_Duration].value * strengthMul;
					break;
				case iWepType::TwoH:
					mag = ConfigManager::GetConfig()[iConfigType::HitShakeController_2H].value * strengthMul;
					dur = ConfigManager::GetConfig()[iConfigType::HitShakeController_2H_Duration].value * strengthMul;
					break;
			}
			ShakeController(false, mag, mag, dur);
		}
	}

	if (ConfigManager::GetConfig()[iConfigType::EnableHitShakeCam].value) {
		if ((evn->target && evn->target->formType == kFormType_Character && evn->caster == pc && evn->target != pc) ||
			(evn->target && evn->target == pc && ConfigManager::GetConfig()[iConfigType::HitShakeCam_OnPlayerHit].value) ||
			((evn->target && evn->target->formType != kFormType_Character || !evn->target) && evn->caster == pc && ConfigManager::GetConfig()[iConfigType::HitShakeCam_OnObjectHit].value)) {
			VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();
			if (registry) {
				float mag;
				float dur;
				float strengthMul = powerattack ? ConfigManager::GetConfig()[iConfigType::HitShakeCam_PowerAttackMult].value : 1.0f;
				switch (weptype) {
					case iWepType::Fist:
						mag = ConfigManager::GetConfig()[iConfigType::HitShakeCam_Fist].value * strengthMul;
						dur = ConfigManager::GetConfig()[iConfigType::HitShakeCam_Fist_Duration].value * strengthMul;
						break;
					case iWepType::Dagger:
						mag = ConfigManager::GetConfig()[iConfigType::HitShakeCam_Dagger].value * strengthMul;
						dur = ConfigManager::GetConfig()[iConfigType::HitShakeCam_Dagger_Duration].value * strengthMul;
						break;
					case iWepType::OneH:
						mag = ConfigManager::GetConfig()[iConfigType::HitShakeCam_1H].value * strengthMul;
						dur = ConfigManager::GetConfig()[iConfigType::HitShakeCam_1H_Duration].value * strengthMul;
						break;
					case iWepType::TwoH:
						mag = ConfigManager::GetConfig()[iConfigType::HitShakeCam_2H].value * strengthMul;
						dur = ConfigManager::GetConfig()[iConfigType::HitShakeCam_2H_Duration].value * strengthMul;
						break;
				}
				ShakeCamera_Native(registry, 0, 0, pc, mag, dur);
			}
		}
	}

	if (ConfigManager::GetConfig()[iConfigType::EnableHitBlur].value) {
		if ((evn->target && evn->target->formType == kFormType_Character && evn->caster == pc && evn->target != pc) ||
			(evn->target && evn->target == pc && ConfigManager::GetConfig()[iConfigType::HitBlur_OnPlayerHit].value) ||
			((evn->target && evn->target->formType != kFormType_Character || !evn->target) && evn->caster == pc && ConfigManager::GetConfig()[iConfigType::HitBlur_OnObjectHit].value)) {
			float mag;
			float dur;
			float strengthMul = powerattack ? ConfigManager::GetConfig()[iConfigType::HitBlur_PowerAttackMult].value : 1.0f;
			switch (weptype) {
				case iWepType::Fist:
					mag = ConfigManager::GetConfig()[iConfigType::HitBlur_Fist].value * strengthMul;
					dur = ConfigManager::GetConfig()[iConfigType::HitBlur_Fist_Duration].value * strengthMul;
					break;
				case iWepType::Dagger:
					mag = ConfigManager::GetConfig()[iConfigType::HitBlur_Dagger].value * strengthMul;
					dur = ConfigManager::GetConfig()[iConfigType::HitBlur_Dagger_Duration].value * strengthMul;
					break;
				case iWepType::OneH:
					mag = ConfigManager::GetConfig()[iConfigType::HitBlur_1H].value * strengthMul;
					dur = ConfigManager::GetConfig()[iConfigType::HitBlur_1H_Duration].value * strengthMul;
					break;
				case iWepType::TwoH:
					mag = ConfigManager::GetConfig()[iConfigType::HitBlur_2H].value * strengthMul;
					dur = ConfigManager::GetConfig()[iConfigType::HitBlur_2H_Duration].value * strengthMul;
					break;
			}
			*(float*)((UInt64)blurModifier + 0x24) = dur;
			ApplyImageSpaceModifier(blurModifier, mag, NULL);
		}
	}
}

void HitStopThreadManager::RequestLaunch() {
	if (running) return;
	threadQueue_Lock.Enter();
	if (!threadQueue.empty()) {
		thread* t = threadQueue.front();
		threadQueue.pop();
		t->detach();
		HitStopThreadManager::running = true;
	}
	threadQueue_Lock.Leave();
}