#include "AddressManager.h"
#include "PatternScanner.h"

uintptr_t ptr_Notification;
uintptr_t ptr_PlayNative;
uintptr_t ptr_DamageInjectionPoint;
uintptr_t ptr_SetAnimationVariableFloat;
uintptr_t ptr_SetInstanceVolumeToNative;
uintptr_t ptr_GodMode;
uintptr_t ptr_UnknownDataHolder;
uintptr_t ptr_ShakeController;
uintptr_t ptr_ShakeCameraNative;
uintptr_t ptr_ApplyImageSpaceModifier;
uintptr_t ptr_VelocityInjectionPoint;
uintptr_t ptr_FrictionOverridePoint;
uintptr_t ptr_OnGroundVelocityOverridePoint;
uintptr_t ptr_AccelerationOverridePoint;
uintptr_t ptr_EngineTick;
uintptr_t ptr_GetMass; 
uintptr_t ptr_AddVelocityOverridePoint;
uintptr_t ptr_PCAnimEventReceiveEvent;
uintptr_t ptr_PCMoveEventReceiveEvent;
uintptr_t ptr_CAnimEventReceiveEvent;
uintptr_t ptr_CMoveEventReceiveEvent;

_SendNotification SendNotification_Fn;
_Play_Native Play_Native;
_SetAnimationVariableFloat SetAnimationVariableFloat;
_SetInstanceVolume_Native SetInstanceVolume_Native;
_ShakeController ShakeController;
_ShakeCamera_Native ShakeCamera_Native;
_ApplyImageSpaceModifier ApplyImageSpaceModifier;
_GetMass GetMass;

AddressManager* AddressManager::instance = nullptr;

