#pragma once
enum iSwingState {
	PrePre,
	Pre,
	Swing,
	Hit,
	End
};
enum iWepType {
	Fist,
	Dagger,
	OneH,
	TwoH,
	None
};
class Actor;
namespace WeaponSpeedManager {
	void ResetRestraintChecker();
	void EvaluateEvent(Actor* a, int evn);
}