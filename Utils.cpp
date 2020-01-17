#include "AddressManager.h"
#include "Utils.h"
#include <skse64\GameReferences.h>
#include <skse64\PapyrusVM.h>
#include <sstream>
#include <iomanip>
#include <typeinfo>
#include <vector>
using namespace std;

void Utils::Dump(void* mem, unsigned int size) {
	char* p = static_cast<char*>(mem);
	unsigned char* up = (unsigned char*)p;
	std::stringstream stream;
	int row = 0;
	for (unsigned int i = 0; i < size; i++) {
		stream << std::setfill('0') << std::setw(2) << std::hex << (int)up[i] << " ";
		if (i % 8 == 7) {
			stream << "\t0x"
				<< std::setw(2) << std::hex << (int)up[i]
				<< std::setw(2) << (int)up[i - 1]
				<< std::setw(2) << (int)up[i - 2]
				<< std::setw(2) << (int)up[i - 3]
				<< std::setw(2) << (int)up[i - 4]
				<< std::setw(2) << (int)up[i - 5]
				<< std::setw(2) << (int)up[i - 6]
				<< std::setw(2) << (int)up[i - 7] << std::setfill('0');
			stream << "\t0x" << std::setw(2) << std::hex << row * 8 << std::setfill('0');
			_MESSAGE("%s", stream.str().c_str());
			stream.str(std::string());
			row++;
		}
	}
}

ActiveEffect* Utils::GetActiveEffectFromActor(Actor* actor, const char* name) {
	tList<ActiveEffect>* list_ae = actor->magicTarget.GetActiveEffects();
	tList<ActiveEffect>::Iterator it = list_ae->Begin();
	if (list_ae->Count() == 0)
		return nullptr;
	while (!it.End()) {
		ActiveEffect* ae = it.Get();
		const char* fullname = ae->effect->mgef->fullName.name.data;
		if (fullname != nullptr && strlen(fullname) > 0 && strcmp(fullname, name) == 0) {
			return ae;
		}
		++it;
	}
	return nullptr;
}

void Utils::SetMatrix33(float a, float b, float c, float d, float e, float f, float g, float h, float i, NiMatrix33& mat) {
	mat.data[0][0] = a;
	mat.data[0][1] = b;
	mat.data[0][2] = c;
	mat.data[1][0] = d;
	mat.data[1][1] = e;
	mat.data[1][2] = f;
	mat.data[2][0] = g;
	mat.data[2][1] = h;
	mat.data[2][2] = i;
}

void Utils::GetRefForward(float pitch, float yaw, float roll, NiPoint3* vec) {
	NiMatrix33 m_yaw;
	SetMatrix33(cos(yaw), -sin(yaw), 0,
				sin(yaw), cos(yaw), 0,
				0, 0, 1,
				m_yaw);
	NiMatrix33 m_roll;
	SetMatrix33(1, 0, 0,
				0, cos(roll), -sin(roll),
				0, sin(roll), cos(roll),
				m_roll);
	NiMatrix33 m_pitch;
	SetMatrix33(cos(pitch), 0, sin(pitch),
				0, 1, 0,
				-sin(pitch), 0, cos(pitch),
				m_pitch);
	NiPoint3 fwd = m_yaw * m_pitch * m_roll * NiPoint3(0, 1, 0);
	vec->x = fwd.x;
	vec->y = fwd.y;
	vec->z = fwd.z;
}

void Utils::SendNotification(const char* str) {
	VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();
	if (!registry) return;
	SendNotification_Fn(str);
}

UInt32 Utils::SoundPlay(TESSound* sound, TESObjectREFR* obj) {
	VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();
	if (!registry) return -1;
	return Play_Native(registry, 0, sound, obj);
}

void Utils::SetInstanceVolume(UInt32 instance, float volume) {
	VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();
	if (!registry) return;
	SetInstanceVolume_Native(registry, 0, instance, volume);
}
