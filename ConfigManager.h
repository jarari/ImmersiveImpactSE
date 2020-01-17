#pragma once
#include "INILibrary\SimpleIni.h"
#include <vector>
using std::vector;
struct Config {
	const char* name;
	float value;
public:
	Config() {
		name = "";
		value = 0;
	}
	Config(const char* n, float v) {
		name = n;
		value = v;
	}
};

enum iConfigType {
	EnableWeaponSpeed,
	Speed_Fist_PrePre,
	Speed_Fist_Pre,
	Speed_Fist_Swing,
	Speed_Fist_Hit,
	Speed_Fist_End,
	Speed_Dagger_PrePre,
	Speed_Dagger_Pre,
	Speed_Dagger_Swing,
	Speed_Dagger_Hit,
	Speed_Dagger_End,
	Speed_1H_PrePre,
	Speed_1H_Pre,
	Speed_1H_Swing,
	Speed_1H_Hit,
	Speed_1H_End,
	Speed_2H_PrePre,
	Speed_2H_Pre,
	Speed_2H_Swing,
	Speed_2H_Hit,
	Speed_2H_End,
	RestrainMovement,
	RestrainAim,
	EnableHitFeedback,
	DeflectChanceMul,
	DeflectChanceMax,
	StaggerResetTime,
	StaggerLimit,
	StaggerDamageMax,
	StaggerAny,
	EnableDash,
	StaggerArmorCap,
	EnableHitStop,
	HitStop_OnPlayerHit,
	HitStop_OnObjectHit,
	HitStop_Fist,
	HitStop_Dagger,
	HitStop_1H,
	HitStop_2H,
	EnableHitShakeController,
	HitShakeController_OnPlayerHit,
	HitShakeController_OnObjectHit,
	HitShakeController_Fist,
	HitShakeController_Dagger,
	HitShakeController_1H,
	HitShakeController_2H,
	EnableHitShakeCam,
	HitShakeCam_OnPlayerHit,
	HitShakeCam_OnObjectHit,
	HitShakeCam_Fist,
	HitShakeCam_Dagger,
	HitShakeCam_1H,
	HitShakeCam_2H,
	EnableHitBlur,
	HitBlur_OnPlayerHit,
	HitBlur_OnObjectHit,
	HitBlur_Fist,
	HitBlur_Dagger,
	HitBlur_1H,
	HitBlur_2H
};

class ConfigManager {
	static ConfigManager* instance;
	static vector<Config> configs;
	static CSimpleIniA ini;
	static const char* filepath;
public:
	ConfigManager() {
		if (instance)
			delete(instance);
		instance = this;
		_MESSAGE("Config manager instance created.");
	}

	static ConfigManager* GetInstance() {
		return instance;
	}

	static vector<Config> GetConfig() {
		return configs;
	}

	void InitializeConfigDefaults();

	void InitializeINI();

	void LoadConfigs();

	void UpdateINIWithCurrentValues();
};