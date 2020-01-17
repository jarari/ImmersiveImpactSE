#pragma once
struct TESHitEvent;
class TESImageSpaceModifier;
namespace HitStopManager {
	static TESImageSpaceModifier* blurModifier;
	void FindBlurEffect();
	void EvaluateEvent(TESHitEvent* evn);
}