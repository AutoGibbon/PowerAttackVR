#include "skse64_common/skse_version.h"
#include <shlobj.h>
#include <intrin.h>
#include <string>
#include "skse64/PluginAPI.h"	
#include "api/PapyrusVRAPI.h"
#include "api/VRManagerAPI.h"
#include "PowerAttackVR.h"

namespace PowerAttackVR
{
	#pragma comment(lib, "Ws2_32.lib")
	
	static SKSEMessagingInterface		* g_messaging = NULL;
	static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
	static SKSEPapyrusInterface         * g_papyrus = NULL;
	static SKSEObjectInterface         * g_object = NULL;
	SKSETaskInterface				* g_task = NULL;
	PapyrusVRAPI* g_papyrusvr;

	//RelocPtr<uintptr_t> ArrowProjectileVtbl(0x016F93A8);
	//RelocPtr<uintptr_t> ProjectileVtbl(0x016FF938);
	
	
	
	
	void SetupReceptors()
	{
		_MESSAGE("Building Event Sinks...");
		
		MenuManager * menuManager = MenuManager::GetSingleton();
		if (menuManager)
			menuManager->MenuOpenCloseEventDispatcher()->AddEventSink(&PowerAttackVR::menuEvent);
	}

	extern "C" {

		bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info) {	// Called by SKSE to learn about this plugin and check that it's safe to load it
			gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim VR\\SKSE\\PowerAttackVR.log");
			gLog.SetPrintLevel(IDebugLog::kLevel_Error);
			gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
			//gLog.SetLogLevel(IDebugLog::kLevel_FatalError);

			std::string logMsg("PowerAttack VR: ");
			logMsg.append(PowerAttackVR::MOD_VERSION);
			_MESSAGE(logMsg.c_str());

			// populate info structure
			info->infoVersion = PluginInfo::kInfoVersion;
			info->name = "PowerAttackVR";
			info->version = 000002; // 0.0.1

			// store plugin handle so we can identify ourselves later
			g_pluginHandle = skse->GetPluginHandle();
						

			std::string skseVers = "SKSE Version: ";
			skseVers += std::to_string(skse->runtimeVersion);
			_MESSAGE(skseVers.c_str());

			if (skse->isEditor)
			{
				_MESSAGE("loaded in editor, marking as incompatible");

				return false;
			}
			else if (skse->runtimeVersion < CURRENT_RELEASE_RUNTIME)
			{
				_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);

				return false;
			}


			// ### do not do anything else in this callback
			// ### only fill out PluginInfo and return true/false

			// supported runtime version
			return true;
		}

		//Listener for PapyrusVR Messages
		void OnPapyrusVRMessage(SKSEMessagingInterface::Message* msg)
		{
			if (msg)
			{
				if (msg->type == kPapyrusVR_Message_Init && msg->data)
				{
					_MESSAGE("SkyrimVRTools Init Message received with valid data, registering for callback");
					g_papyrusvr = (PapyrusVRAPI*)msg->data;

					PowerAttackVR::hookManager = g_papyrusvr->GetOpenVRHook();
				}
			}
		}

		//Listener for SKSE Messages
		void OnSKSEMessage(SKSEMessagingInterface::Message* msg)
		{
			if (msg)
			{
				if (msg->type == SKSEMessagingInterface::kMessage_InputLoaded)
					SetupReceptors();
				else if (msg->type == SKSEMessagingInterface::kMessage_DataLoaded)
				{					
					PowerAttackVR::StartMod();
					PowerAttackVR::GameLoad();
				}
				else if (msg->type == SKSEMessagingInterface::kMessage_PostLoad)
				{
					_MESSAGE("SKSE PostLoad received, registering for SkyrimVRTools messages");
					//SkyrimVRTools registered here
					g_messaging->RegisterListener(g_pluginHandle, "SkyrimVRTools", OnPapyrusVRMessage);
				}
				else if (msg->type == SKSEMessagingInterface::kMessage_PostLoadGame)
				{
					if ((bool)(msg->data) == true)
					{
						PowerAttackVR::GameLoad();
						PowerAttackVR::AfterGameLoad();
					}
				}
			}
		}

		bool SKSEPlugin_Load(const SKSEInterface * skse) {	// Called by SKSE to load this plugin

			g_task = (SKSETaskInterface *)skse->QueryInterface(kInterface_Task);

			g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);

			//SafeWrite64(ArrowProjectileVtbl.GetUIntPtr() + 0x87 * 8, GetFnAddr(&GetVelocity_Hook));
			//SafeWrite64(ProjectileVtbl.GetUIntPtr() + 0x87 * 8, GetFnAddr(&GetVelocity_Hook));
			
			
			//static RelocPtr<uintptr_t> ArrowProjectileVtbl(0x016F93A8);
			//typedef UInt32(*GetVelocityOriginalFunctionArrow)(TESObjectREFR * ref, NiPoint3 * velocity);
			/*_MESSAGE("Hooking start");

			static RelocPtr<GetVelocityOriginalFunctionArrow> GetVelocityOriginalFunctionArrow_vtbl(ArrowProjectileVtbl_Offset + 0x87 * 8);
			static RelocPtr<GetVelocityOriginalFunctionArrow> GetVelocityOriginalFunctionProjectile_vtbl(ProjectileVtbl_Offset + 0x87 * 8);
			g_originalGetVelocityFunctionArrow = *GetVelocityOriginalFunctionArrow_vtbl;
			g_originalGetVelocityFunctionProjectile = *GetVelocityOriginalFunctionProjectile_vtbl;

			_MESSAGE("Hooking function Arrow: %p", (uintptr_t)g_originalGetVelocityFunctionArrow - uintptr_t(GetModuleHandle(NULL)));
			_MESSAGE("Hooking function Projectile: %p", (uintptr_t)g_originalGetVelocityFunctionProjectile - uintptr_t(GetModuleHandle(NULL)));

			SafeWrite64(GetVelocityOriginalFunctionArrow_vtbl.GetUIntPtr(), GetFnAddr(&GetVelocity_HookArrow));
			SafeWrite64(GetVelocityOriginalFunctionProjectile_vtbl.GetUIntPtr(), GetFnAddr(&GetVelocity_HookProjectile));*/

			g_messaging = (SKSEMessagingInterface*)skse->QueryInterface(kInterface_Messaging);
			g_messaging->RegisterListener(g_pluginHandle, "SKSE", OnSKSEMessage);

			g_papyrus->Register(PowerAttackVR::RegisterFuncs);
						
			return true;
		}
	};

}
