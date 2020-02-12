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
	extern TESImageSpaceModifier* blurModifier;
	extern std::chrono::system_clock::time_point lastEffect;
	void FindBlurEffect();
	void UnleashCameraShakeLimit();
	void EvaluateEvent(TESHitEvent* evn);
}

namespace HitStopThreadManager {
	extern ICriticalSection threadQueue_Lock;
	extern ICriticalSection gamePause_Lock;
	extern bool running;
	extern queue<thread*> threadQueue;
	extern thread* lastThread;
	void RequestLaunch();
}