#pragma once
#include "AddressManager.h"
#include <skse64\GameMenus.h>
#include <skse64\gamethreads.h>
#include <skse64\PluginAPI.h>
#include <list>
using std::list;
class Actor;

class StaggerTask : public TaskDelegate {
public:
	static StaggerTask* Create(Actor* attcker, Actor* target, float dir, float mag);
	virtual void Run();
	virtual void Dispose();
	int counter;
private:
	Actor* attacker;
	Actor* target;
	float dir;
	float mag;
};

class StaggerPool {
	static list<StaggerTask*> tasks;
public:
	static void AddTask(StaggerTask* t);
	static void ProcessTasks();
	static void ResetPool();
};

class StaggerHelper : public IMenu {
	static StaggerHelper* instance;
public:
	StaggerHelper() {
		if (instance)
			delete(instance);
		instance = this;
		InitMovie();
		unk0C = 0x2;
		flags = 0x18902;
	}
	static void Register() {
		MenuManager* mm = MenuManager::GetSingleton();
		if (!mm)
			return;
		mm->Register("BingleStaggerHelper", []()->IMenu * {
			StaggerHelper* helper = new StaggerHelper();
			return helper;
		});
	}
	static StaggerHelper* GetInstance() {
		return instance;
	}
	virtual void Render() override;
};