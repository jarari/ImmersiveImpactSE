#include "ConfigManager.h"
#include "HitEventWatcher.h"
#include "PhysicsManager.h"

ConfigManager* ConfigManager::instance = nullptr;
vector<Config> ConfigManager::configs = vector<Config>();
vector<Config> ConfigManager::physconfigs = vector<Config>();
CSimpleIniA ConfigManager::ini(true, false, false);
const char* ConfigManager::filepath = "Data\\SKSE\\Plugins\\ImmersiveImpactSE.ini";

bool FileExists(const char* path) {
	struct stat buffer;
	return (stat(path, &buffer) == 0);
}

void ConfigManager::InitializeConfigDefaults() {
	configs.clear();
	configs.push_back(Config("EnableWeaponSpeed", 1));
	configs.push_back(Config("Speed_Fist_PrePre", 0.6f));
	configs.push_back(Config("Speed_Fist_Pre", 0.7f));
	configs.push_back(Config("Speed_Fist_Swing", 1.3f));
	configs.push_back(Config("Speed_Fist_Hit", 1.4f));
	configs.push_back(Config("Speed_Fist_End", 1.0f));
	configs.push_back(Config("Speed_Fist_PowerAttack_PrePre", 0.8f));
	configs.push_back(Config("Speed_Fist_PowerAttack_Pre", 0.9f));
	configs.push_back(Config("Speed_Fist_PowerAttack_Swing", 1.4f));
	configs.push_back(Config("Speed_Fist_PowerAttack_Hit", 1.5f));
	configs.push_back(Config("Speed_Fist_PowerAttack_End", 0.9f));
	configs.push_back(Config("Speed_Dagger_PrePre", 0.8f));
	configs.push_back(Config("Speed_Dagger_Pre", 0.8f));
	configs.push_back(Config("Speed_Dagger_Swing", 1.0f));
	configs.push_back(Config("Speed_Dagger_Hit", 1.1f));
	configs.push_back(Config("Speed_Dagger_End", 1.0f));
	configs.push_back(Config("Speed_Dagger_PowerAttack_PrePre", 0.8f));
	configs.push_back(Config("Speed_Dagger_PowerAttack_Pre", 0.8f));
	configs.push_back(Config("Speed_Dagger_PowerAttack_Swing", 1.0f));
	configs.push_back(Config("Speed_Dagger_PowerAttack_Hit", 1.1f));
	configs.push_back(Config("Speed_Dagger_PowerAttack_End", 1.0f));
	configs.push_back(Config("Speed_1H_PrePre", 0.6f));
	configs.push_back(Config("Speed_1H_Pre", 0.7f));
	configs.push_back(Config("Speed_1H_Swing", 1.5f));
	configs.push_back(Config("Speed_1H_Hit", 1.6f));
	configs.push_back(Config("Speed_1H_End", 0.9f));
	configs.push_back(Config("Speed_1H_PowerAttack_PrePre", 0.9f));
	configs.push_back(Config("Speed_1H_PowerAttack_Pre", 1.0f));
	configs.push_back(Config("Speed_1H_PowerAttack_Swing", 1.5f));
	configs.push_back(Config("Speed_1H_PowerAttack_Hit", 1.6f));
	configs.push_back(Config("Speed_1H_PowerAttack_End", 0.8f));
	configs.push_back(Config("Speed_2H_PrePre", 0.7f));
	configs.push_back(Config("Speed_2H_Pre", 0.8f));
	configs.push_back(Config("Speed_2H_Swing", 1.7f));
	configs.push_back(Config("Speed_2H_Hit", 1.8f));
	configs.push_back(Config("Speed_2H_End", 0.7f));
	configs.push_back(Config("Speed_2H_PowerAttack_PrePre", 1.0f));
	configs.push_back(Config("Speed_2H_PowerAttack_Pre", 1.1f));
	configs.push_back(Config("Speed_2H_PowerAttack_Swing", 1.7f));
	configs.push_back(Config("Speed_2H_PowerAttack_Hit", 1.8f));
	configs.push_back(Config("Speed_2H_PowerAttack_End", 0.7f));
	configs.push_back(Config("FakeFist", 0));
	configs.push_back(Config("RestrainMovement", 1));
	configs.push_back(Config("RestrainAim", 1));
	configs.push_back(Config("EnableHitFeedback", 1));
	configs.push_back(Config("DeflectChanceMul", 10.0f));
	configs.push_back(Config("DeflectChanceMax", 50.0f));
	configs.push_back(Config("StaggerResetTime", 2.0f));
	configs.push_back(Config("StaggerLimit", 3.0f));
	configs.push_back(Config("StaggerDamageMax", 100.0f));
	configs.push_back(Config("StaggerAny", 0));
	configs.push_back(Config("StaggerArmorCap", 300.0f));
	configs.push_back(Config("StaggerIgnoreArrow", 1));
	configs.push_back(Config("EnableKnockback", 0));
	configs.push_back(Config("Knockback_Fist", 10.0f));
	configs.push_back(Config("Knockback_Dagger", 5.0f));
	configs.push_back(Config("Knockback_1H", 10.0f));
	configs.push_back(Config("Knockback_2H", 15.0f));
	configs.push_back(Config("Knockback_LastMultiplier", 1.5f));
	configs.push_back(Config("EnableDash", 0));
	configs.push_back(Config("Dash_Fist", 20.0f));
	configs.push_back(Config("Dash_Dagger", 15.0f));
	configs.push_back(Config("Dash_1H", 20.0f));
	configs.push_back(Config("Dash_2H", 30.0f));
	configs.push_back(Config("EnableHitStop", 0));
	configs.push_back(Config("HitStop_OnPlayerHit", 0));
	configs.push_back(Config("HitStop_OnObjectHit", 0));
	configs.push_back(Config("HitStop_Fist", 0.075f));
	configs.push_back(Config("HitStop_Dagger", 0.05f));
	configs.push_back(Config("HitStop_1H", 0.075f));
	configs.push_back(Config("HitStop_2H", 0.1f));
	configs.push_back(Config("HitStop_FovStep", 0.15f));
	configs.push_back(Config("HitStop_SyncFist", 0.075f));
	configs.push_back(Config("HitStop_SyncDagger", 0.05f));
	configs.push_back(Config("HitStop_Sync1H", 0.075f));
	configs.push_back(Config("HitStop_Sync2H", 0.1f));
	configs.push_back(Config("EnableHitShakeController", 0));
	configs.push_back(Config("HitShakeController_OnPlayerHit", 1));
	configs.push_back(Config("HitShakeController_OnObjectHit", 1));
	configs.push_back(Config("HitShakeController_Fist", 0.3f));
	configs.push_back(Config("HitShakeController_Fist_Duration", 0.25f));
	configs.push_back(Config("HitShakeController_Dagger", 0.2f));
	configs.push_back(Config("HitShakeController_Dagger_Duration", 0.2f));
	configs.push_back(Config("HitShakeController_1H", 0.4f));
	configs.push_back(Config("HitShakeController_1H_Duration", 0.25f));
	configs.push_back(Config("HitShakeController_2H", 0.5f));
	configs.push_back(Config("HitShakeController_2H_Duration", 0.35f));
	configs.push_back(Config("EnableHitShakeCam", 0));
	configs.push_back(Config("HitShakeCam_OnPlayerHit", 1));
	configs.push_back(Config("HitShakeCam_OnObjectHit", 1));
	configs.push_back(Config("HitShakeCam_Fist", 0.3f));
	configs.push_back(Config("HitShakeCam_Fist_Duration", 0.25f));
	configs.push_back(Config("HitShakeCam_Dagger", 0.2f));
	configs.push_back(Config("HitShakeCam_Dagger_Duration", 0.2f));
	configs.push_back(Config("HitShakeCam_1H", 0.4f));
	configs.push_back(Config("HitShakeCam_1H_Duration", 0.25f));
	configs.push_back(Config("HitShakeCam_2H", 0.5f));
	configs.push_back(Config("HitShakeCam_2H_Duration", 0.35f));
	configs.push_back(Config("EnableHitBlur", 0));
	configs.push_back(Config("HitBlur_OnPlayerHit", 1));
	configs.push_back(Config("HitBlur_OnObjectHit", 1));
	configs.push_back(Config("HitBlur_Fist", 0.3f));
	configs.push_back(Config("HitBlur_Fist_Duration", 0.25f));
	configs.push_back(Config("HitBlur_Dagger", 0.2f));
	configs.push_back(Config("HitBlur_Dagger_Duration", 0.2f));
	configs.push_back(Config("HitBlur_1H", 0.4f));
	configs.push_back(Config("HitBlur_1H_Duration", 0.25f));
	configs.push_back(Config("HitBlur_2H", 0.5f));
	configs.push_back(Config("HitBlur_2H_Duration", 0.35f));

	physconfigs.clear();
	physconfigs.push_back(Config("Tick", 0.016667f));
	physconfigs.push_back(Config("DefaultFriction", 0.75f));
	physconfigs.push_back(Config("DefaultAirDrag", 1.0f));
}

