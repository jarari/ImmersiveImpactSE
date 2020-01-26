#include "AddressManager.h"
#include "PhysicsManager.h"
#include "Utils.h"
#include <skse64_common\BranchTrampoline.h>
#include <skse64_common\SafeWrite.h>
#include <xbyak\xbyak.h>

float PhysicsManager::defaultFriction = 0.9f;
float PhysicsManager::defaultDrag = 1.0f;

hkVector4 PhysicsManager::GetAccelerationMultiplier(bhkCharacterController* cCon, hkVector4 limit) {
	Actor* a = (Actor*)(**(UInt64 **)((UInt64)cCon + 0x10) - 0xD0);
	if (datamap.count((UInt64)a)) {
		PhysData* pd = GetData(a);
		hkVector4 currLocal = Utils::WorldToLocal(pd->currentVelocity, NiPoint3(), Utils::GetRotationMatrix33(-a->rot.x, -a->rot.z, 0));
		hkVector4 mult = hkVector4(1, 1, 1);
		if (limit.x != 0) {
			if (currLocal.x * limit.x >= 0) { //same sign
				mult.x = max(abs(limit.x) - abs(currLocal.x), 0) / abs(limit.x);
			}
			else {
				mult.x = 1.0f;
			}
		}
		if (limit.y != 0) {
			if (currLocal.y * limit.y >= 0) {
				mult.y = max(abs(limit.y) - abs(currLocal.y), 0) / abs(limit.y);
			}
			else {
				mult.y = 1.0f;
			}
		}
		if (limit.z != 0) {
			if (currLocal.z * limit.z >= 0) {
				mult.z = max(abs(limit.z) - abs(currLocal.z), 0) / abs(limit.z);
			}
			else {
				mult.z = 1.0f;
			}
		}
		float falltime = *(float*)((UInt64)cCon + 0x244);
		_MESSAGE("Local vel\t\t%f %f %f", currLocal.x, currLocal.y, currLocal.z);
		_MESSAGE("Max vel\t\t\t%f %f %f", limit.x, limit.y, limit.z);
		_MESSAGE("Add\t\t\t\t%f %f %f", mult.x * limit.x, mult.y * limit.y, mult.z * limit.z);
		return mult / (falltime + 1.0f);
	}
	return hkVector4(1, 1, 1);
}

void PhysicsManager::HookOnGroundVelocity() {
	SafeWrite8(ptr_FrictionOverridePoint, 0x90);
	SafeWrite8(ptr_FrictionOverridePoint + 1, 0x90);
	SafeWrite8(ptr_FrictionOverridePoint + 2, 0x90);

	SafeWrite8(ptr_OnGroundVelocityOverridePoint, 0x90);
	SafeWrite8(ptr_OnGroundVelocityOverridePoint + 1, 0xE9);


	struct InstallHookAcceleration_Code : Xbyak::CodeGenerator {
		InstallHookAcceleration_Code(void* buf, uintptr_t getAccelMul) : Xbyak::CodeGenerator(4096, buf) {
			Xbyak::Label retn;
			mulss(xmm7, xmm0);	//x
			mulss(xmm8, xmm0);	//y
			mulss(xmm6, xmm0);	//z

			push(rax);
			push(rcx);
			push(rdx);
			push(r8);
			push(r9);
			lea(rsp, ptr[rsp - 0x78]);
			movaps(ptr[rsp + 0x60], xmm1);
			movaps(ptr[rsp + 0x70], xmm3);
			movss(ptr[rsp + 0x50], xmm7);
			movss(ptr[rsp + 0x54], xmm8);
			movss(ptr[rsp + 0x58], xmm6);
			lea(r8, ptr[rsp + 0x50]);
			mov(rdx, rsi);
			mov(rax, getAccelMul);
			call(rax);
			movaps(xmm1, ptr[rsp + 0x60]);
			movaps(xmm3, ptr[rsp + 0x70]);
			lea(rsp, ptr[rsp + 0x78]);
			pop(r9);
			pop(r8);
			pop(rdx);
			pop(rcx);
			mulss(xmm7, dword[rax]);
			mulss(xmm8, dword[rax + 0x4]);
			mulss(xmm6, dword[rax + 0x8]);
			pop(rax);

			L(retn);
			movss(ptr[rsi + 0xB0], xmm7);
			movss(ptr[rsi + 0xB4], xmm8);
			movss(ptr[rsi + 0xB8], xmm6);
			jmp(ptr[rip]);
			dq(ptr_AccelerationOverridePoint + 0x26);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	InstallHookAcceleration_Code code(codeBuf, GetFnAddr(PhysicsManager::GetAccelerationMultiplier));
	g_localTrampoline.EndAlloc(code.getCurr());

	if (!g_branchTrampoline.Write5Branch(ptr_AccelerationOverridePoint, uintptr_t(code.getCode())))
		return;
}

int PhysData::tick = 30;

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
}

//ground normal = bhkCharacterController + 0x1B0
//fall time = bhkCharacterController + 0x244
//ground entity = bhkCharacterController + 0x2B0
//get/set velocity = bhkCharacterController virtual + 0x30 0x38
bool PhysicsManager::Simulate(Actor* a) {
	PhysData* pd = GetData(a);
	if (!pd) return false;
	float dt = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - pd->lastRun).count();
	if (dt >= pd->tick) {
		bhkCharacterController* controller = *(bhkCharacterController * *)((UInt64)a->processManager->middleProcess + 0x250);
		if (!controller) {
			datamap.erase((UInt64)a);
			return false;
		}
		typedef bool (*_GetVelocity)(bhkCharacterController * con, const hkVector4 & vel);
		_GetVelocity GetVelocity = *(_GetVelocity*)(*(UInt64*)controller + 0x30);
		typedef bool (*_SetVelocity)(bhkCharacterController * con, const hkVector4 & vel);
		_SetVelocity SetVelocity = *(_SetVelocity*)(*(UInt64*)controller + 0x38);

		void* groundEntity = *(void**)((UInt64)controller + 0x2B0);
		hkVector4* groundNormal = (hkVector4*)((UInt64)controller + 0x1B0);
		float falltime = *(float*)((UInt64)controller + 0x244);

		bool onGround = falltime == 0;
		bool inWater = onGround && !groundEntity;

		hkVector4 vel;
		GetVelocity(controller, vel);

		float dt_t = dt / 16666.6667f;
		float len = vel.Length();
		hkVector4 friction = vel * -1.0f * (float)onGround * pd->friction;
		friction.z = 0.0f;
		hkVector4 drag = vel;
		drag.Normalize();
		drag *= hkVector4(1.0f, 1.0f, 0.1f) * -0.5f * len * len * ((float)inWater + 1.0f) * pd->airdrag * dt / 60000000.0f;

		vel += pd->velocity;
		pd->velocity = hkVector4();
		pd->currentVelocity = (vel + friction + drag) * dt_t;
		SetVelocity(controller, pd->currentVelocity);

		pd->lastRun = std::chrono::system_clock::now();
	}
	return true;
}

void PhysicsManager::InitializeData(Actor* a) {
	datamap.insert(std::make_pair((UInt64)a, PhysData(PhysicsManager::defaultFriction, PhysicsManager::defaultDrag)));
}

void PhysicsManager::ResetPhysics() {
	datamap.clear();
}

void PhysicsManager::ResetTimer() {
	for (unordered_map<UInt64, PhysData>::iterator it = datamap.begin(); it != datamap.end(); it++) {
		it->second.lastRun = std::chrono::system_clock::now();
	}
}
