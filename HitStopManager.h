#pragma once
#include <skse64\GameMenus.h>
#include <common\ICriticalSection.h>
#include <queue>
#include <thread>
using std::thread;
using std::queue;

struct TESHitEvent;
class TESImageSpaceModifier;
namespace HitStopManager {
	static TESImageSpaceModifier* blurModifier;
	void FindBlurEffect();
	void UnleashCameraShakeLimit();
	void EvaluateEvent(TESHitEvent* evn);
}

namespace HitStopThreadManager {
	static ICriticalSection threadQueue_Lock;
	static ICriticalSection gamePause_Lock;
	static bool running;
	static queue<thread*> threadQueue;
	static thread* lastThread;
	void RequestLaunch();
}