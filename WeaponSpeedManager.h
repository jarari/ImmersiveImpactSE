#pragma once
#include <vector>
using std::vector;
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
	void CompatibilityPatch();
	void EvaluateEvent(Actor* a, int evn);
}