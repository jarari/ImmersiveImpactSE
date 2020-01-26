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
	Speed_Fist_PowerAttack_PrePre,
	Speed_Fist_PowerAttack_Pre,
	Speed_Fist_PowerAttack_Swing,
	Speed_Fist_PowerAttack_Hit,
	Speed_Fist_PowerAttack_End,
	Speed_Dagger_PrePre,
	Speed_Dagger_Pre,
	Speed_Dagger_Swing,
	Speed_Dagger_Hit,
	Speed_Dagger_End,
	Speed_Dagger_PowerAttack_PrePre,
	Speed_Dagger_PowerAttack_Pre,
	Speed_Dagger_PowerAttack_Swing,
	Speed_Dagger_PowerAttack_Hit,
	Speed_Dagger_PowerAttack_End,
	Speed_1H_PrePre,
	Speed_1H_Pre,
	Speed_1H_Swing,
	Speed_1H_Hit,
	Speed_1H_End,
	Speed_1H_PowerAttack_PrePre,
	Speed_1H_PowerAttack_Pre,
	Speed_1H_PowerAttack_Swing,
	Speed_1H_PowerAttack_Hit,
	Speed_1H_PowerAttack_End,
	Speed_2H_PrePre,
	Speed_2H_Pre,
	Speed_2H_Swing,
	Speed_2H_Hit,
	Speed_2H_End,
	Speed_2H_PowerAttack_PrePre,
	Speed_2H_PowerAttack_Pre,
	Speed_2H_PowerAttack_Swing,
	Speed_2H_PowerAttack_Hit,
	Speed_2H_PowerAttack_End,
	FakeFist,
	RestrainMovement,
	RestrainAim,
	EnableHitFeedback,
	DeflectChanceMul,
	DeflectChanceMax,
	StaggerResetTime,
	StaggerLimit,
	StaggerDamageMax,
	StaggerAny,
	StaggerArmorCap,
	StaggerIgnoreArrow,
	EnableKnockback,
	Knockback_Fist,
	Knockback_Dagger,
	Knockback_1H,
	Knockback_2H,
	Knockback_LastMultiplier,
	EnableDash,
	Dash_Fist,
	Dash_Dagger,
	Dash_1H,
	Dash_2H,
	EnableHitStop,
	HitStop_OnPlayerHit,
	HitStop_OnObjectHit,
	HitStop_Fist,
	HitStop_Dagger,
	HitStop_1H,
	HitStop_2H,
	HitStop_FovStep,
	HitStop_SyncFist,
	HitStop_SyncDagger,
	HitStop_Sync1H,
	HitStop_Sync2H,
	EnableHitShakeController,
	HitShakeController_OnPlayerHit,
	HitShakeController_OnObjectHit,
	HitShakeController_Fist,
	HitShakeController_Fist_Duration,
	HitShakeController_Dagger,
	HitShakeController_Dagger_Duration,
	HitShakeController_1H,
	HitShakeController_1H_Duration,
	HitShakeController_2H,
	HitShakeController_2H_Duration,
	EnableHitShakeCam,
	HitShakeCam_OnPlayerHit,
	HitShakeCam_OnObjectHit,
	HitShakeCam_Fist,
	HitShakeCam_Fist_Duration,
	HitShakeCam_Dagger,
	HitShakeCam_Dagger_Duration,
	HitShakeCam_1H,
	HitShakeCam_1H_Duration,
	HitShakeCam_2H,
	HitShakeCam_2H_Duration,
	EnableHitBlur,
	HitBlur_OnPlayerHit,
	HitBlur_OnObjectHit,
	HitBlur_Fist,
	HitBlur_Fist_Duration,
	HitBlur_Dagger,
	HitBlur_Dagger_Duration,
	HitBlur_1H,
	HitBlur_1H_Duration,
	HitBlur_2H,
	HitBlur_2H_Duration
};

class ConfigManager {
	static ConfigManager* instance;
	static vector<Config> configs;
	static vector<Config> physconfigs;
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

	static CSimpleIniA* GetINI() {
		return &ini;
	}

	void InitializeConfigDefaults();

	void InitializeINI();

	void LoadConfigs();

	void UpdateINIWithCurrentValues();
};