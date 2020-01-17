#pragma once
enum iSwingState {
	PrePre,
	Pre,
	Swing,
	SwingL,
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
	void EvaluateEvent(Actor* a, int evn);
}