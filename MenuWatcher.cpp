#include "AddressManager.h"
#include "MenuWatcher.h"
#include "PhysicsManager.h"
#include "StaggerTask.h"
#include "Utils.h"
#include <Psapi.h>
#include <unordered_map>
#include "HitEventTask.h"
#include "WeaponSpeedManager.h"
#include "ConfigManager.h"
using std::unordered_map;

std::string MenuWatcher::className = "MenuWatcher";
MenuWatcher* MenuWatcher::instance = nullptr;

LPCSTR moduleNames[] = {
	"hdtSSEPhysics.dll",
	"hdtSMP64.dll"
};

class FreezeEventHandler : public BSTEventSink<MenuOpenCloseEvent> {
public:
	typedef EventResult(FreezeEventHandler::* FnReceiveEvent)(MenuOpenCloseEvent* evn, BSTEventSource<MenuOpenCloseEvent>* dispatcher);

	static unordered_map<UInt64, FnReceiveEvent> fnHash;

	EventResult ReceiveEventHook(MenuOpenCloseEvent* evn, BSTEventSource<MenuOpenCloseEvent>* dispatcher) {
		static BSFixedString menuName = "BingleStaggerHelper";

		if (evn->menuName == menuName) {
			return kEvent_Continue;
		}

		FnReceiveEvent fn = fnHash.at(*(UInt64*)this);
		return fn ? (this->*fn)(evn, dispatcher) : kEvent_Continue;
	}

	void InstallHook() {
		FnReceiveEvent fn = Utils::SafeWrite64Alt(*(UInt64*)this + 0x8, &FreezeEventHandler::ReceiveEventHook);
		fnHash.insert(std::pair<UInt64, FnReceiveEvent>(*(UInt64*)this, fn));
		_MESSAGE("Patched HDT function at %llx", *(UInt64*)this + 0x8);
	}
};
unordered_map<UInt64, FreezeEventHandler::FnReceiveEvent> FreezeEventHandler::fnHash;

class MenuOpenCloseEventSource : public EventDispatcherEx<MenuOpenCloseEvent> {
public:
	void ProcessHook() {
		HMODULE hdtHandle;
		for (int i = 0; i < sizeof(moduleNames) / sizeof(LPCSTR); i++) {
			_MESSAGE("Getting handle by name : %s", moduleNames[i]);
			hdtHandle = GetModuleHandleA(moduleNames[i]);
			if (hdtHandle) {
				_MESSAGE("Success!");
				break;
			}
		}
		if (!hdtHandle) {
			_MESSAGE("Freeze patch failed : Could not retrieve the handle of HDT-SMP dll!");
			return;
		}
		MODULEINFO mi{};
		GetModuleInformation(GetCurrentProcess(), hdtHandle, &mi, sizeof(mi));

		lock.Lock();

		BSTEventSink<MenuOpenCloseEvent>* sink;
		UInt32 idx = 0;
		while (eventSinks.GetNthItem(idx, sink)) {
			if ((uintptr_t)sink >= (uintptr_t)hdtHandle && (uintptr_t)sink <= (uintptr_t)hdtHandle + mi.SizeOfImage) {
				_MESSAGE("Found FreezeEventHandler! Patching...");
				((FreezeEventHandler*)sink)->InstallHook();
			}
			++idx;
		}

		lock.Release();
	}

	static void InitHook() {
		MenuManager* mm = MenuManager::GetSingleton();
		if (mm) {
			MenuOpenCloseEventSource* pThis = (MenuOpenCloseEventSource*)(mm->MenuOpenCloseEventDispatcher());
			pThis->ProcessHook();
		}
	}
};

void MenuWatcher::InitWatcher() {
	if (!instance) {
		instance = new MenuWatcher();
		MenuOpenCloseEventSource::InitHook();
	}
	_MESSAGE((className + std::string(" initialized.")).c_str());
}

EventResult MenuWatcher::ReceiveEvent(MenuOpenCloseEvent* evn, EventDispatcher<MenuOpenCloseEvent>* src) {
	MenuManager* mm = MenuManager::GetSingleton();
	UIManager* ui = UIManager::GetSingleton();
	UIStringHolder* uistr = UIStringHolder::GetSingleton();
	if (evn->menuName == uistr->loadingMenu) {
		if (!evn->opening) {
			if (!mm->IsMenuOpen(&BSFixedString("BingleStaggerHelper"))) {
				CALL_MEMBER_FN(ui, AddMessage)(&BSFixedString("BingleStaggerHelper"), UIMessage::kMessage_Open, nullptr);
				_MESSAGE("Helper created");
			}
		}
		else if (evn->opening) {
			ConfigManager::GetInstance()->LoadConfigs();
			HitEventPool::ResetPool();
			StaggerPool::ResetPool();
			PhysicsManager::ResetPhysics();
			WeaponSpeedManager::ResetRestraintChecker();
		}
	}
	
	return kEvent_Continue;
}