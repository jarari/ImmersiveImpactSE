#include "AddressManager.h"
#include "PhysicsManager.h"
#include <skse64_common\BranchTrampoline.h>
#include <skse64_common\SafeWrite.h>
#include <xbyak\xbyak.h>

float PhysicsManager::defaultFriction = 0.9f;
float PhysicsManager::defaultDrag = 1.0f;

PhysData* PhysicsManager::ShouldOverrideVelocity(bhkCharacterController* cCon) {
	Actor* a = (Actor*)(**(UInt64 **)((UInt64)cCon + 0x10) - 0xD0);
	if (datamap.count((UInt64)a)) {
		PhysData* pd = GetData(a);
		return pd;
	}
	return NULL;
}

void PhysicsManager::HookOnGroundVelocity() {
	/*struct InstallHookOnGroundVelocity_Code : Xbyak::CodeGenerator {
		InstallHookOnGroundVelocity_Code(void* buf, uintptr_t shouldOverride) : Xbyak::CodeGenerator(4096, buf) {
			Xbyak::Label retn;

			push(rax);
			push(rcx);
			push(rdx);
			push(r8);
			push(r9);
			push(r10);
			push(r11);
			lahf();
			sub(rsp, 8);	//align stack
			lea(rsp, ptr[rsp - 0x100]);
			mov(word[rsp + 0x90], ah);
			movaps(qword[rsp + 0x40], xmm0);
			mov(rcx, rsi);
			mov(rax, shouldOverride);
			call(rax);
			test(rax, rax);
			mov(dword[rbp - 0x60], 0x3f800000);
			je(retn);
			mov(ecx, ptr[rax]);
			mov(dword[rbp - 0x60], ecx);

			L(retn);
			movaps(xmm0, qword[rsp + 0x40]);
			mov(ah, word[rsp + 0x90]);
			lea(rsp, ptr[rsp + 0x100]);
			add(rsp, 8);
			sahf();
			pop(r11);
			pop(r10);
			pop(r9);
			pop(r8);
			pop(rdx);
			pop(rcx);
			pop(rax);
			jmp(ptr[rip]);
			dq(ptr_VelocityInjectionPoint + 0x7);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	InstallHookOnGroundVelocity_Code code(codeBuf, GetFnAddr(PhysicsManager::ShouldOverrideVelocity));
	g_localTrampoline.EndAlloc(code.getCurr());

	if (!g_branchTrampoline.Write5Branch(ptr_VelocityInjectionPoint, uintptr_t(code.getCode())))
		return;
	SafeWrite8(ptr_VelocityInjectionPoint + 5, 0x90);
	SafeWrite8(ptr_VelocityInjectionPoint + 6, 0x90);*/

	SafeWrite8(ptr_FrictionOverridePoint, 0x90);
	SafeWrite8(ptr_FrictionOverridePoint + 1, 0x90);
	SafeWrite8(ptr_FrictionOverridePoint + 2, 0x90);

	SafeWrite8(ptr_OnGroundVelocityOverridePoint, 0x90);
	SafeWrite8(ptr_OnGroundVelocityOverridePoint + 1, 0xE9);
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
		SetVelocity(controller, (vel + friction + drag) * dt_t);

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
