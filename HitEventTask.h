#pragma once
#include <skse64\GameEvents.h>
#include <skse64\gamethreads.h>
#include <list>
using std::list;
class Actor;
class ActiveEffect;
class HitEventTask : public TaskDelegate {
public:
	static HitEventTask* Create(Actor* target, Actor* attacker, ActiveEffect* ae, float staggerMul, bool isArrow, int wepType);
	void UpdateDamage(float dmg);
	virtual void Run();
	virtual void Dispose();
	Actor* attacker;
	Actor* target;
private:
	ActiveEffect* ae;
	bool isArrow;
	int wepType;
	float damage;
	float staggerMul;
};

class HitEventPool {
	static list<HitEventTask*> tasks;
public:
	static void AddTask(HitEventTask* t);
	static void ProcessTask(Actor* attacker, Actor* target, float damage);
	static void ResetPool();
};