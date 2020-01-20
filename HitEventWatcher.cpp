#include "ActorManager.h"
#include "AddressManager.h"
#include "ConfigManager.h"
#include "HitEventTask.h"
#include "HitEventWatcher.h"
#include "HitStopManager.h"
#include "Utils.h"
#include <skse64_common\BranchTrampoline.h>
#include <skse64_common\SafeWrite.h>
#include <skse64\GameExtraData.h>
#include <skse64\GameMenus.h>
#include <skse64\GameReferences.h>
#include <skse64\GameRTTI.h>
#include <xbyak\xbyak.h>
#include <random>

std::string HitEventWatcher::className = "HitEventWatcher";
HitEventWatcher* HitEventWatcher::instance = nullptr;
std::random_device rd;

void HookDamageCalculation() {
	struct InstallHookDamageCalculation_Code : Xbyak::CodeGenerator {
		InstallHookDamageCalculation_Code(void* buf, uintptr_t processTask) : Xbyak::CodeGenerator(4096, buf) {
			Xbyak::Label retn;
			test(rdi, rdi);
			je(retn);
			cmp(rdi, rbx);
			je(retn);

			push(rax);					//rax, rcx, rdx, r8, r9, r10, r12, xmm0, xmm1, xmm2, ZF, CF are affected... Save all values
			push(rcx);
			push(rdx);
			push(r8);
			sahf();
			lea(rsp, ptr[rsp - 0x80]);
			mov(ptr[rsp + 0x40], r12);
			mov(ptr[rsp + 0x50], r10);
			mov(ptr[rsp + 0x60], r9);
			mov(ptr[rsp + 0x70], ah);
			movdqu(xmm6, xmm0);
			movdqu(xmm7, xmm1);
			mov(rcx, rdi);
			mov(rdx, rbx);
			mov(rax, processTask);
			call(rax);
			movdqu(xmm2, xmm8);
			movdqu(xmm1, xmm7);
			movdqu(xmm0, xmm6);
			movdqu(xmm6, xmm8);
			movdqu(xmm7, xmm8);
			mov(r12, ptr[rsp + 0x40]);
			mov(r10, ptr[rsp + 0x50]);
			mov(r9, ptr[rsp + 0x60]);
			mov(ah, ptr[rsp + 0x70]);
			lea(rsp, ptr[rsp + 0x80]);
			lahf();
			pop(r8);
			pop(rdx);
			pop(rcx);
			pop(rax);

			L(retn);
				mov(rax, ptr [rcx]);
				mov(edx, 0x00000018);
				jmp(ptr[rip]);
				dq(ptr_DamageInjectionPoint + 0x8);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	InstallHookDamageCalculation_Code code(codeBuf, GetFnAddr(HitEventPool::ProcessTask));
	g_localTrampoline.EndAlloc(code.getCurr());

	if (!g_branchTrampoline.Write5Branch(ptr_DamageInjectionPoint, uintptr_t(code.getCode())))
		return;
	SafeWrite8(ptr_DamageInjectionPoint + 5, 0x90);
	SafeWrite8(ptr_DamageInjectionPoint + 6, 0x90);
	SafeWrite8(ptr_DamageInjectionPoint + 7, 0x90);
}

void HitEventWatcher::InitWatcher() {
	if (!instance) {
		instance = new HitEventWatcher();
		HookDamageCalculation();
	}
	_MESSAGE((className + std::string(" initialized.")).c_str());
}

EventResult HitEventWatcher::ReceiveEvent(TESHitEvent* evn, EventDispatcher<TESHitEvent>* src) {
	HitStopManager::EvaluateEvent(evn);
	if (!evn->target || evn->target->formType != kFormType_Character || !evn->caster || evn->caster->formType != kFormType_Character)
		return kEvent_Continue;
	Character* target = (Character*)evn->target;
	Character* attacker = (Character*)evn->caster;
	//If GodMode is on
	if (target == *g_thePlayer && *(bool*)ptr_GodMode)
		return kEvent_Continue;
	//If the target is dying
	if (ActorManager::IsInKillmove(attacker) || ActorManager::IsInKillmove(target) || target->IsDead(1))
		return kEvent_Continue;

	ActiveEffect* ae = Utils::GetActiveEffectFromActor(target, "BingleHitFeedback");
	//If the damage is done by magic
	//If the target cannot be knock-downed.
	if (evn->projectileFormID != 0
		|| ((!ActorManager::CanBeKnockdown(target) || ActorManager::GetAV(target, "Mass") >= 2)
			&& target != *g_thePlayer && ConfigManager::GetConfig()[iConfigType::StaggerAny].value == 0))
		return kEvent_Continue;

	if (ae == nullptr)
		return kEvent_Continue;

	if (ae->elapsed - ae->duration > ConfigManager::GetConfig()[iConfigType::StaggerResetTime].value)
		ae->magnitude = 0;

	ae->duration = ae->elapsed;

	bool blocked = (evn->flags & TESHitEvent::kFlag_Blocked) == TESHitEvent::kFlag_Blocked;
	bool powerattack = (evn->flags & TESHitEvent::kFlag_PowerAttack) == TESHitEvent::kFlag_PowerAttack;
	bool bash = (evn->flags & TESHitEvent::kFlag_Bash) == TESHitEvent::kFlag_Bash;

	//Dump(evn, 256);
	TESObjectWEAP* wep = (TESObjectWEAP*)LookupFormByID(evn->sourceFormID);
	if (!wep || wep->formType != kFormType_Weapon)
		return kEvent_Continue;

	bool isArrow = false;
	if ((wep->type() == TESObjectWEAP::GameData::kType_Bow
		 || wep->type() == TESObjectWEAP::GameData::kType_Bow2
		 || wep->type() == TESObjectWEAP::GameData::kType_CrossBow
		 || wep->type() == TESObjectWEAP::GameData::kType_CBow)
		&& !bash) {
		isArrow = true;
		ae->duration += 1.0f;
	}

	if (blocked
		&& !powerattack
		&& !bash) {
		ActorManager::deflectAttack(target, ae, isArrow, false);
		return kEvent_Continue;
	}

	/*TESContainer* container = DYNAMIC_CAST(target->baseForm, TESForm, TESContainer);
	ExtraContainerChanges* pXContainerChanges = static_cast<ExtraContainerChanges*>(target->extraData.GetByType(kExtraData_ContainerChanges));
	InventoryEntryData* objList = (InventoryEntryData*)pXContainerChanges->data->objList;
	tList<EntryDataList>::Iterator it = ((tList<EntryDataList>*)objList)->Begin();
	float armorValue = 0;
	int armorPieces = 0;
	while (!it.End()) {
		InventoryEntryData* extraData = (InventoryEntryData*)it.Get();
		if (extraData && extraData->type->GetFormType() == FormType::kFormType_Armor) {
			TESForm* form = extraData->type;
			int baseCount = 0;
			if (container)
				baseCount = container->CountItem(form);
			if (extraData->countDelta > 0) {
				InventoryEntryData::EquipData state;
				extraData->GetEquipItemData(state, form->formID, baseCount);
				if (state.isTypeWorn || state.isTypeWornLeft || state.isItemWorn || state.isItemWornLeft) {
					armorValue += ((TESObjectARMO*)form)->armorValTimes100;
					armorPieces++;
				}
			}
		}
		++it;
	}
	armorValue /= 100.0f;*/
	float armorValue = max(ActorManager::GetAV(target, "DamageResist"), 0);
	float deflectChance = min(armorValue * ConfigManager::GetConfig()[iConfigType::DeflectChanceMul].value / 100.0f, ConfigManager::GetConfig()[iConfigType::DeflectChanceMax].value);
	std::mt19937 e{ rd() }; // or std::default_random_engine e{rd()};
	std::uniform_int_distribution<int> dist{ 0, 99 };
	float chance = dist(e);
	if (chance < deflectChance) {
		PlayerCameraEx* pcam = (PlayerCameraEx*)PlayerCamera::GetSingleton();
		NiPoint3 dpos = (NiPoint3(pcam->posX, pcam->posY, pcam->posZ) - target->pos);
		/*float dist = sqrt(dpos.x * dpos.x + dpos.y * dpos.y + dpos.z * dpos.z);
		float maxDist = 1500.0f;
		float vol = min(max(maxDist - dist, 0.0f) / maxDist, 1.0f);*/
		ActorManager::deflectAttack(target, ae, isArrow, true);
		evn->flags &= ~(UInt32)0x8;
		evn->flags |= TESHitEvent::kFlag_Blocked;
		if (attacker == *g_thePlayer) {
			char buff[64];
			/*TESFullName* pFullName = DYNAMIC_CAST(target->baseForm, TESForm, TESFullName);
			const char* name = "Unknown";
			if (pFullName)
				name = pFullName->name.data;*/
			snprintf(buff, sizeof buff, "$BINGLE_IIPT_DEFLECT");
			Utils::SendNotification(buff);
		}
		ae->magnitude = ConfigManager::GetConfig()[iConfigType::StaggerLimit].value;
		return kEvent_Continue;
	}
	else {
		//float damage = (damageBase + arrowDamage) * (1.0f - (armorValue * 0.12f + 0.03f * armorPieces));
		float armorCap = ConfigManager::GetConfig()[iConfigType::StaggerArmorCap].value;
		float staggerMul = min(powerattack + bash + 1.0f, 2.0f) / (powf(min(armorCap, armorValue) / armorCap, 1.5) * 3.0f + 1.0f);
		HitEventTask* cmd = HitEventTask::Create(target, attacker, ae, staggerMul, isArrow, wep->type());
		if (cmd) {
			HitEventPool::AddTask(cmd);
		}
	}
	return kEvent_Continue;
}