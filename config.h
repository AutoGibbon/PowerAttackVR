#ifndef CONFIG_H
#define CONFIG_H

#pragma once

#include <fstream>
#include <future>
#include "skse64\GameSettings.h"
#include "skse64\GameData.h"

#include <unordered_map>
#include "skse64/PapyrusNativeFunctions.h"
#include "skse64_common/skse_version.h"
#include "Utility.hpp"

namespace PowerAttackVR
{
	extern int enableLog;
	extern int leftHandedMode;
	
	extern int HoldButton;
	extern int HoldButtonSecondary;

	extern bool releaseWithNoTouch;

	extern std::vector<int> ButtonList;

	extern float SpinOrientationDeciderAngle;

	void ButtonMaskChange();
	void LeftHandedModeChange();

	void Log(const int msgLogLevel, const char * fmt, ...);

	void SetIntValue(StaticFunctionTag* base, BSFixedString setting, UInt32 intValue);
	
	void SetFloatValue(StaticFunctionTag* base, BSFixedString setting, float floatValue);

	bool RegisterFuncs(VMClassRegistry* registry);
		
	enum eLogLevels
	{
		LOGLEVEL_ERR = 0,
		LOGLEVEL_WARN,
		LOGLEVEL_INFO,
	};

	#define LOG(fmt, ...) Log(LOGLEVEL_WARN, fmt, ##__VA_ARGS__)
	#define LOG_ERR(fmt, ...) Log(LOGLEVEL_ERR, fmt, ##__VA_ARGS__)
	#define LOG_INFO(fmt, ...) Log(LOGLEVEL_INFO, fmt, ##__VA_ARGS__)
	
	//void ReadConfigFile();
	
}

#endif
////