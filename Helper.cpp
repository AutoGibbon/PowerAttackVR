#include "Helper.h"

namespace PowerAttackVR
{
	RelocAddr<_DoCombatSpellApply> DoCombatSpellApply(0x00992240);
	RelocAddr<_MoveTo> MoveTo(0x009CF360);
	RelocAddr<_RemoveItem> RemoveItem(0x009D1190);
	RelocAddr<_AddItem> AddItem(0x009CDC20);
	RelocAddr<_Fire> Fire(0x009EA9F0);
	RelocAddr<_SetAngle> SetAngle(0x009D18F0);
	RelocAddr<_GetHeadingAngle> GetHeadingAngle(0x009CEBB0);
	RelocAddr <_GetItemCount> GetItemCount(0x09CEC90);
	RelocAddr<_PushActorAway> PushActorAway(0x009D0E60);
	RelocAddr <_EquipSpell> EquipSpell(0x0984CA0);
	RelocAddr<_CastSpell> CastSpell(0x009BB6B0);

	RelocAddr <_ActorEquipItem> ActorEquipItem(0x09849C0);
	RelocAddr <_DropObject> DropObject(0x09CE580);

	RelocAddr<_GetVelocity> GetVelocity(0x0077B440);
	RelocAddr<_Delete> Delete(0x009CE380);
	RelocAddr<_SetMotionType> SetMotionType(0x009D1FF0);
	RelocAddr<_TranslateTo> TranslateTo(0x009D2570);
	RelocAddr<_StopTranslation> StopTranslation(0x009D3140);
	RelocAddr<_IsAttached> IsAttached(0x009C9D60);
	RelocAddr<_ActorUnEquipItem> ActorUnEquipItem(0x0987AF0);
	RelocAddr<_PlaySoundEffect> PlaySoundEffect(0x009EF150);
	RelocAddr<_PlayImpactEffect> PlayImpactEffect(0x009D06C0);

	RelocAddr<_DamageActorValue> DamageActorValue(0x09848B0);
	RelocAddr<_RestoreActorValue> RestoreActorValue(0x0986480);
	RelocAddr <_HasSpell> HasSpell(0x0984420);
	RelocAddr <_IsInCombat> IsInCombat(0x0992400);
	
	RelocAddr<_VisualEffect_Play> VisualEffect_Play(0x9A4E00);
	RelocAddr<_VisualEffect_Stop> VisualEffect_Stop(0x9A4F80);

	RelocAddr<_PlayEffectArt> PlayEffectArt(0x320EB0);
	RelocAddr<_PlayEffectShader> PlayEffectShader(0x2AE290);
	
	RelocAddr<_Decapitate> Decapitate(0x005FAC70);

	RelocAddr<_ApplyImageSpaceModifier> ApplyImageSpaceModifier(0x009C3C70);
	RelocAddr<_RemoveImageSpaceModifier> RemoveImageSpaceModifier(0x009C3CE0);

	std::string pluginName = "PowerAttackVR.esp";


	//Formid of the mod
	UInt32 powerAttackControlValueFormId = 0x000D61;
	UInt32 powerAttackControlValueFullFormId;
	TESGlobal* powerAttackControlValueGlobal;

	//Globals
	UInt32 globalEnableLogFormId = 0x000DB2;

	// Gets the root NiNode of the game world
	NiNode * getWorldRoot()
	{
		NiNode * node = (*g_thePlayer)->loadedState->node;
		while (node && node->m_parent) 
		{
			node = node->m_parent;
		}
		return node;
	}
	
	// Functions for manipulating extra data
	bool hasExtraData(NiAVObject * node, const char * name)
	{
		if (!node || !node->m_extraData)
			return false;
		
		for (int i = 0; i < node->m_extraDataLen; ++i) 
		{
			NiExtraData * extraData = node->m_extraData[i];
			if (!extraData)
				continue;
			
			if (strcmp(extraData->m_pcName, name) == 0) 
			{
				return true;
			}
		}
		return false;
	}

	
	// Modified version of NiExtraData::Create from SKSE - corrected to use 64 bit pointers
	NiExtraData* NiExtraData_Create(UInt32 size, uintptr_t vtbl)
	{
		void* memory = Heap_Allocate(size);
		memset(memory, 0, size);
		NiExtraData* xData = (NiExtraData*)memory;
		((uintptr_t *)memory)[0] = vtbl;
		return xData;
	}

