#pragma once
#include <skse64\GameCamera.h>
#include <skse64\GameEvents.h>
#include <skse64\GameTypes.h>
template <typename EventT, typename EventArgT = EventT>
class EventDispatcherEx {
public:
	typedef BSTEventSink<EventT> SinkT;
	tArray<SinkT*>		eventSinks;			// 000
	tArray<SinkT*>		addBuffer;			// 018 - schedule for add
	tArray<SinkT*>		removeBuffer;		// 030 - schedule for remove
	SimpleLock			lock;				// 048
	bool				stateFlag;			// 050 - some internal state changed while sending
	char				pad[7];				// 051

	EventDispatcherEx() : stateFlag(false) {}
};

struct TESEquipEvent {
	UInt64	unk_00; //Actor
	UInt32	unk_01; //Form ID
	UInt32	unk_02;
	UInt32	unk_03; //Equipping? or unequipping?
};
class EventDispatcherListEx {
public:
	EventDispatcher<void>								unk00;					//	00
	EventDispatcher<void>								unk58;					//  58  - sink offset 010
	EventDispatcher<TESActiveEffectApplyRemoveEvent>	unkB0;					//  B0  - sink offset 018
	EventDispatcher<void>								unk108;					//  108 - sink offset 020
	EventDispatcher<void>								unk160;					//  160 - sink offset 028
	EventDispatcher<TESCellAttachDetachEvent>			unk1B8;					//  1B8 - sink offset 030
	EventDispatcher<void>								unk210;					//  210 - sink offset 038
	EventDispatcher<void>								unk2C0;					//  2C0 - sink offset 040
	EventDispatcher<TESCombatEvent>						combatDispatcher;		//  318 - sink offset 048
	EventDispatcher<TESContainerChangedEvent>			unk370;					//  370 - sink offset 050
	EventDispatcher<TESDeathEvent>						deathDispatcher;		//  3C8 - sink offset 058
	EventDispatcher<void>								unk420;					//  420 - sink offset 068
	EventDispatcher<void>								unk478;					//  478 - sink offset 070
	EventDispatcher<TESEquipEvent>								equipDispatcher;					//  4D0 - sink offset 078
	EventDispatcher<void>								unk528;					//  528 - sink offset 080
	EventDispatcher<void>								unk580;					//  580 - sink offset 088
	EventDispatcher<void>								unk5D8;					//  5D8 - sink offset 090
	EventDispatcher<TESHitEvent>								hitEventDispatcher;					//  630 - sink offset 098
	EventDispatcher<TESInitScriptEvent>					initScriptDispatcher;	//  688 - sink offset 0A0
	EventDispatcher<void>								unk6E0;					//  6E0 - sink offset 0A8
	EventDispatcher<void>								unk738;					//  738 - sink offset 0B0
	EventDispatcher<void>								unk790;					//  790 - sink offset 0B8
	EventDispatcher<void>								unk7E8;					//  7E8 - sink offset 0C0
	EventDispatcher<void>								unk840;					//  840 - sink offset 0C8
	EventDispatcher<TESObjectLoadedEvent>				objectLoadedDispatcher;	//  898 - sink offset 0D0
	EventDispatcher<void>								unk8F0;					//  8F0 - sink offset 0D8
	EventDispatcher<void>								unk948;					//  948 - sink offset 0E0
	EventDispatcher<void>								unk9A0;					//  9A0 - sink offset 0E8
	EventDispatcher<void>								unk9F8;					//  9F8 - sink offset 0F0
	EventDispatcher<void>								unkA50;					//  A50 - sink offset 0F8
	EventDispatcher<void>								unkAA8;					//  AA8 - sink offset 100
	EventDispatcher<void>								unkB00;					//  B00 - sink offset 108
	EventDispatcher<void>								unkB58;					//  B58 - sink offset 110
	EventDispatcher<void>								unkBB0;					//  BB0 - sink offset 118
	EventDispatcher<void>								unkC08;					//  C08 - sink offset 120
	EventDispatcher<void>								unkC60;					//  C60 - sink offset 128
	EventDispatcher<void>								unkCB8;					//  CB8 - sink offset 130
	EventDispatcher<void>								unkD10;					//  D10 - sink offset 138
	EventDispatcher<void>								unkD68;					//  D68 - sink offset 140
	EventDispatcher<void>								unkDC0;					//  DC0 - sink offset 148
	EventDispatcher<void>								unkE18;					//  E18 - sink offset 150
	EventDispatcher<void>								unkE70;					//  E70 - sink offset 158
	EventDispatcher<void>								unkEC8;					//  EC8 - sink offset 160
	EventDispatcher<void>								unkF20;					//  F20 - sink offset 168
	EventDispatcher<void>								unkF78;					//  F78 - sink offset 170
	EventDispatcher<void>								unkFD0;					//  FD0 - sink offset 178
	EventDispatcher<void>								unk1028;				// 1028 - sink offset 180
	EventDispatcher<void>								unk1080;				// 1080 - sink offset 188
	EventDispatcher<void>								unk10D8;				// 10D8 - sink offset 190
	EventDispatcher<void>								unk1130;				// 1130 - sink offset 198
	EventDispatcher<void>								unk1188;				// 1188 - sink offset 200
	EventDispatcher<void>								unk11E0;				// 11E0 - sink offset 208
	EventDispatcher<void>								unk1238;				// 1238 - sink offset 210
	EventDispatcher<TESUniqueIDChangeEvent>				uniqueIdChangeDispatcher;	// 1290 - sink offset 218
};

