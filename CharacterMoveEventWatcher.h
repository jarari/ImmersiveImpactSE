#pragma once
#pragma once
#include <string>
#include <unordered_map>
#include <skse64\GameEvents.h>
using std::unordered_map;

class bhkCharacterController;

struct bhkCharacterMoveFinishEvent {
	float x;		//00
	float y;		//04
	float z;		//08
	float unk0C;	//0C
	UInt64 unk10;	//10
	void* returnAddr;	//18
	float unk20;	//20
};

class CharacterMoveEventWatcher : public BSTEventSink<bhkCharacterMoveFinishEvent> {
public:
	typedef EventResult (CharacterMoveEventWatcher::* FnReceiveEvent)(bhkCharacterMoveFinishEvent* evn, EventDispatcher<bhkCharacterMoveFinishEvent>* dispatcher);
	CharacterMoveEventWatcher() {
		_MESSAGE((className + std::string(" instance created.")).c_str());
	}

	EventResult ReceiveEventHook(bhkCharacterMoveFinishEvent* evn, EventDispatcher<bhkCharacterMoveFinishEvent>* src);

	void HookSink();

protected:
	static std::string className;
	static unordered_map<UInt64, FnReceiveEvent> fnHash;
};