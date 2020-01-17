#include "ConfigManager.h"
#include "HitEventWatcher.h"

ConfigManager* ConfigManager::instance = nullptr;
vector<Config> ConfigManager::configs = vector<Config>();
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
	configs.push_back(Config("Speed_Fist_Swing", 1));
	configs.push_back(Config("Speed_Fist_Hit", 1));
	configs.push_back(Config("Speed_Fist_End", 1));
	configs.push_back(Config("Speed_Dagger_PrePre", 0.8f));
	configs.push_back(Config("Speed_Dagger_Pre", 0.8f));
	configs.push_back(Config("Speed_Dagger_Swing", 1.0f));
	configs.push_back(Config("Speed_Dagger_Hit", 1.1f));
	configs.push_back(Config("Speed_Dagger_End", 1.0f));
	configs.push_back(Config("Speed_1H_PrePre", 0.6f));
	configs.push_back(Config("Speed_1H_Pre", 0.7f));
	configs.push_back(Config("Speed_1H_Swing", 1.2f));
	configs.push_back(Config("Speed_1H_Hit", 1.3f));
	configs.push_back(Config("Speed_1H_End", 0.9f));
	configs.push_back(Config("Speed_2H_PrePre", 0.7f));
	configs.push_back(Config("Speed_2H_Pre", 0.8f));
	configs.push_back(Config("Speed_2H_Swing", 1.2f));
	configs.push_back(Config("Speed_2H_Hit", 1.25f));
	configs.push_back(Config("Speed_2H_End", 0.7f));
	configs.push_back(Config("RestrainMovement", 1));
	configs.push_back(Config("RestrainAim", 1));
	configs.push_back(Config("EnableHitFeedback", 1));
	configs.push_back(Config("DeflectChanceMul", 10.0f));
	configs.push_back(Config("DeflectChanceMax", 50.0f));
	configs.push_back(Config("StaggerResetTime", 2.0f));
	configs.push_back(Config("StaggerLimit", 3.0f));
	configs.push_back(Config("StaggerDamageMax", 100.0f));
	configs.push_back(Config("StaggerAny", 0));
	configs.push_back(Config("EnableDash", 0));
	configs.push_back(Config("StaggerArmorCap", 300.0f));
}

void ConfigManager::InitializeINI() {
	char f2c[16];
	for (int i = 0; i < configs.size(); i++) {
		sprintf_s(f2c, "%f", configs[i].value);
		ini.SetValue("General", configs[i].name, f2c);
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

	char f2c[16];
	for (int i = 0; i < configs.size(); i++) {
		sprintf_s(f2c, "%f", configs[i].value);
		configs[i].value = std::stof(ini.GetValue("General", configs[i].name, f2c, NULL));
	}

	if (!configs[iConfigType::EnableHitFeedback].value) {
		HitEventWatcher::GetInstance()->RemoveHook();
	}
}
