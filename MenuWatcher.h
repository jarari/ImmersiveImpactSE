#pragma once
#include <skse64\GameEvents.h>
#include <skse64\GameMenus.h>

class MenuWatcher : public BSTEventSink<MenuOpenCloseEvent> {
protected:
	static MenuWatcher* instance;
	static std::string className;
public:
	MenuWatcher() {
		if (instance)
			delete(instance);
		instance = this;
		_MESSAGE((className + std::string(" instance created.")).c_str());
		MenuManager* mm = MenuManager::GetSingleton();
		mm->MenuOpenCloseEventDispatcher()->AddEventSink(instance);
	}

	virtual ~MenuWatcher() {
		_MESSAGE((className + std::string(" instance destroyed.")).c_str());
	}

	static MenuWatcher* GetInstance() {
		return instance;
	}

	virtual EventResult ReceiveEvent(MenuOpenCloseEvent* evn, EventDispatcher<MenuOpenCloseEvent>* src) override;

	static void InitWatcher();

};