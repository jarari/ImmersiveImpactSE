#pragma once
#include <common\ICriticalSection.h>
#include <skse64\GameReferences.h>
#include <skse64\NiTypes.h>
#include <chrono>
#include <unordered_map>
using std::unordered_map;

typedef float  hkReal;
class hkVector4 {
public:
	hkVector4() { x = 0; y = 0; z = 0; w = 0; };
	hkVector4(NiPoint3 p) {
		x = p.x;
		y = p.y;
		z = p.z;
		w = 0.0f;
	}
	hkVector4(float _x, float _y, float _z, float _w = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	__declspec(align(16)) hkReal x;
	hkReal y;
	hkReal z;
	hkReal w;
	hkVector4& operator=(const hkVector4& v) {
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = v.w;
		return *this;
	}
	hkVector4 operator+(const hkVector4& v) {
		return hkVector4(x + v.x, y + v.y, z + v.z);
	}
	hkVector4& operator+=(const hkVector4& v) {
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		this->w += v.w;
		return *this;
	}
	hkVector4 operator-(const hkVector4& v) {
		return hkVector4(x - v.x, y - v.y, z - v.z);
	}
	hkVector4& operator-=(const hkVector4& v) {
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		this->w -= v.w;
		return *this;
	}
	hkVector4 operator*(float a) {
		return hkVector4(x * a, y * a, z * a, w * a);
	}
	hkVector4& operator*=(float a) {
		this->x *= a;
		this->y *= a;
		this->z *= a;
		this->w *= a;
		return *this;
	}
	hkVector4 operator*(const hkVector4& v) {
		return hkVector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}
	hkVector4& operator*=(const hkVector4& v) {
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		this->w *= v.w;
		return *this;
	}
	hkVector4 operator/(float a) {
		return hkVector4(x / a, y / a, z / a);
	}
	hkVector4& operator/=(float a) {
		this->x /= a;
		this->y /= a;
		this->z /= a;
		this->w /= a;
		return *this;
	}
	hkVector4 operator/(const hkVector4& v) {
		return hkVector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}
	hkVector4& operator/=(const hkVector4& v) {
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;
		this->w /= v.w;
		return *this;
	}
	float Length() {
		return sqrt(x * x + y * y + z * z);
	}
	hkVector4& Normalize() {
		float l = Length();
		if (l == 0) {
			this->x = 0;
			this->y = 0;
			this->z = 0;
			return *this;
		}
		this->x /= l;
		this->y /= l;
		this->z /= l;
		return *this;
	}
	operator NiPoint3() {
		return NiPoint3(x, y, z);
	}
};

class bhkCharacterController;
struct PhysData {
	float friction;
	float airdrag;
	std::chrono::system_clock::time_point lastRun;
	hkVector4 velocity;
	hkVector4 currentVelocity;
	PhysData() {
		lastRun = std::chrono::system_clock::now();
		velocity = hkVector4();
	}
	PhysData(float f, float d) : PhysData() {
		friction = f;
		airdrag = d;
	}
};

namespace PhysicsManager {
	extern int tick;
	extern float defaultFriction;
	extern float defaultDrag;
	static ICriticalSection data_Lock;
	static unordered_map<UInt64, PhysData> datamap;
	hkVector4 GetAccelerationMultiplier(bhkCharacterController* cCon, hkVector4 limit);
	void HookOnGroundVelocity();
	PhysData* GetData(Actor* a);
	void AddVelocity(Actor* a, hkVector4 vel);
	void SetFriction(Actor* a, float f);
	void SetDrag(Actor* a, float f);
	bool Simulate(Actor* a);
	void InitializeData(Actor* c);
	void ResetPhysics();
	void ResetTimer();
}