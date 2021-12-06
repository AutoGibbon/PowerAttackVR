#include "config.h"
#include "skse64/GameRTTI.h"


namespace PowerAttackVR
{
	//Config parameters are defined here
	const int enableLog = 1;

	int leftHandedMode = 0;
	int HoldButton = 33;
	int HoldButtonSecondary = 33;
	bool releaseWithNoTouch = false;
	std::vector<int> ButtonList = { 33, 2, 7, 0, 1, 32, 34, 35, 36 };

	float SpinOrientationDeciderAngle = 5.0f;

	/// This function is used to set int config parameters from MCM to skse plugin.
	void SetIntValue(StaticFunctionTag* base, BSFixedString setting, UInt32 intValue)
	{
		LOG("Value change - %s: %d", setting.data, intValue);
	}

	/// This function is used to set float config parameters from MCM to skse plugin.
	void SetFloatValue(StaticFunctionTag* base, BSFixedString setting, float floatValue)
	{
		LOG("Value change - %s: %g", setting.data, floatValue);
	}

	/// Registering papyrus scripts for MCM to communicate with the SKSE plugin.
	bool RegisterFuncs(VMClassRegistry* registry)
	{
		registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, void, BSFixedString, UInt32>("SetIntValue", "PowerAttackVR_PluginScript", SetIntValue, registry));

		registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, void, BSFixedString, float>("SetFloatValue", "PowerAttackVR_PluginScript", SetFloatValue, registry));
		
		return true;
	}

	///Log function
	void Log(const int msgLogLevel, const char * fmt, ...)
	{
		if (msgLogLevel > enableLog)
		{
			return;
		}

		va_list args;
		char logBuffer[4096];
		
		va_start(args, fmt);
		vsprintf_s(logBuffer, sizeof(logBuffer), fmt, args);
		va_end(args);

		_MESSAGE(logBuffer);
	}
}
