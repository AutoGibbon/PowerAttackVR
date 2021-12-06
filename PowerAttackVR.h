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

	struct HandPosition
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

	extern TESGlobal* powerAttackControlValueGlobal;
}
