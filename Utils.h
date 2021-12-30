#pragma once
#include <skse64\GameTypes.h>
class Actor;
class ActiveEffect;
class NiPoint3;
class NiMatrix33;
class TESForm;
class TESObjectREFR;
class TESObjectWEAP;
class TESSound;

struct Quaternion {
public:
	float x, y, z, w;
	Quaternion(float _x, float _y, float _z, float _w);
	float Norm();
	NiMatrix33 ToRotationMatrix33();
};

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
	ActiveEffect* GetActiveEffectFromActor(Actor* actor, EffectSetting* mgef);
	void SetMatrix33(float a, float b, float c, float d, float e, float f, float g, float h, float i, NiMatrix33& mat);
	NiMatrix33 GetRotationMatrix33(float pitch, float yaw, float roll);
	NiMatrix33 GetRotationMatrix33(NiPoint3 axis, float angle);
	float Determinant(NiMatrix33 mat);
	NiMatrix33 Inverse(NiMatrix33 mat);
	NiPoint3 WorldToLocal(NiPoint3 wpos, NiPoint3 lorigin, NiMatrix33 rot);
	NiPoint3 LocalToWorld(NiPoint3 lpos, NiPoint3 lorigin, NiMatrix33 rot);
	void GetRefForward(float pitch, float yaw, float roll, NiPoint3* vec);
	void SendNotification(const char* str);
	UInt32 SoundPlay(TESSound* sound, TESObjectREFR* obj);
	void SetInstanceVolume(UInt32 instance, float volume);
	int GetWeaponType(TESObjectWEAP* wep);
	const char* GetName(TESForm* form);
}