class IAnimationGraphManagerHolderEx {
public:
	virtual ~IAnimationGraphManagerHolderEx();

	virtual bool			SendAnimationEvent(const BSFixedString& eventName);
	virtual UInt32			Unk_02(void);
	virtual UInt32			Unk_03(void);
	virtual UInt32			Unk_04(void);
	virtual UInt32			Unk_05(void);
	virtual UInt32			Unk_06(void);
	virtual UInt32			Unk_07(void);
	virtual UInt32			Unk_08(void);
	virtual UInt32			Unk_09(void);
	virtual UInt32			Unk_0A(void);
	virtual UInt32			Unk_0B(void);
	virtual UInt32			Unk_0C(void);
	virtual UInt32			Unk_0D(void);
	virtual UInt32			Unk_0E(void);
	virtual UInt32			Unk_0F(void);
	virtual bool	GetAnimationVariableFloat(const BSFixedString& variableName, float& out);
	virtual bool	GetAnimationVariableInt(const BSFixedString& variableName, SInt32& out);
	virtual bool	GetAnimationVariableBool(const BSFixedString& variableName, bool& out);

	//	void	** _vtbl;
};

class TESCameraEx {
public:
	TESCameraEx();
	virtual ~TESCameraEx();

	virtual void SetNode(NiNode* node);
	virtual void Update();

	float		rotZ;	// 08
	float		rotX;	// 0C
	NiPoint3	pos;	// 10
	float		zoom;	// 1C
	NiNode* cameraNode;	// 20 - First child is usually NiCamera
	TESCameraState* cameraState;	// 28
	UInt8		unk30;			// 30
	UInt8		pad31[7];		// 31
};

class PlayerCameraEx : public TESCameraEx {
public:
	PlayerCameraEx();
	virtual ~PlayerCameraEx();

	enum {
		kCameraState_FirstPerson = 0,
		kCameraState_AutoVanity,
		kCameraState_VATS,
		kCameraState_Free,
		kCameraState_IronSights,
		kCameraState_Furniture,
		kCameraState_Transition,
		kCameraState_TweenMenu,
		kCameraState_ThirdPerson1,
		kCameraState_ThirdPerson2,
		kCameraState_Horse,
		kCameraState_Bleedout,
		kCameraState_Dragon,
		kNumCameraStates
	};

	UInt8	unk38[0xB8 - 0x38];							// 028
	TESCameraState* cameraStates[kNumCameraStates];	// 0B8
	UInt64	unk120;										// 120
	UInt64	unk128;										// 128
	UInt64	unk130;										// 130
	UInt32	unk138;										// 138
	float	worldFOV;									// 13C
	float	firstPersonFOV;								// 140
	float	posX;										// 144
	float	posY;										// 148
	float	posZ;										// 14C
	UInt8	unk150[0x160 - 0x150];						// 150
	UInt8	unk160;										// 160
	UInt8	unk161;										// 161
	UInt8	unk162;										// 162 - init'd to 1
	UInt8	unk163;										// 163
	UInt8	unk164;										// 164
	UInt8	unk165;										// 165
	UInt8	pad166[2];									// 166

	MEMBER_FN_PREFIX(PlayerCamera);
	DEFINE_MEMBER_FN(UpdateThirdPerson, void, 0x0084D630, bool weaponDrawn);
};