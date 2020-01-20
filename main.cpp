#include "ActorManager.h"
#include "AddressManager.h"
#include "AnimEventWatcher.h"
#include "ConfigManager.h"
#include "HitEventWatcher.h"
#include "HitEventTask.h"
#include "HitStopManager.h"
#include "MenuWatcher.h"
#include "ObjectLoadWatcher.h"
#include "StaggerTask.h"
#include "WeaponSpeedManager.h"
#include <common\IDebugLog.h>
#include <skse64_common\BranchTrampoline.h>
#include <skse64_common\skse_version.h>
#include <skse64\GameRTTI.h>
#include <skse64\PluginAPI.h>
#include <ShlObj.h>

IDebugLog	gLog;
const char* logPath = "\\My Games\\Skyrim Special Edition\\SKSE\\ImmersiveImpactSE.log";
const char* pluginName = "ImmersiveImpactSE";

PluginHandle	g_pluginHandle = kPluginHandle_Invalid;
SKSEMessagingInterface* g_message = nullptr;

extern "C" {

	bool SKSEPlugin_Query(const SKSEInterface* skse, PluginInfo* info) {
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath);
		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = pluginName;
		info->version = 1;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();

		if (skse->isEditor) {
			_MESSAGE("loaded in editor, marking as incompatible");

			return false;
		}
		else if (skse->runtimeVersion != RUNTIME_VERSION_1_5_80) {
			_MESSAGE("You're running this mod on runtime version %08X. This mod is designed for 1.5.80 so it may not work perfectly.", skse->runtimeVersion);

		}
		if (!g_branchTrampoline.Create(1024 * 64)) {
			_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return false;
		}

		if (!g_localTrampoline.Create(1024 * 64)) {
			_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return false;
		}
		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface* skse) {
		ConfigManager* cm = new ConfigManager();
		g_message = (SKSEMessagingInterface*) skse->QueryInterface(kInterface_Messaging);
		g_message->RegisterListener(skse->GetPluginHandle(), "SKSE", [](SKSEMessagingInterface::Message* msg) -> void {
			if (msg->type == SKSEMessagingInterface::kMessage_PreLoadGame || msg->type == SKSEMessagingInterface::kMessage_NewGame) {
				HitEventPool::ResetPool();
				StaggerPool::ResetPool();
				ConfigManager::GetInstance()->LoadConfigs();
				WeaponSpeedManager::ResetRestraintChecker();
			}
			else if (msg->type == SKSEMessagingInterface::kMessage_DataLoaded) {
				ConfigManager::GetInstance()->LoadConfigs();
				ConfigManager::GetInstance()->UpdateINIWithCurrentValues();
				AddressManager* am = new AddressManager();
				am->FindAddresses();
				ActorManager::FindDeflectSound();
				HitStopManager::FindBlurEffect();
				HitStopManager::UnleashCameraShakeLimit();
				WeaponSpeedManager::CompatibilityPatch();
				StaggerHelper::Register();
				MenuWatcher::InitWatcher();
				HitEventWatcher::InitWatcher();
				ObjectLoadWatcher::InitWatcher();
				AnimEventWatcher* ae = static_cast<AnimEventWatcher*>(&(*g_thePlayer)->animGraphEventSink);
				ae->HookSink();
				_MESSAGE("Player : %llx", *g_thePlayer);
			}
		});
		return true;
	}
};