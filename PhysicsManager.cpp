#include "AddressManager.h"
#include "PhysicsManager.h"
#include "Utils.h"
#include <skse64_common\BranchTrampoline.h>
#include <skse64_common\SafeWrite.h>
#include <xbyak\xbyak.h>

int PhysicsManager::tick = 16667;
float PhysicsManager::defaultFriction = 0.75f;
float PhysicsManager::defaultDrag = 1.0f;
typedef bool (*_GetVelocity)(bhkCharacterController* con, const hkVector4& vel);
typedef bool (*_SetVelocity)(bhkCharacterController* con, const hkVector4& vel);

hkVector4 PhysicsManager::GetAccelerationMultiplier(bhkCharacterController* cCon, hkVector4 limit) {
	Actor* a = (Actor*)(**(UInt64 **)((UInt64)cCon + 0x10) - 0xD0);
	_GetVelocity GetVelocity = *(_GetVelocity*)(*(UInt64*)cCon + 0x30);
	if (datamap.count((UInt64)a)) {
		PhysData* pd = GetData(a);
		if (!pd)
			return hkVector4(1, 1, 1);
		hkVector4 curr;
		GetVelocity(cCon, curr);
		hkVector4 currLocal = Utils::WorldToLocal(curr, NiPoint3(), Utils::GetRotationMatrix33(0, -a->rot.z, 0));
		hkVector4 mult = hkVector4(1, 1, 1);
		if (limit.x != 0) {
			if (currLocal.x * limit.x >= 0) { //same sign
				mult.x = max(abs(limit.x) - abs(currLocal.x), 0) / abs(limit.x);
			}
			else {
				mult.x = 1.0f;
			}
		}
		else {
			mult.x = 0;
		}
		if (limit.y != 0) {
			if (currLocal.y * limit.y >= 0) {
				mult.y = max(abs(limit.y) - abs(currLocal.y), 0) / abs(limit.y);
			}
			else {
				mult.y = 1.0f;
			}
		}
		else {
			mult.y = 0;
		}
		if (limit.z != 0) {
			if (currLocal.z * limit.z >= 0) {
				mult.z = max(abs(limit.z) - abs(currLocal.z), 0) / abs(limit.z);
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
	return hkVector4(1, 1, 1);
}

void PhysicsManager::HookSkyrimPhys() {
	_MESSAGE("Dash or knockback enabled. Replacing Skyrim physics."); struct InstallHookAcceleration_Code : Xbyak::CodeGenerator {
		InstallHookAcceleration_Code(void* buf, uintptr_t getAccelMul) : Xbyak::CodeGenerator(4096, buf) {
			mulss(xmm7, xmm0);	//x
			mulss(xmm8, xmm0);	//y
			mulss(xmm6, xmm0);	//z

			push(rax);
			push(rcx);
			push(rdx);
			push(r8);
			push(r9);
			push(r10);
			push(r11);
			lahf();
			lea(rsp, ptr[rsp - 0x208]);
			mov(ptr[rsp + 0x150], ah);
			movaps(ptr[rsp + 0x160], xmm1);
			movaps(ptr[rsp + 0x170], xmm2);
			movaps(ptr[rsp + 0x180], xmm3);
			movaps(ptr[rsp + 0x190], xmm5);
			movss(ptr[rsp + 0x140], xmm7);
			movss(ptr[rsp + 0x144], xmm8);
			movss(ptr[rsp + 0x148], xmm6);
			lea(r8, ptr[rsp + 0x140]);
			mov(rdx, rsi);
			mov(rax, getAccelMul);
			call(rax);
			mulss(xmm7, dword[rax]);
			mulss(xmm8, dword[rax + 0x4]);
			mulss(xmm6, dword[rax + 0x8]);
			movaps(xmm1, ptr[rsp + 0x160]);
			movaps(xmm2, ptr[rsp + 0x170]);
			movaps(xmm3, ptr[rsp + 0x180]);
			movaps(xmm5, ptr[rsp + 0x190]);
			mov(ah, ptr[rsp + 0x150]);
			lea(rsp, ptr[rsp + 0x208]);
			sahf();
			pop(r11);
			pop(r10);
			pop(r9);
			pop(r8);
			pop(rdx);
			pop(rcx);
			pop(rax);

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

	SafeWrite8(ptr_FrictionOverridePoint, 0x90);
	SafeWrite8(ptr_FrictionOverridePoint + 1, 0x90);
	SafeWrite8(ptr_FrictionOverridePoint + 2, 0x90);

	SafeWrite8(ptr_OnGroundVelocityOverridePoint, 0x90);
	SafeWrite8(ptr_OnGroundVelocityOverridePoint + 1, 0xE9);

	physHooked = true;
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
}

//ground normal = bhkCharacterController + 0x1B0
//fall time = bhkCharacterController + 0x244
//ground entity = bhkCharacterController + 0x2B0
//get/set velocity = bhkCharacterController virtual + 0x30 0x38
bool PhysicsManager::Simulate(Actor* a) {
	PhysData* pd = GetData(a);
	if (!pd) return false;
	float dt = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - pd->lastRun).count();
	if (dt >= tick) {
		bhkCharacterController* controller = *(bhkCharacterController * *)((UInt64)a->processManager->middleProcess + 0x250);
		if (!controller) {
			datamap.erase((UInt64)a);
			return false;
		}
		_GetVelocity GetVelocity = *(_GetVelocity*)(*(UInt64*)controller + 0x30);
		_SetVelocity SetVelocity = *(_SetVelocity*)(*(UInt64*)controller + 0x38);

		UInt32 flag = *(UInt32*)((UInt64)controller + 0x218);

		bool onGround = (flag & 0x100) == 0x100;
		bool inWater = (flag & 0xA00000) == 0xA00000;

		hkVector4 vel;
		GetVelocity(controller, vel);

		float dt_t = *(float*)ptr_EngineTick * 1000000.0f / 16666.6667f;
		vel += pd->velocity;
		pd->velocity = hkVector4();
		float len = vel.Length();
		hkVector4 friction = -pd->currentVelocity * onGround * pd->friction * dt_t;
		hkVector4 drag = -pd->currentVelocity;
		drag.Normalize();
		float density = inWater ? 997.0f : 1.225f;
		float c = 0.005f * *(float*)ptr_EngineTick * density * len * len * pd->airdrag;
		drag *= c;
		if(abs(drag.x) > abs(pd->currentVelocity.x))
			drag.x = -pd->currentVelocity.x;
		if (abs(drag.y) > abs(pd->currentVelocity.y))
			drag.y = -pd->currentVelocity.y;
		if (abs(drag.z) > abs(pd->currentVelocity.z))
			drag.z = -pd->currentVelocity.z;

		pd->currentVelocity = vel + friction + drag;
		SetVelocity(controller, pd->currentVelocity);

		pd->lastRun = std::chrono::system_clock::now();
	}
	return true;
}

void PhysicsManager::InitializeData(Actor* a) {
	data_Lock.Enter();
	datamap.insert(std::make_pair((UInt64)a, PhysData(PhysicsManager::defaultFriction, PhysicsManager::defaultDrag)));
	data_Lock.Leave();
}

void PhysicsManager::ResetPhysics() {
	data_Lock.Enter();
	datamap.clear();
	data_Lock.Leave();
}

void PhysicsManager::ResetTimer() {
	for (unordered_map<UInt64, PhysData>::iterator it = datamap.begin(); it != datamap.end(); it++) {
		it->second.lastRun = std::chrono::system_clock::now();
	}
}
