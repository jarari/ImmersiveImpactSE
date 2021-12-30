#include "AddressManager.h"
#include "PhysicsManager.h"
#include "Utils.h"
#include <skse64_common\BranchTrampoline.h>
#include <skse64_common\SafeWrite.h>
#include <xbyak\xbyak.h>

int PhysicsManager::tick = 16666;
unordered_map<Actor*, VelocityData> PhysicsManager::velMap;
unordered_map<Actor*, VelocityData> PhysicsManager::queueMap;
typedef bool (*_GetVelocity)(bhkCharacterController* con, const hkVector4& vel);
typedef bool (*_SetVelocity)(bhkCharacterController* con, const hkVector4& vel);

/*hkVector4 PhysicsManager::GetAccelerationMultiplier(bhkCharacterController* cCon, hkVector4 limit, bool local) {
	Actor* a = (Actor*)(**(UInt64 **)((UInt64)cCon + 0x10) - 0xD0);
	_GetVelocity GetVelocity = *(_GetVelocity*)(*(UInt64*)cCon + 0x30);
	if (datamap.count((UInt64)a)) {
		PhysData* pd = GetData(a);
		if (!pd)
			return hkVector4(1.0f, 1.0f, 1.0f);
		hkVector4 curr = hkVector4(0, 0, 0);
		GetVelocity(cCon, curr);
		if (local)
			curr = Utils::WorldToLocal(curr, NiPoint3(), Utils::GetRotationMatrix33(0, -a->rot.z, 0));
		hkVector4 mult = hkVector4(1.0f, 1.0f, 1.0f);
		if (limit.x != 0) {
			if (curr.x * limit.x >= 0) { //same sign
				mult.x = max(abs(limit.x) - abs(curr.x), 0) / abs(limit.x);
			}
			else {
				mult.x = 1.0f;
			}
		}
		else {
			mult.x = 0;
		}
		if (limit.y != 0) {
			if (curr.y * limit.y >= 0) {
				mult.y = max(abs(limit.y) - abs(curr.y), 0) / abs(limit.y);
			}
			else {
				mult.y = 1.0f;
			}
		}
		else {
			mult.y = 0;
		}
		if (limit.z != 0) {
			if (curr.z * limit.z >= 0) {
				mult.z = max(abs(limit.z) - abs(curr.z), 0) / abs(limit.z);
			}
			else {
				mult.z = 1.0f;
			}
		}
		else {
			mult.z = 0;
		}
		//_MESSAGE("currlocal\t\t%f %f %f\nlimit\t\t\t%f %f %f\nmult\t\t\t%f %f %f", currLocal.x, currLocal.y, currLocal.z, limit.x, limit.y, limit.z, mult.x, mult.y, mult.z);
		return mult;
	}
	return hkVector4(1.0f, 1.0f, 1.0f);
}

PhysData* PhysicsManager::GetData(Actor* a) {
	if (!datamap.count((UInt64)a))
		return NULL;
	if (!a->loadedState || a->IsDead(1)) {
		datamap.erase((UInt64)a);
		return NULL;
	}
	return &datamap[(UInt64)a];
}

void PhysicsManager::AddVelocity(Actor* a, hkVector4 vel) {
	PhysData* pd = GetData(a);
	if (!pd) return;
	pd->velocity += vel;
}

void PhysicsManager::SetFriction(Actor* a, float f) {
	PhysData* pd = GetData(a);
	if (!pd) return;
	pd->friction = f;
}

void PhysicsManager::SetDrag(Actor* a, float f) {
	PhysData* pd = GetData(a);
	if (!pd) return;
	pd->airdrag = f;
}*/


