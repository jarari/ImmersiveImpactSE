#pragma once
#include <map>
#include <skse64_common\Relocation.h>
class ActiveEffect;
class VMClassRegistry;
class TESObjectREFR;
class TESSound;
class Actor;

namespace ActorManager {
	static TESSound* deflectSound;
	void FindDeflectSound();
	void deflectAttack(Actor* actor, ActiveEffect* ae, bool isArrow = false, bool playSound = true, float volume = 1.0f);
	bool IsInKillmove(Actor* a);
	bool CanBeKnockdown(Actor* a);
	void TranslateTo(Actor* a, float x, float y, float z, float pitch, float yaw, float roll, float vel, float angvel);
	void RestrainPlayerMovement(bool restrain);
	void RestrainPlayerView(bool restrain);
	static std::map<char*, UInt32> avPtrs;
	UInt32 GetActorValuePointerFromMap(char* AVname);
	void SetBaseAV(Actor* a, char* AVname, float v);
	void SetCurrentAV(Actor* a, char* AVname, float v);
	float GetAV(Actor* a, char* AVname);
	float GetAVMax(Actor* a, char* AVname);
}