void AddressManager::FindAddresses() {
	MemoryRegion* mr = new MemoryRegion();
	PatternScanner::GetSkyrimMemoryRegion(mr);
	_MESSAGE("base : %llx, size : %u", mr->base, mr->size);

	/*unordered_map<string, vector<BYTE>> patterns;
	patterns.insert(pair<string, vector<BYTE>>("Notification", vector<BYTE>{0x48, 0x89, 0x74, 0x24, 0x70, 0x4C, 0x8B, 0xFA, 0x48, 0x8B, 0xE9}));
	patterns.insert(pair<string, vector<BYTE>>("Play", vector<BYTE>{0x48, 0x83, 0xEC, 0x30, 0x49, 0x8B, 0xF1, 0x49, 0x8B, 0xD8, 0x8B, 0xEA}));
	patterns.insert(pair<string, vector<BYTE>>("Damage", vector<BYTE>{0x48, 0x8B, 0x01, 0xBA, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x50, 0x08, 0x48, 0x8B, 0xD7}));
	patterns.insert(pair<string, vector<BYTE>>("SetAnimationVariableFloat", vector<BYTE>{0xF3, 0x0F, 0x11, 0x54, 0x24, 0x18, 0x48, 0x83, 0xEC, 0x28, 0x4C, 0x8D}));
	patterns.insert(pair<string, vector<BYTE>>("SetInstanceVolume", vector<BYTE>{0xF3, 0x0F, 0x10, 0x4C, 0x24, 0x60, 0x48, 0x8D, 0x4C}));
	patterns.insert(pair<string, vector<BYTE>>("God", vector<BYTE>{0x84, 0xC0, 0x0F, 0x94, 0xC1, 0xE8}));
	patterns.insert(pair<string, vector<BYTE>>("Pause/Resume", vector<BYTE>{0xE8, 0x00, 0x00, 0x00, 0x00, 0x33, 0xC9, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x84, 0xC0}));
	patterns.insert(pair<string, vector<BYTE>>("ShakeController", vector<BYTE>{0x29, 0x7C, 0x24, 0x50, 0x0F, 0x28, 0xF2}));
	patterns.insert(pair<string, vector<BYTE>>("ShakeCamera", vector<BYTE>{0x48, 0x83, 0xEC, 0x38, 0xF3, 0x0F, 0x10, 0x44, 0x24, 0x60, 0x0F}));
	patterns.insert(pair<string, vector<BYTE>>("ApplyImageSpaceModifier", vector<BYTE>{0x48, 0x89, 0x68, 0x18, 0x0F, 0x29, 0x70, 0xD8, 0x49, 0x8B, 0xF0}));
	patterns.insert(pair<string, vector<BYTE>>("Velocity", vector<BYTE>{0x45, 0x0F, 0x57, 0xC0, 0xC7, 0x45, 0xA0}));
	patterns.insert(pair<string, vector<BYTE>>("OnGround", vector<BYTE>{0x0F, 0x28, 0x33, 0x0F, 0x28, 0xEE}));
	patterns.insert(pair<string, vector<BYTE>>("Acceleration", vector<BYTE>{0xF3, 0x0F, 0x59, 0xF8, 0xF3, 0x0F, 0x11, 0xBE}));
	patterns.insert(pair<string, vector<BYTE>>("GetMass", vector<BYTE>{0x20, 0x0F, 0x57, 0xF6, 0x48, 0x8B, 0xD9}));
	patterns.insert(pair<string, vector<BYTE>>("AddVelocity", vector<BYTE>{0x76, 0x29, 0xF7, 0x81, 0x18}));

	unordered_map<string, uintptr_t> addresses = PatternScanner::PatternScanInternal(mr, patterns, true);

	auto finder = addresses.find("Notification");
	if (finder != addresses.end()) {
		ptr_Notification = finder->second - 0x21;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("Function - Notification %llx", ptr_Notification);
		SendNotification_Fn = (_SendNotification)ptr_Notification;
	}
	finder = addresses.find("Play");
	if (finder != addresses.end()) {
		ptr_PlayNative = finder->second - 0x10;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("Function - Play (Native) %llx", ptr_PlayNative);
		Play_Native = (_Play_Native)ptr_PlayNative;
	}

	finder = addresses.find("Damage");
	if (finder != addresses.end()) {
		ptr_DamageInjectionPoint = finder->second;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("Damage Hook Injection Point %llx", ptr_DamageInjectionPoint);
	}

	finder = addresses.find("SetAnimationVariableFloat");
	if (finder != addresses.end()) {
		ptr_SetAnimationVariableFloat = finder->second;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("Function - SetAnimationVariableFloat %llx", ptr_SetAnimationVariableFloat);
		SetAnimationVariableFloat = (_SetAnimationVariableFloat)ptr_SetAnimationVariableFloat;
	}

	finder = addresses.find("SetInstanceVolume");
	if (finder != addresses.end()) {
		ptr_SetInstanceVolumeToNative = finder->second - 0x9;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("Function - SetInstanceVolume (Native) %llx", ptr_SetInstanceVolumeToNative);
		SetInstanceVolume_Native = (_SetInstanceVolume_Native)ptr_SetInstanceVolumeToNative;
	}

	finder = addresses.find("God");
	if (finder != addresses.end()) {
		uintptr_t godFuncOffset = finder->second + 0x66;
		uintptr_t godValOffset = godFuncOffset + *(UInt32*)godFuncOffset + 0x6;
		ptr_GodMode = godValOffset + *(UInt32*)godValOffset + 0x4;
		_MESSAGE("God Mode Bool %llx", ptr_GodMode);
	}

	finder = addresses.find("Pause/Resume");
	if (finder != addresses.end()) {
		uintptr_t dataholderOffset = finder->second + 0x13;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		ptr_UnknownDataHolder = *(uintptr_t*)(dataholderOffset + *(UInt32*)dataholderOffset + 0x4);
		_MESSAGE("Unk Data Holder (For Pause/Resume) %llx", ptr_UnknownDataHolder); //0x160 = pause counter
	}

	finder = addresses.find("ShakeController");
	if (finder != addresses.end()) {
		ptr_ShakeController = finder->second - 0x15;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("Function - ShakeController %llx", ptr_ShakeController);
		ShakeController = (_ShakeController)ptr_ShakeController;
	}

	finder = addresses.find("ShakeCamera");
	if (finder != addresses.end()) {
		ptr_ShakeCameraNative = finder->second;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("Function - ShakeCamera (Native) %llx", ptr_ShakeCameraNative);
		ShakeCamera_Native = (_ShakeCamera_Native)ptr_ShakeCameraNative;
	}

	finder = addresses.find("ApplyImageSpaceModifier");
	if (finder != addresses.end()) {
		ptr_ApplyImageSpaceModifier = finder->second - 0x17;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("Function - Apply (ImageSpaceModifier) %llx", ptr_ApplyImageSpaceModifier);
		ApplyImageSpaceModifier = (_ApplyImageSpaceModifier)ptr_ApplyImageSpaceModifier;
	}

	finder = addresses.find("Velocity");
	if (finder != addresses.end()) {
		ptr_VelocityInjectionPoint = finder->second + 0x4;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("Velocity Hook Injection Point %llx", ptr_VelocityInjectionPoint);

		uintptr_t calculateFrictionOffset = ptr_VelocityInjectionPoint + 0x116;
		ptr_FrictionOverridePoint = (calculateFrictionOffset + *(UInt32*)(calculateFrictionOffset + 0x1) + 0x232);
		_MESSAGE("Friction Override Point %llx", ptr_FrictionOverridePoint);
	}

	finder = addresses.find("OnGround");
	if (finder != addresses.end()) {
		ptr_OnGroundVelocityOverridePoint = finder->second - 0x54;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("On Ground Velocity Override Point %llx", ptr_OnGroundVelocityOverridePoint);
	}

	finder = addresses.find("Notification");
	if (finder != addresses.end()) {
		ptr_AccelerationOverridePoint = finder->second;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("Acceleration Override Point %llx", ptr_AccelerationOverridePoint);

		uintptr_t engineTickOffset = ptr_AccelerationOverridePoint - 0x78;
		ptr_EngineTick = (engineTickOffset + *(UInt32*)(engineTickOffset + 0x4)) + 0x8;
		_MESSAGE("Engine Tick %llx", ptr_EngineTick);
	}

	finder = addresses.find("GetMass");
	if (finder != addresses.end()) {
		ptr_GetMass = finder->second - 0xA;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("Function - GetMass %llx", ptr_GetMass);
		GetMass = (_GetMass)ptr_GetMass;
	}

	finder = addresses.find("AddVelocity");
	if (finder != addresses.end()) {
		ptr_AddVelocityOverridePoint = finder->second + 0x1C;
		_MESSAGE("Finder %llx", finder->second - mr->base);
		_MESSAGE("AddVelocity Override Point %llx", ptr_AddVelocityOverridePoint);
	}*/

	ptr_Notification = mr->base + 0x8da3f1 - 0x21;
	_MESSAGE("Function - Notification %llx", ptr_Notification);
	SendNotification_Fn = (_SendNotification)ptr_Notification;

	ptr_PlayNative = mr->base + 0x9b4630 - 0x10;
	_MESSAGE("Function - Play (Native) %llx", ptr_PlayNative);
	Play_Native = (_Play_Native)ptr_PlayNative;

	ptr_DamageInjectionPoint = mr->base + 0x627d83;
	_MESSAGE("Damage Hook Injection Point %llx", ptr_DamageInjectionPoint);

	ptr_SetAnimationVariableFloat = mr->base + 0x4f0720;
	_MESSAGE("Function - SetAnimationVariableFloat %llx", ptr_SetAnimationVariableFloat);
	SetAnimationVariableFloat = (_SetAnimationVariableFloat)ptr_SetAnimationVariableFloat;

	ptr_SetInstanceVolumeToNative = mr->base + 0x9b4919 - 0x9;
	_MESSAGE("Function - SetInstanceVolume (Native) %llx", ptr_SetInstanceVolumeToNative);
	SetInstanceVolume_Native = (_SetInstanceVolume_Native)ptr_SetInstanceVolumeToNative;

	uintptr_t godFuncOffset = mr->base + 0x310E0B + 0x66;
	uintptr_t godValOffset = godFuncOffset + *(UInt32*)godFuncOffset + 0x6;
	ptr_GodMode = godValOffset + *(UInt32*)godValOffset + 0x4;
	_MESSAGE("God Mode Bool %llx", ptr_GodMode);

	uintptr_t dataholderOffset = mr->base + 0x5b35a3 + 0x13;
	ptr_UnknownDataHolder = *(uintptr_t*)(dataholderOffset + *(UInt32*)dataholderOffset + 0x4);
	_MESSAGE("Unk Data Holder (For Pause/Resume) %llx", ptr_UnknownDataHolder); //0x160 = pause counter

	ptr_ShakeController = mr->base + 0xc10625 - 0x15;
	_MESSAGE("Function - ShakeController %llx", ptr_ShakeController);
	ShakeController = (_ShakeController)ptr_ShakeController;

	ptr_ShakeCameraNative = mr->base + 0x9738d0;
	_MESSAGE("Function - ShakeCamera (Native) %llx", ptr_ShakeCameraNative);
	ShakeCamera_Native = (_ShakeCamera_Native)ptr_ShakeCameraNative;

	ptr_ApplyImageSpaceModifier = mr->base + 0x24c877 - 0x17;
	_MESSAGE("Function - Apply (ImageSpaceModifier) %llx", ptr_ApplyImageSpaceModifier);
	ApplyImageSpaceModifier = (_ApplyImageSpaceModifier)ptr_ApplyImageSpaceModifier;

	ptr_VelocityInjectionPoint = mr->base + 0xe1de6e + 0x4;
	_MESSAGE("Velocity Hook Injection Point %llx", ptr_VelocityInjectionPoint);

	uintptr_t calculateFrictionOffset = ptr_VelocityInjectionPoint + 0x116;
	ptr_FrictionOverridePoint = (calculateFrictionOffset + *(UInt32*)(calculateFrictionOffset + 0x1) + 0x232);
	_MESSAGE("Friction Override Point %llx", ptr_FrictionOverridePoint);

	ptr_OnGroundVelocityOverridePoint = mr->base + 0xe1e082 - 0x54;
	_MESSAGE("On Ground Velocity Override Point %llx", ptr_OnGroundVelocityOverridePoint);

	ptr_AccelerationOverridePoint = mr->base + 0x8da3f1;
	_MESSAGE("Acceleration Override Point %llx", ptr_AccelerationOverridePoint);

	uintptr_t engineTickOffset = ptr_AccelerationOverridePoint - 0x78;
	ptr_EngineTick = (engineTickOffset + *(UInt32*)(engineTickOffset + 0x4)) + 0x8;
	_MESSAGE("Engine Tick %llx", ptr_EngineTick);

	ptr_GetMass = mr->base + 0x3a5fea - 0xA;
	_MESSAGE("Function - GetMass %llx", ptr_GetMass);
	GetMass = (_GetMass)ptr_GetMass;

	ptr_AddVelocityOverridePoint = mr->base + 0xdc0a0e + 0x1C;
	_MESSAGE("AddVelocity Override Point %llx", ptr_AddVelocityOverridePoint);

	ptr_PCAnimEventReceiveEvent = mr->base + 0x1663F78;
	ptr_PCMoveEventReceiveEvent = mr->base + 0x16641B8;
	ptr_CAnimEventReceiveEvent = mr->base + 0x165E3B0;
	ptr_CMoveEventReceiveEvent = mr->base + 0x165E5F0;

	delete(mr);
}
