#pragma once
#include "ModifiedSKSE.h"
#include <skse64\GameReferences.h>
class TESObjectREFR;
class TESSound;
class VMClassRegistry;
extern uintptr_t ptr_Notification;
extern uintptr_t ptr_PlayNative;
extern uintptr_t ptr_DamageInjectionPoint;
extern uintptr_t ptr_SetAnimationVariableFloat;
extern uintptr_t ptr_TranslateToNative;
extern uintptr_t ptr_SetInstanceVolumeToNative;
extern uintptr_t ptr_GodMode;

typedef void (*_SendNotification)(const char* str);
extern _SendNotification SendNotification_Fn;

typedef UInt32 (*_Play_Native)(VMClassRegistry* registry, UInt32 stackId, TESSound* sound, TESObjectREFR* target);
extern _Play_Native Play_Native;

typedef bool (*_SetAnimationVariableFloat)(IAnimationGraphManagerHolder* holder, const BSFixedString& name, float value);
extern _SetAnimationVariableFloat SetAnimationVariableFloat;

typedef void (*_TranslateTo_Native)(VMClassRegistry* registry, UInt32 stackId, TESObjectREFR* target, float x, float y, float z, float pitch, float yaw, float roll, float vel, float angvel);
extern _TranslateTo_Native TranslateTo_Native;

typedef void (*_SetInstanceVolume_Native)(VMClassRegistry* registry, UInt32 stackId, UInt32 instance, float volume);
extern _SetInstanceVolume_Native SetInstanceVolume_Native;

class AddressManager {
	static AddressManager* instance;
public:
	AddressManager() {
		if (instance)
			delete(instance);
		instance = this;
		_MESSAGE("Address manager instance created.");
	}

	static AddressManager* GetInstance() {
		return instance;
	}

	void FindAddresses();
};