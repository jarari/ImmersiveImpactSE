#include "ActorManager.h"
#include "AddressManager.h"
#include "AnimEventWatcher.h"
#include "CharacterMoveEventWatcher.h"
#include "ModifiedSKSE.h"
#include "Utils.h"
#include <skse64\GameCamera.h>
#include <skse64\GameData.h>
#include <skse64\GameReferences.h>
#include <skse64\PapyrusVM.h>

TESSound* ActorManager::deflectSound;

bool isRidingHorse(Actor* a) {
	return (a->actorState.flags04 & (3 << 14));
}

void ActorManager::HookEvents() {
	AnimEventWatcher::HookSink(ptr_PCAnimEventReceiveEvent);
	CharacterMoveEventWatcher::HookSink(ptr_PCMoveEventReceiveEvent);
	AnimEventWatcher::HookSink(ptr_CAnimEventReceiveEvent);
	CharacterMoveEventWatcher::HookSink(ptr_CMoveEventReceiveEvent);
}

void ActorManager::FindDeflectSound() {
	DataHandler* dh = DataHandler::GetSingleton();
	const ModInfo* iimpt = dh->LookupModByName("ImmersiveImpact.esp");
	if (!iimpt)
		_MESSAGE("ImmersiveImpact.esp not found!");
	deflectSound = (TESSound*)LookupFormByID(iimpt->GetFormID(0x803));
	if (deflectSound)
		_MESSAGE("Found the sound at %llx", deflectSound);
}

void ActorManager::deflectAttack(Actor* actor, ActiveEffect* ae, bool isArrow, bool playSound, float volume) {
	//ae->magnitude = 0;
	//ae->duration = ae->elapsed;
	((IAnimationGraphManagerHolderEx*)(&actor->animGraphHolder))->SendAnimationEvent("staggerStop");
	if (actor != *g_thePlayer && !isArrow) {
		((IAnimationGraphManagerHolderEx*)(&actor->animGraphHolder))->SendAnimationEvent("attackStart");
	}
	if (playSound && deflectSound) {
		UInt32 sound = Utils::SoundPlay(deflectSound, actor);
		if (sound != -1) {
			Utils::SetInstanceVolume(sound, volume);
		}
	}
}

bool ActorManager::IsInKillmove(Actor* a) {
	return (a->flags2 & 0x00004000) == 0x00004000;
}

bool ActorManager::CanBeKnockdown(Actor* a) {
	return !((a->race->data.raceFlags & TESRace::kRace_NoKnockdowns) == TESRace::kRace_NoKnockdowns);
}

void ActorManager::RestrainPlayerMovement(bool restrain) {
	if (isRidingHorse(*g_thePlayer)) return;
	*(bool*)((UInt64)PlayerControls::GetSingleton()->movementHandler + 0x8) = !restrain;
}

void ActorManager::RestrainPlayerView(bool restrain) {
	PlayerCamera::GetSingleton()->unk162 = restrain;
}

std::map<char*, UInt32> avPtrs;
UInt32 ActorManager::GetActorValuePointerFromMap(char* AVname) {
	std::map<char*, UInt32>::iterator it = avPtrs.find(AVname);
	if (it != avPtrs.end()) {
		return it->second;
	}
	UInt32 avPtr = LookupActorValueByName(AVname);
	avPtrs[AVname] = avPtr;
	return avPtr;
}

void ActorManager::SetBaseAV(Actor* a, char* AVname, float v) {
	UInt32 AVPtr = GetActorValuePointerFromMap(AVname);
	a->actorValueOwner.SetBase(AVPtr, v);
}

void ActorManager::SetCurrentAV(Actor* a, char* AVname, float v) {
	UInt32 AVPtr = GetActorValuePointerFromMap(AVname);
	a->actorValueOwner.SetCurrent(AVPtr, v);
}

float ActorManager::GetAV(Actor* a, char* AVname) {
	UInt32 AVPtr = GetActorValuePointerFromMap(AVname);
	return a->actorValueOwner.GetCurrent(AVPtr);
}

float ActorManager::GetAVMax(Actor* a, char* AVname) {
	UInt32 AVPtr = GetActorValuePointerFromMap(AVname);
	return a->actorValueOwner.GetMaximum(AVPtr);
}
