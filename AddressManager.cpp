#include "AddressManager.h"
#include "PatternScanner.h"

uintptr_t ptr_Notification;
uintptr_t ptr_PlayNative;
uintptr_t ptr_DamageInjectionPoint;
uintptr_t ptr_SetAnimationVariableFloat;
uintptr_t ptr_TranslateToNative;
uintptr_t ptr_SetInstanceVolumeToNative;
uintptr_t ptr_GodMode;
uintptr_t ptr_UnknownDataHolder;
uintptr_t ptr_ShakeController;
uintptr_t ptr_ShakeCameraNative;
uintptr_t ptr_ApplyImageSpaceModifier;
uintptr_t ptr_VelocityInjectionPoint;
uintptr_t ptr_FrictionOverridePoint;
uintptr_t ptr_OnGroundVelocityOverridePoint;

_SendNotification SendNotification_Fn;
_Play_Native Play_Native;
_SetAnimationVariableFloat SetAnimationVariableFloat;
_TranslateTo_Native TranslateTo_Native;
_SetInstanceVolume_Native SetInstanceVolume_Native;
_ShakeController ShakeController;
_ShakeCamera_Native ShakeCamera_Native;
_ApplyImageSpaceModifier ApplyImageSpaceModifier;

AddressManager* AddressManager::instance = nullptr;

void AddressManager::FindAddresses() {
	MemoryRegion* mr = new MemoryRegion();
	PatternScanner::GetSkyrimMemoryRegion(mr);
	_MESSAGE("base : %llx, size : %u", mr->base, mr->size);

	ptr_Notification = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x48, 0x89, 0x74, 0x24, 0x70, 0x4C, 0x8B, 0xFA, 0x48, 0x8B, 0xE9}) - 0x21;
	_MESSAGE("Function - Notification %llx", ptr_Notification);
	SendNotification_Fn = (_SendNotification)ptr_Notification;

	ptr_PlayNative = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x48, 0x83, 0xEC, 0x30, 0x49, 0x8B, 0xF1, 0x49, 0x8B, 0xD8, 0x8B, 0xEA}) - 0x10;
	_MESSAGE("Function - Play (Native) %llx", ptr_PlayNative);
	Play_Native = (_Play_Native)ptr_PlayNative;

	ptr_DamageInjectionPoint = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x48, 0x8B, 0x01, 0xBA, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x50, 0x08, 0x48, 0x8B, 0xD7});
	_MESSAGE("Damage Hook Injection Point %llx", ptr_DamageInjectionPoint);

	ptr_SetAnimationVariableFloat = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0xF3, 0x0F, 0x11, 0x54, 0x24, 0x18, 0x48, 0x83, 0xEC, 0x28, 0x4C, 0x8D});
	_MESSAGE("Function - SetAnimationVariableFloat %llx", ptr_SetAnimationVariableFloat);
	SetAnimationVariableFloat = (_SetAnimationVariableFloat)ptr_SetAnimationVariableFloat;

	ptr_TranslateToNative = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x4D, 0x8B, 0xF0, 0x44, 0x8B, 0xE2, 0x4C, 0x8B, 0xF9}) - 0x32;
	_MESSAGE("Function - TranslateTo (Native) %llx", ptr_TranslateToNative);
	TranslateTo_Native = (_TranslateTo_Native)ptr_TranslateToNative;

	ptr_SetInstanceVolumeToNative = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0xF3, 0x0F, 0x10, 0x4C, 0x24, 0x60, 0x48, 0x8D, 0x4C}) - 0x9;
	_MESSAGE("Function - SetInstanceVolume (Native) %llx", ptr_SetInstanceVolumeToNative);
	SetInstanceVolume_Native = (_SetInstanceVolume_Native)ptr_SetInstanceVolumeToNative;

	uintptr_t godFuncOffset = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x84, 0xC0, 0x0F, 0x94, 0xC1, 0xE8}, true) + 0x66;
	uintptr_t godValOffset = godFuncOffset + *(UInt32*)godFuncOffset + 0x6;
	ptr_GodMode = godValOffset + *(UInt32*)godValOffset + 0x4;
	_MESSAGE("God Mode Bool %llx", ptr_GodMode);

	uintptr_t dataholderOffset = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0xE8, 0x00, 0x00, 0x00, 0x00, 0x33, 0xC9, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x84, 0xC0}, true) + 0x13;
	ptr_UnknownDataHolder = *(uintptr_t*)(dataholderOffset + *(UInt32*)dataholderOffset + 0x4);
	_MESSAGE("Unk Data Holder (For Pause/Resume) %llx", ptr_UnknownDataHolder); //0x160 = pause counter

	ptr_ShakeController = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x29, 0x7C, 0x24, 0x50, 0x0F, 0x28, 0xF2}) - 0x15;
	_MESSAGE("Function - ShakeController %llx", ptr_ShakeController);
	ShakeController = (_ShakeController)ptr_ShakeController;

	ptr_ShakeCameraNative = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x48, 0x83, 0xEC, 0x38, 0xF3, 0x0F, 0x10, 0x44, 0x24, 0x60, 0x0F});
	_MESSAGE("Function - ShakeCamera (Native) %llx", ptr_ShakeCameraNative);
	ShakeCamera_Native = (_ShakeCamera_Native)ptr_ShakeCameraNative;

	ptr_ApplyImageSpaceModifier = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x48, 0x89, 0x68, 0x18, 0x0F, 0x29, 0x70, 0xD8, 0x49, 0x8B, 0xF0}) - 0x17;
	_MESSAGE("Function - Apply (ImageSpaceModifier) %llx", ptr_ApplyImageSpaceModifier);
	ApplyImageSpaceModifier = (_ApplyImageSpaceModifier)ptr_ApplyImageSpaceModifier;

	ptr_VelocityInjectionPoint = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x45, 0x0F, 0x57, 0xC0, 0xC7, 0x45, 0xA0}) + 0x4;
	_MESSAGE("Velocity Hook Injection Point %llx", ptr_VelocityInjectionPoint);

	uintptr_t calculateFrictionOffset = ptr_VelocityInjectionPoint + 0x116;
	ptr_FrictionOverridePoint = (calculateFrictionOffset + *(UInt32*)(calculateFrictionOffset + 0x1) + 0x232);
	_MESSAGE("Friction Override Point %llx", ptr_FrictionOverridePoint);

	ptr_OnGroundVelocityOverridePoint = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x0F, 0x28, 0x33, 0x0F, 0x28, 0xEE}) - 0x54;
	_MESSAGE("On Ground Velocity Override Point %llx", ptr_OnGroundVelocityOverridePoint);

	delete(mr);
}
