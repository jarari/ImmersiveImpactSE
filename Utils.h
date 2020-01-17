#pragma once
#include <skse64\GameTypes.h>
class Actor;
class ActiveEffect;
class NiPoint3;
class NiMatrix33;
class TESObjectREFR;
class TESSound;

namespace Utils{
	void Dump(void* mem, unsigned int size);
	template<class Ty>
	Ty SafeWrite64Alt(uintptr_t addr, Ty data) {
		UInt32	oldProtect;
		size_t len = sizeof(data);

		VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
		Ty olddata = *(Ty*)addr;
		memcpy((void*)addr, &data, len);
		VirtualProtect((void*)addr, len, oldProtect, &oldProtect);
		return olddata;
	}
	ActiveEffect* GetActiveEffectFromActor(Actor* actor, const char* name);
	void SetMatrix33(float a, float b, float c, float d, float e, float f, float g, float h, float i, NiMatrix33& mat);
	void GetRefForward(float pitch, float yaw, float roll, NiPoint3* vec);
	void SendNotification(const char* str);
	UInt32 SoundPlay(TESSound* sound, TESObjectREFR* obj);
	void SetInstanceVolume(UInt32 instance, float volume);
}