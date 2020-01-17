#include "AddressManager.h"
#include "PatternScanner.h"

uintptr_t ptr_Notification = -1;
uintptr_t ptr_PlayNative = -1;
uintptr_t ptr_DamageInjectionPoint = -1;
uintptr_t ptr_SetAnimationVariableFloat = -1;
uintptr_t ptr_TranslateToNative = -1;
uintptr_t ptr_SetInstanceVolumeToNative = -1;
uintptr_t ptr_GodMode = -1;

_SendNotification SendNotification_Fn;
_Play_Native Play_Native;
_SetAnimationVariableFloat SetAnimationVariableFloat;
_TranslateTo_Native TranslateTo_Native;
_SetInstanceVolume_Native SetInstanceVolume_Native;

AddressManager* AddressManager::instance = nullptr;

void AddressManager::FindAddresses() {
	MemoryRegion* mr = new MemoryRegion();
	PatternScanner::GetSkyrimMemoryRegion(mr);
	_MESSAGE("base : %llx, size : %u", mr->base, mr->size);

	ptr_Notification = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x40, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x83, 0xEC, 0x30, 0x48, 0xC7, 0x44, 0x24, 0x28, 0xFE});
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

	ptr_TranslateToNative = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x48, 0x81, 0xEC, 0xB0, 0x00, 0x00, 0x00, 0x48, 0xC7, 0x45, 0xDF, 0xFE, 0xFF, 0xFF, 0xFF, 0x48, 0x89, 0x58, 0x08, 0x48, 0x89, 0x70, 0x10}) - 0x10;
	_MESSAGE("Function - TranslateTo (Native) %llx", ptr_TranslateToNative);
	TranslateTo_Native = (_TranslateTo_Native)ptr_TranslateToNative;

	ptr_SetInstanceVolumeToNative = PatternScanner::PatternScanInternal(mr, vector<BYTE>{0x48, 0x83, 0xEC, 0x38, 0x45, 0x85, 0xC9, 0x74, 0x27, 0xF3, 0x0F, 0x10, 0x4C, 0x24, 0x60, 0x48, 0x8D, 0x4C, 0x24, 0x20, 0x44, 0x89, 0x4C, 0x24});
	_MESSAGE("Function - SetInstanceVolume (Native) %llx", ptr_SetInstanceVolumeToNative);
	SetInstanceVolume_Native = (_SetInstanceVolume_Native)ptr_SetInstanceVolumeToNative;

	ptr_GodMode = mr->base + 0x2F3AFB6; //I couldn't find a way to get this address

	delete(mr);
}
