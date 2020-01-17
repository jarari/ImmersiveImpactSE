#pragma once
#include "ModifiedSKSE.h"
#include <skse64\GameReferences.h>
class TESImageSpaceModifier;
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
extern uintptr_t ptr_UnknownDataHolder;
extern uintptr_t ptr_ShakeController;
extern uintptr_t ptr_ShakeCameraNative;
extern uintptr_t ptr_ApplyImageSpaceModifier;

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

typedef void (*_ShakeController)(bool unk, float leftmag, float rightmag, float duration);
extern _ShakeController ShakeController;

typedef void (*_ShakeCamera_Native)(VMClassRegistry* registry, UInt32 stackId, void* unk, TESObjectREFR* source, float strength, float duration);
extern _ShakeCamera_Native ShakeCamera_Native;

typedef void (*_ApplyImageSpaceModifier)(TESImageSpaceModifier* modifier, float strength, void* null);
extern _ApplyImageSpaceModifier ApplyImageSpaceModifier;

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