void ConfigManager::InitializeINI() {
	char f2c[64];
	for (int i = 0; i < configs.size(); i++) {
		sprintf_s(f2c, "%f", configs[i].value);
		ini.SetValue("General", configs[i].name, f2c);
	}
	for (int i = 0; i < physconfigs.size(); i++) {
		sprintf_s(f2c, "%f", physconfigs[i].value);
		ini.SetValue("Phys", physconfigs[i].name, f2c);
	}
	ini.SaveFile(filepath, false);
	SI_Error error = ini.LoadFile(filepath);
	if (error < 0) {
		_MESSAGE("Error loading ini data!");
		return;
	}
}

void ConfigManager::LoadConfigs() {
	InitializeConfigDefaults();

	if (!FileExists(filepath))
		InitializeINI();

	SI_Error error = ini.LoadFile(filepath);
	if (error < 0) {
		_MESSAGE("Error loading ini data!");
		return;
	}

	char f2c[64];
	for (int i = 0; i < configs.size(); i++) {
		sprintf_s(f2c, "%f", configs[i].value);
		configs[i].value = std::stof(ini.GetValue("General", configs[i].name, f2c, NULL));
	}
	for (int i = 0; i < physconfigs.size(); i++) {
		sprintf_s(f2c, "%f", physconfigs[i].value);
		physconfigs[i].value = std::stof(ini.GetValue("Phys", physconfigs[i].name, f2c, NULL));
	}
	PhysData::tick = physconfigs[0].value;
	PhysicsManager::defaultFriction = physconfigs[1].value;
	PhysicsManager::defaultDrag = physconfigs[2].value;
}

void ConfigManager::UpdateINIWithCurrentValues() {
	char f2c[64];
	for (int i = 0; i < configs.size(); i++) {
		sprintf_s(f2c, "%f", configs[i].value);
		ini.SetValue("General", configs[i].name, f2c);
	}
	for (int i = 0; i < physconfigs.size(); i++) {
		sprintf_s(f2c, "%f", physconfigs[i].value);
		ini.SetValue("Phys", physconfigs[i].name, f2c);
	}
	ini.SaveFile(filepath, false);
}