	// Nearly identical version of NiStringsExtraData::Create from SKSE - modified to use the corrected function above
	RelocAddr<uintptr_t> g_extraDataStringsVtbl(0x017F0A20);
	NiStringsExtraData * NiStringsExtraData_Create(BSFixedString name, BSFixedString * stringData, UInt32 size)
	{
		NiStringsExtraData* data = (NiStringsExtraData*)NiExtraData_Create(sizeof(NiStringsExtraData), g_extraDataStringsVtbl.GetUIntPtr());
		if (data)
		{
			data->m_pcName = const_cast<char*>(name.data);
			data->m_data = (char**)Heap_Allocate(sizeof(char*) * size);
			data->m_size = size;
			for (int i = 0; i < size; i++)
			{
				UInt32 strLength = strlen(stringData[i].data) + 1;
				data->m_data[i] = (char*)Heap_Allocate(sizeof(char*) * strLength);
				memcpy(data->m_data[i], stringData[i].data, sizeof(char) * strLength);
			}
		}
		return data;
	}


	float GetHeadingAngleFunc(TESObjectREFR* akSource, TESObjectREFR* akOther)
	{
		return GetHeadingAngle((*g_skyrimVM)->GetClassRegistry(), 0, akSource, akOther);
	}


	// Converts the lower bits of a FormID to a full FormID depending on plugin type
	UInt32 GetFullFormID(const ModInfo * modInfo, UInt32 formLower)
	{
		return (modInfo->modIndex << 24) | formLower;
	}

	inline bool file_exists(const std::string& name) {
		struct stat buffer;
		return (stat(name.c_str(), &buffer) == 0);
	}

	void LoadGlobalValues()
	{
		std::string	runtimeDirectory = GetRuntimeDirectory();
	
		DataHandler * dataHandler = DataHandler::GetSingleton();

		if (dataHandler)
		{			
			const ModInfo * modInfo = dataHandler->LookupLoadedModByName(pluginName.c_str());

			if (modInfo)
			{
				if (IsValidModIndex(modInfo->modIndex)) //If plugin is in the load order.
				{
					/*const UInt32 globalEnableLogFullFormId = GetFullFormID(modInfo, GetBaseFormID(globalEnableLogFormId));
					if (globalEnableLogFullFormId > 0)
					{
						const auto form = LookupFormByID(globalEnableLogFullFormId);
						if (form)
						{
							const auto EnableLogGlobal = DYNAMIC_CAST(form, TESForm, TESGlobal);

							enableLog = (int)(EnableLogGlobal->value);

							LOG_INFO("enableLog is %d", enableLog);
						}
					}*/
					powerAttackControlValueFullFormId = GetFullFormID(modInfo, GetBaseFormID(powerAttackControlValueFormId));
					if (powerAttackControlValueFullFormId > 0)
					{
						const auto form = LookupFormByID(powerAttackControlValueFullFormId);
						if (form)
						{
							powerAttackControlValueGlobal = DYNAMIC_CAST(form, TESForm, TESGlobal);
							_MESSAGE("loaded power attack control global");
						}
					}
				}
			}
		}
	}

	bool runOnceGameLoad = false;
	
	void GameLoad()
	{
		if (runOnceGameLoad == false)
		{
			runOnceGameLoad = true;
			DataHandler* dataHandler = DataHandler::GetSingleton();

			if (dataHandler)
			{
				const ModInfo * modInfo = dataHandler->LookupLoadedModByName(pluginName.c_str());

				if (modInfo)
				{
					if (IsValidModIndex(modInfo->modIndex)) //If plugin is in the load order.
					{
						TESForm * form = nullptr;
					}
				}
				else
				{
					LOG_ERR("PowerAttackVR esp not found!!!!!!!!!!!!!!!!!");
				}

			}
		}
		
		LoadGlobalValues();
	}

	bool IsCellAttached(TESObjectCELL* cell)
	{
		return IsAttached((*g_skyrimVM)->GetClassRegistry(), 0, cell);
	}

	NiPoint3 GetPlayerPos()
	{
		NiPoint3 targetPos = (*g_thePlayer)->loadedState->node->m_worldTransform.pos;

		TESRace* race = (*g_thePlayer)->race;
		BGSBodyPartData* bodyPart = race->bodyPartData;
		BGSBodyPartData::Data* data;

		if (bodyPart)
		{
			// get actor's torso position
			data = bodyPart->part[0];
			if (data)
			{
				if (data->unk08.data[0])
				{
					NiAVObject* object = (NiAVObject*)(*g_thePlayer)->GetNiNode();
					if (object)
					{
						object = object->GetObjectByName(&data->unk08.data);
						if (object)
						{
							targetPos = object->m_worldTransform.pos;
						}
					}
				}
			}
		}

		return targetPos;
	}
	
}
