#pragma once
#include "MenuChecker.h"

#include <fstream>
#include "skse64/GameVR.h"
#include "skse64_common/BranchTrampoline.h"
#include "api/VRHookAPI.h"
#include "skse64/InternalTasks.h"

#include <thread>
#include <atomic>
#include "api/utils/OpenVRUtils.h"

#include "Helper.h"
#include <skse64/PapyrusEvents.h>

namespace PowerAttackVR
{
	const float TOLERANCE= 0.00001f;

	struct HandPosStamina
	{
		NiPoint3 pos;
		NiMatrix33 rot;
		float stamina;
		double time;
	};

	enum AttackDirection
	{
		AttackStanding = 0,
		AttackForward = 1,
		AttackRight = 2,
		AttackBackward = 3,
		AttackLeft = 4,
	};

	enum Orientation
	{
		VerticalDown = 0,
		HorizontalRight = 2,
		VerticalUp = 3,
		HorizontalLeft = 4
	};

	enum SpinningType
	{
		Left,
		Downwards,
		Right,
		Upwards
	};

	extern OpenVRHookManagerAPI* hookManager;
	
	void StartMod();
	void AfterGameLoad();
	void InitializePointers();

	void HitEventFunc(bool secondaryHand, Actor * targetActor, TESObjectREFR* targetRefr);

	void Hooks();

	class SKSEActionEventHandler : public BSTEventSink <SKSEActionEvent>
	{
	public:
		virtual EventResult ReceiveEvent(SKSEActionEvent* evn, EventDispatcher<SKSEActionEvent>* dispatcher);
	};

	extern SKSEActionEventHandler g_SKSEActionEventHandler;

	class HitEventHandler : public BSTEventSink <TESHitEvent>
	{
	public:
		virtual	EventResult ReceiveEvent(TESHitEvent * evn, EventDispatcher<TESHitEvent> * dispatcher);
	};

	extern EventDispatcher<TESHitEvent>* g_HitEventDispatcher;
	extern HitEventHandler g_HitEventHandler;

	typedef void(*OriginalAttackBlockHandler)(TESObjectREFR* ref, NiPoint3* velocity);

	extern TESGlobal* powerAttackControlValueGlobal;
}
