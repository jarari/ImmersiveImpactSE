#include "AddressManager.h"
#include "Utils.h"
#include "WeaponSpeedManager.h"
#include <skse64\GameReferences.h>
#include <skse64\GameRTTI.h>
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

NiMatrix33 Utils::GetRotationMatrix33(float pitch, float yaw, float roll) {
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
	return m_yaw * m_pitch * m_roll;
}

NiMatrix33 Utils::GetRotationMatrix33(NiPoint3 axis, float angle) {
	float x = axis.x * sin(angle / 2.0f);
	float y = axis.y * sin(angle / 2.0f);
	float z = axis.z * sin(angle / 2.0f);
	float w = cos(angle / 2.0f);
	Quaternion q = Quaternion(x, y, z, w);
	return q.ToRotationMatrix33();
}

//Sarrus rule
float Utils::Determinant(NiMatrix33 mat) {
	return mat.data[0][0] * mat.data[1][1] * mat.data[2][2]
		+ mat.data[0][1] * mat.data[1][2] * mat.data[2][0]
		+ mat.data[0][2] * mat.data[1][0] * mat.data[2][1]
		- mat.data[0][2] * mat.data[1][1] * mat.data[2][0]
		- mat.data[0][1] * mat.data[1][0] * mat.data[2][2]
		- mat.data[0][0] * mat.data[1][2] * mat.data[2][1];
}

NiMatrix33 Utils::Inverse(NiMatrix33 mat) {
	float det = Determinant(mat);
	if (det == 0) {
		NiMatrix33 idmat;
		idmat.Identity();
		return idmat;
	}
	float a = mat.data[0][0];
	float b = mat.data[0][1];
	float c = mat.data[0][2];
	float d = mat.data[1][0];
	float e = mat.data[1][1];
	float f = mat.data[1][2];
	float g = mat.data[2][0];
	float h = mat.data[2][1];
	float i = mat.data[2][2];
	NiMatrix33 invmat;
	SetMatrix33(e * i - f * h, -(b * i - c * h), b * f - c * e,
				-(d * i - f * g), a * i - c * g, -(a * f - c * d),
				d * h - e * g, -(a * h - b * g), a * e - b * d,
				invmat);
	return invmat * (1.0f / det);
}

//(Rotation Matrix)^-1 * (World pos - Local Origin)
NiPoint3 Utils::WorldToLocal(NiPoint3 wpos, NiPoint3 lorigin, NiMatrix33 rot) {
	NiPoint3 lpos = wpos - lorigin;
	NiMatrix33 invrot = Inverse(rot);
	return invrot * lpos;
}

NiPoint3 Utils::LocalToWorld(NiPoint3 lpos, NiPoint3 lorigin, NiMatrix33 rot) {
	return rot * lpos + lorigin;
}

void Utils::GetRefForward(float pitch, float yaw, float roll, NiPoint3* vec) {
	NiPoint3 fwd = GetRotationMatrix33(pitch, yaw, roll) * NiPoint3(0, 1, 0);
	vec->x = fwd.x;
	vec->y = fwd.y;
	vec->z = fwd.z;
}

Quaternion::Quaternion(float _x, float _y, float _z, float _w) {
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

float Quaternion::Norm() {
	return x * x + y * y + z * z + w * w;
}

//From https://android.googlesource.com/platform/external/jmonkeyengine/+/59b2e6871c65f58fdad78cd7229c292f6a177578/engine/src/core/com/jme3/math/Quaternion.java
NiMatrix33 Quaternion::ToRotationMatrix33() {
	float norm = Norm();
	// we explicitly test norm against one here, saving a division
	// at the cost of a test and branch.  Is it worth it?
	float s = (norm == 1.0f) ? 2.0f : (norm > 0.0f) ? 2.0f / norm : 0;

	// compute xs/ys/zs first to save 6 multiplications, since xs/ys/zs
	// will be used 2-4 times each.
	float xs = x * s;
	float ys = y * s;
	float zs = z * s;
	float xx = x * xs;
	float xy = x * ys;
	float xz = x * zs;
	float xw = w * xs;
	float yy = y * ys;
	float yz = y * zs;
	float yw = w * ys;
	float zz = z * zs;
	float zw = w * zs;

	// using s=2/norm (instead of 1/norm) saves 9 multiplications by 2 here
	NiMatrix33 mat;
	Utils::SetMatrix33(1 - (yy + zz), (xy - zw), (xz + yw),
		(xy + zw), 1 - (xx + zz), (yz - xw),
					   (xz - yw), (yz + xw), 1 - (xx + yy),
					   mat);

	return mat;
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

int Utils::GetWeaponType(TESObjectWEAP* wep) {
	if(!wep)
		return iWepType::Fist;
	int weptype = wep->type();
	if (weptype == 5 || weptype == 6) {
		return iWepType::TwoH;
	}
	else if (weptype == 1 || weptype == 3 || weptype == 4) {
		return iWepType::OneH;
	}
	else if (weptype == 2) {
		return iWepType::Dagger;
	}
	else if (weptype == 0) {
		return iWepType::Fist;
	}
	return iWepType::None;
}

const char* Utils::GetName(TESForm* form) {
	TESFullName* pFullName = DYNAMIC_CAST(form, TESForm, TESFullName);
	const char* name = "Unknown";
	if (pFullName)
		name = pFullName->name.data;
	return name;
}