/*
* ground normal = bhkCharacterController + 0x1B0
* fall time = bhkCharacterController + 0x244
* ground entity = bhkCharacterController + 0x2B0
* get/set velocity = bhkCharacterController virtual + 0x30 0x38
* _GetVelocity GetVelocity = *(_GetVelocity*)(*(UInt64*)controller + 0x30);
* _SetVelocity SetVelocity = *(_SetVelocity*)(*(UInt64*)controller + 0x38);
*
* UInt32 flag = *(UInt32*)((UInt64)controller + 0x218);
*
* bool onGround = (flag & 0x100) == 0x100;
* bool inWater = (flag & 0xA00000) == 0xA00000;
* 
* See SkyrimSE.exe+0xE1E270. bhkCharacterStateSwimming makes player movable
*/

float PhysicsManager::ModifyVelocity(Actor* a, hkVector4 v, bool modifyState) {
	if (!a->processManager || !a->processManager->middleProcess || a->IsDead(1))
		return 0;
	bhkCharacterController* con = *(bhkCharacterController**)((UInt64)a->processManager->middleProcess + 0x250);
	if (!con)
		return 0;
	_SetVelocity SetVelocity = *(_SetVelocity*)(*(UInt64*)con + 0x38);
	SetVelocity(con, v);
	float deltaTime = *(float*)((UInt64)con + 0x88);
	*(uint64_t*)((uint64_t)con + 0x218) = *(uint64_t*)((uint64_t)con + 0x218) & ~((uint64_t)0xFF00) | (uint64_t)0x8700; //Jetpack flag?
	*(float*)((uint64_t)con + 0x220) = deltaTime;

	int32_t& state = *(int32_t*)((UInt64)con + 0x200);
	if (modifyState) {
		state = 5;
	}
	else {
		if (state == 0) {
			a->pos += NiPoint3(0, 0, 40);
		}
	}
	return deltaTime;
}

void PhysicsManager::SetVelocity(Actor* a, hkVector4 from, hkVector4 to, float dur) {
	if (!a || !a->loadedState || a->IsDead(1))
		return;

	auto it = velMap.find(a);
	if (it != velMap.end()) {
		//logger::warn(_MESSAGE("Actor found on the map. Inserting queue"));
		VelocityData data = it->second;
		data.velocity = from;
		data.duration = dur;
		data.step = (to - from) / (dur / tick);
		queueMap.insert(std::pair<Actor*, VelocityData>(a, data));
	}
	else {
		//logger::warn(_MESSAGE("Actor not found on the map. Creating data"));
		VelocityData data = VelocityData();
		data.velocity = from;
		data.duration = dur;
		data.step = (to - from) / (dur / tick);
		velMap.insert(std::pair<Actor*, VelocityData>(a, data));
	}
}

bool PhysicsManager::Simulate(Actor* a) {
	if (a->loadedState && !a->IsDead(1) && (a->actorState.flags04 >> 14 & 15) == 0) {
		auto result = velMap.find(a);
		if (result != velMap.end()) {
			VelocityData& data = result->second;
			auto qresult = queueMap.find(a);
			if (qresult != queueMap.end()) {
				VelocityData& queueData = qresult->second;
				data.velocity = queueData.velocity;
				data.step = queueData.step;
				data.duration = queueData.duration;
				queueMap.erase(a);
			}
			if (data.duration < 0) {
				velMap.erase(result);
			}
			else {
				float deltaTime = ModifyVelocity(a, data.velocity, true);
				data.velocity += data.step * deltaTime / tick;
				data.duration -= deltaTime;
			}
		}
	}
	else {
		velMap.erase(a);
		queueMap.erase(a);
		return false;
	}
	return true;
}

void PhysicsManager::Reset() {
	velMap.clear();
	queueMap.clear();
}

bool PhysicsManager::IsOnGround(Actor* a) {
	if (!a || !a->processManager || !a->processManager->middleProcess)
		return false;
	bhkCharacterController* con = *(bhkCharacterController**)((UInt64)a->processManager->middleProcess + 0x250);
	if (!con)
		return false;
	return (*(UInt32*)((UInt64)con + 0x218) & 0x100) == 0x100;
}

hkVector4 operator*(float a, hkVector4& v) {
	return v * a;
}
