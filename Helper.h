#pragma once
#include "skse64\NiNodes.h"
#include "skse64\NiTypes.h"
#include "skse64/GameObjects.h"
#include "skse64/NiExtraData.h"
#include "skse64/GameRTTI.h"
#include "skse64/InternalTasks.h"
#include "skse64/PluginAPI.h"
#include "skse64/PapyrusActor.h"
#include "skse64/PapyrusGame.h"
#include "skse64_common/SafeWrite.h"
#include "skse64/GameExtraData.h"

#include "MenuChecker.h"

namespace PowerAttackVR
{
#define NINODE_CHILDREN(ninode) ((NiTArray <NiAVObject *> *) ((char*)(&((ninode)->m_children))))

	extern std::string pluginName;

	extern std::vector<UInt32> notExteriorWorlds;
		
	UInt32 GetFullFormID(const ModInfo * modInfo, UInt32 formLower);

	void GameLoad();
	void LoadGlobalValues();

	bool hasExtraData(NiAVObject * node, const char * name);
	NiNode * getWorldRoot();

	NiStringsExtraData * NiStringsExtraData_Create(BSFixedString name, BSFixedString * stringData, UInt32 size);
	NiExtraData* NiExtraData_Create(UInt32 size, uintptr_t vtbl);
	
	NiPoint3 GetPlayerPos();
	
	extern SKSETaskInterface	* g_task;

	//Function calls
	float GetHeadingAngleFunc(TESObjectREFR* akSource, TESObjectREFR* akOther);

	typedef bool(*_DoCombatSpellApply)(VMClassRegistry* registry, UInt32 stackId, Actor* akActor, SpellItem* spell, TESObjectREFR* akTarget);
	extern RelocAddr<_DoCombatSpellApply> RemoteCastSpell;

	typedef void(*_MoveTo)(VMClassRegistry* registry, UInt32 stackId, TESObjectREFR* akSource, TESObjectREFR* refObj, float afXOffset, float afYOffset, float afZOffset, bool abMatchRotation);
	extern RelocAddr<_MoveTo> MoveTo;
	
	typedef void(*_AddItem)(VMClassRegistry* registry, UInt32 stackId, TESObjectREFR* akSource, TESForm* akItemToAdd, int aiCount, bool abSilent);
	extern RelocAddr<_AddItem> AddItem;

	typedef void(*_RemoveItem)(VMClassRegistry* registry, UInt32 stackId, TESObjectREFR* akSource, TESForm* akItemToRemove, int aiCount, bool abSilent, TESObjectREFR* akOtherContainer);
	extern RelocAddr<_RemoveItem> RemoveItem;
	typedef void(*_Fire)(VMClassRegistry* registry, UInt32 stackId, TESObjectWEAP* akWeapon, TESObjectREFR* akSource, TESAmmo* akAmmo);
	extern RelocAddr<_Fire> Fire;

	typedef bool(*_SetAngle)(VMClassRegistry* registry, UInt32 stackId, TESObjectREFR* akSource, float afXAngle, float afYAngle, float afZAngle);
	extern RelocAddr<_SetAngle> SetAngle;

	typedef float(*_GetHeadingAngle)(VMClassRegistry* registry, UInt32 stackId, TESObjectREFR* akSource, TESObjectREFR* akOther);
	extern RelocAddr<_GetHeadingAngle> GetHeadingAngle;

	typedef UInt32(*_GetItemCount)(VMClassRegistry * registry, UInt64 stackID, TESObjectREFR *actorRefr, TESForm *akItem);
	extern RelocAddr <_GetItemCount> GetItemCount;

	typedef void(*_PushActorAway)(VMClassRegistry* registry, UInt32 stackId, TESObjectREFR* akSource, Actor* akActor, float afKnockbackForce);
	extern RelocAddr<_PushActorAway> PushActorAway;

	typedef void(*_DebugSendAnimationEvent)(VMClassRegistry* VMinternal, UInt32 stackId, void* unk1, TESObjectREFR* objectRefr, BSFixedString const &animEvent);
	extern RelocAddr<_DebugSendAnimationEvent> DebugSendAnimationEvent;

	typedef void(*_EquipSpell)(VMClassRegistry* registry, UInt32 stackId, Actor* akActor, SpellItem* akSpell, int aiSource);
	extern RelocAddr<_EquipSpell> EquipSpell;

	typedef __int64 (*_GetVelocity)(TESObjectREFR * ref, NiPoint3 * velocity);
	extern RelocAddr<_GetVelocity> GetVelocity;

	typedef void(*_Delete)(VMClassRegistry* registry, UInt32 stackId, TESObjectREFR* obj);
	extern RelocAddr<_Delete> Delete;

	typedef void(*_SetMotionType)(VMClassRegistry* registry, UInt32 stackId, TESObjectREFR* obj, UInt32 aiMotionType, bool abAllowActivate);
	extern RelocAddr<_SetMotionType> SetMotionType;

	typedef void(*_TranslateTo)(VMClassRegistry* registry, UInt32 stackId, TESObjectREFR* obj, float afX, float afY, float afZ, float afAngleX, float afAngleY, float afAngleZ, float afSpeed, float afMaxRotationSpeed);
	extern RelocAddr<_TranslateTo> TranslateTo;

	typedef void(*_StopTranslation)(VMClassRegistry* registry, UInt32 stackId, TESObjectREFR* obj);
	extern RelocAddr<_StopTranslation> StopTranslation;

	typedef bool(*_IsAttached)(VMClassRegistry* registry, UInt32 stackId, TESObjectCELL* cell);
	extern RelocAddr<_IsAttached> IsAttached;

	typedef void(*_ActorEquipItem)(VMClassRegistry* registry, UInt64 stackID, Actor* actor, TESForm* akItem, bool abPreventRemoval, bool abSilent);
	extern RelocAddr <_ActorEquipItem> ActorEquipItem;

	typedef void(*_ActorUnEquipItem)(VMClassRegistry * registry, UInt64 stackID, Actor *actor, TESForm *akItem, bool abPreventEquip, bool abSilent);
	extern RelocAddr <_ActorUnEquipItem> ActorUnEquipItem;

	typedef bool(*_PlayImpactEffect)(VMClassRegistry* VMinternal, UInt32 stackId, TESObjectREFR* obj, BGSImpactDataSet* impactData, BSFixedString const &asNodeName, float afPickDirX, float afPickDirY,
		float afPickDirZ, float afPickLength, bool abApplyNodeRotation, bool abUseNodeLocalRotation);
	extern RelocAddr<_PlayImpactEffect> PlayImpactEffect;
	
	typedef void(*_PlaySoundEffect)(VMClassRegistry* VMinternal, UInt32 stackId, TESSound* sound, TESObjectREFR* source);
	extern RelocAddr <_PlaySoundEffect> PlaySoundEffect;

	typedef void(*_DamageActorValue)(VMClassRegistry* VMinternal, UInt32 stackId, Actor * thisActor, BSFixedString const &dmgValueName, float dmg);
	extern RelocAddr<_DamageActorValue> DamageActorValue;

	typedef void(*_RestoreActorValue)(VMClassRegistry* VMinternal, UInt32 stackId, Actor * thisActor, BSFixedString const &dmgValueName, float amount);
	extern RelocAddr<_RestoreActorValue> RestoreActorValue;

	typedef bool(*_HasSpell)(VMClassRegistry * registry, UInt64 stackID, Actor *actor, TESForm *akSpell);
	extern RelocAddr <_HasSpell> HasSpell;

	typedef bool(*_IsInCombat)(VMClassRegistry * registry, UInt64 stackID, Actor *actor);
	extern RelocAddr <_IsInCombat> IsInCombat;

	typedef bool(*_IsInCombatNative)(Actor *actor);

	typedef void(*_VisualEffect_Play)(VMClassRegistry* registry, UInt32 stackId, BGSReferenceEffect *shader, TESObjectREFR *target, float duration, TESObjectREFR *facingobject);
	extern RelocAddr <_VisualEffect_Play> VisualEffect_Play;

	typedef void(*_VisualEffect_Stop)(VMClassRegistry* registry, UInt32 stackId, BGSReferenceEffect* shader, TESObjectREFR* target);
	extern RelocAddr <_VisualEffect_Stop> VisualEffect_Stop;

	typedef void(*_PlayEffectArt)(TESObjectREFR *ref, BGSArtObject *effectArt, float duration, TESObjectREFR *facingRef, UInt8 a5, UInt8 a6, __int64 a7, bool a8);
	extern RelocAddr <_PlayEffectArt> PlayEffectArt;

	typedef void(*_PlayEffectShader)(TESObjectREFR *ref, TESEffectShader *effectShader, float duration, TESObjectREFR *facingRef, UInt8 a5, UInt8 a6, __int64 a7, bool a8);
	extern RelocAddr <_PlayEffectShader> PlayEffectShader;

	typedef bool(*_CastSpell)(VMClassRegistry* registry, UInt32 stackId, SpellItem* spell, TESObjectREFR* akSource, TESObjectREFR* akTarget);
	extern RelocAddr<_CastSpell> CastSpell;

	typedef TESObjectREFR* (*_DropObject)(VMClassRegistry* registry, UInt64 stackID, TESObjectREFR* actorRefr, TESForm* akItem, int aiCount);
	extern RelocAddr <_DropObject> DropObject;

	typedef void (*_Decapitate)(Actor* thisActor);
	extern RelocAddr <_Decapitate> Decapitate;

	typedef void(*_ApplyImageSpaceModifier)(VMClassRegistry* registry, UInt32 stackId, TESImageSpaceModifier* imod, float afStrength);
	extern RelocAddr<_ApplyImageSpaceModifier> ApplyImageSpaceModifier;

	typedef void(*_RemoveImageSpaceModifier)(VMClassRegistry* registry, UInt32 stackId, TESImageSpaceModifier* imod);
	extern RelocAddr<_RemoveImageSpaceModifier> RemoveImageSpaceModifier;


	class TESGlobal : public TESForm
	{
	public:
		enum { kTypeID = kFormType_Global };

		BSString unk20; // 20
		UInt8 unk1C; // 30 - type? - init'd to 115
		UInt8 pad[3]; // 31
		float value; // 34 corrected type which shows the FLTV (Value) of the GLOB form
	};

	class BGSBaseAlias // Not actually a form, but its used like one in Papyrus
	{
	public:

		enum class FillType : UInt16
		{
			kNone = 0,
			kForcedReference = 1,
			kLocationAliasReference = 2,
			kFindMatchingReferenceFromEvent = 3,
			kCreateReferenceToObject = 4,
			kExternalAliasReference = 5,
			kUniqueActor = 6,
			kFindMatchingReferenceNearAlias = 7
		};
		
		virtual ~BGSBaseAlias();
		virtual void		Unk01(void);
		virtual void		Unk02(void);
		virtual const char	* Unk03(void);

		enum { kTypeID = kFormType_Alias };

		BSFixedString name;	// 08
		TESQuest * owner;		// 10
		UInt32 aliasId;			// 18 - init'd to FFFFFFFF
		UInt32 aliasFlags;			// 1C - init'd to 0
		FillType filltype;			// 20 - init'd to 0
		UInt16 unk22;		// 22
		UInt32 pad24;		// 24
	};

	class BGSRefAlias : public BGSBaseAlias
	{
	public:
		const void* RTTI = RTTI_BGSRefAlias;

		union FillData
		{
			struct LocationAliasReference
			{
				UInt32			alias;		// 00 - ALFA
				UInt32			pad04;		// 04
				BGSLocation*	refType;	// 08 - ALRT
			};
			STATIC_ASSERT(sizeof(LocationAliasReference) == 0x10);


			struct ReferenceFromEvent
			{
				UInt32	fromEvent;	// ALFE
				UInt32		eventData;	// ALFD
			};
			STATIC_ASSERT(sizeof(ReferenceFromEvent) == 0x8);


			struct ReferenceToObject
			{
				enum class Level : UInt16	// ALCL
				{
					kEasy = 0,
					kMedium = 1,
					kHard = 2,
					kVeryHard = 3,
					kNone = 4
				};


				struct Alias	// ALCA
				{
					enum class Create : UInt16
					{
						kAt = 0x0000,
						kIn = 0x8000
					};


					UInt16	alias;	// 0
					Create	create;	// 2
				};
				STATIC_ASSERT(sizeof(Alias) == 0x4);


				TESObjectREFR*	object;	// 00 - ALCO
				Alias			alias;	// 08 - ALCA
				Level			level;	// 0C - ALCL
				UInt16			pad0E;	// 0E
			};
			STATIC_ASSERT(sizeof(ReferenceToObject) == 0x10);


			struct ExternalAliasReference
			{
				TESQuest*	quest;	// 00 - ALEQ
				UInt32		alias;	// 08 - ALEA
				UInt32		pad0C;	// 0C
			};
			STATIC_ASSERT(sizeof(ExternalAliasReference) == 0x10);


			struct ReferenceNearAlias
			{
				enum class Type : UInt32
				{
					kLinkedRefChild = 0
				};


				UInt32	alias;	// 0 - ALNA
				Type	type;	// 4 - ALNT
			};
			STATIC_ASSERT(sizeof(ReferenceNearAlias) == 0x8);


			// members
			UInt32				forcedRef;			// ALFR - kForcedReference
			LocationAliasReference	locationAliasRef;	// kLocationAliasReference
			ReferenceFromEvent		refFromEvent;		// kFindMatchingReferenceFromEvent
			ReferenceToObject		refToObject;		// kCreateReferenceToObject
			ExternalAliasReference	externalAliasRef;	// kExternalAliasReference
			TESNPC*					uniqueActor;		// ALUA - kUniqueActor
			ReferenceNearAlias		refNearAlias;		// kFindMatchingReferenceNearAlias
		};
		STATIC_ASSERT(sizeof(FillData) == 0x10);


		virtual ~BGSRefAlias();											// 00

																		// override (BGSBaseAlias)
		virtual bool					Load(TESFile* a_mod);	// 01
		virtual void					Unk_02(void);			// 02
		virtual const BSFixedString&	GetTypeString() const;	// 03 - { return "Ref"; }


																		// members
		FillData	fillData;	// 28
		UInt64		unk38;		// 38
		Condition*	conditions;	// 40
	};

	class BSTempEffect : public NiObject
	{
	public:

		virtual ~BSTempEffect();

		// members
		UInt32	unk10;	// 10
		UInt32	unk14;	// 14
		UInt64	unk18;	// 18
		UInt32	unk20;	// 20
		UInt8	unk24;	// 24
		UInt8	unk25;	// 25
		UInt16	unk26;	// 26
		UInt32	unk28;	// 28
		UInt32	unk2C;	// 2C
	};
	STATIC_ASSERT(sizeof(BSTempEffect) == 0x30);


	class ReferenceEffect : public BSTempEffect
	{
	public:

		virtual ~ReferenceEffect();

		// members
		UInt32						controller;	// 30
		UInt32						refHandle;	// 38
		UInt32						unk3C;		// 3C
		UInt8						unk40;		// 40
		UInt8						unk41;		// 41
		UInt16						unk42;		// 42
		UInt32						unk44;		// 44
	};
	STATIC_ASSERT(sizeof(ReferenceEffect) == 0x48);

	class AIProcessManager
	{
	public:
		static AIProcessManager* GetSingleton();

		// members
		bool						enableDetection;				// 001
		bool						enableDetectionStats;			// 002
		UInt8						pad003;							// 003
		UInt32						trackedDetectionHandle;			// 004
		bool						enableHighProcessing;			// 008
		bool						enableLowProcessing;			// 009
		bool						enableMiddleHighProcessing;		// 00A
		bool						enableMiddleLowProcessing;		// 00B
		UInt16						unk00C;							// 00C
		UInt8						unk00E;							// 00E
		UInt8						pad00F;							// 00F
		SInt32						numActorsInHighProcess;			// 010
		float						unk014;							// 014
		UInt32						unk018;							// 018
		float						removeExcessComplexDeadTime;	// 01C
		HANDLE						semaphore;						// 020
		UInt32						unk028;							// 028
		UInt32						pad02C;							// 02C
		tArray<UInt32>				highProcesses;					// 030
		tArray<UInt32>				lowProcesses;					// 048
		tArray<UInt32>				middleLowProcesses;				// 060
		tArray<UInt32>				middleHighProcesses;			// 078
		tArray<UInt32>*				highProcessesPtr;				// 090
		tArray<UInt32>*				lowProcessesPtr;				// 098
		tArray<UInt32>*				middleLowProcessesPtr;			// 0A0
		tArray<UInt32>*				middleHighProcessesPtr;			// 0A8
		UInt64						unk0B0;							// 0B0
		UInt64						unk0B8;							// 0B8
		UInt64						unk0C0;							// 0C0
		UInt64						unk0C8;							// 0C8
		UInt64						unk0D0;							// 0D0
		UInt64						unk0D8;							// 0D8
		UInt64						unk0E0;							// 0E0
		tArray<BSTempEffect*>		tempEffects;					// 0E8
		SimpleLock					tempEffectsLock;				// 100
		tArray<ReferenceEffect*>	referenceEffects;				// 108
		SimpleLock					referenceEffectsLock;			// 120
		tArray<void*>				unk128;							// 128
		UInt64						unk140;							// 140
		UInt64						unk148;							// 148
		UInt64						unk150;							// 150
		tArray<UInt32>				unk158;							// 158
		UInt32						unk170;							// 170
		UInt32						pad174;							// 174
		UInt64						unk178;							// 178
		tArray<void*>				unk180;							// 180
		SimpleLock					unk198;							// 198
		tArray<UInt32>				unk1A0;							// 1A0
		tArray<void*>				unk1B8;							// 1B8
		float						unk1D0;							// 1D0
		float						unk1D4;							// 1D4
		UInt64						unk1D8;							// 1D8
		UInt32						unk1E0;							// 1E0
		bool						enableAIProcessing;				// 1E4
		bool						enableMovementProcessing;		// 1E5
		bool						enableAnimationProcessing;		// 1E6
		UInt8						unk1E7;							// 1E7
		UInt64						unk1E8;							// 1E8


		MEMBER_FN_PREFIX(AIProcessManager);
		DEFINE_MEMBER_FN(StopArtObject, void, 0x007048E0, TESObjectREFR*, BGSArtObject*);

	};
	STATIC_ASSERT(sizeof(AIProcessManager) == 0x1F0);

	class ActorCause
	{
	public:
		SInt32	DecRefCount();
		SInt32	IncRefCount();
		SInt32	GetRefCount() const;


		// members
		UInt32		actor;			// 00
		NiPoint3				origin;			// 04
		UInt32					actorCauseID;	// 10
		volatile mutable SInt32	refCount;		// 14
	};
	STATIC_ASSERT(sizeof(ActorCause) == 0x18);

	// Must be aligned to 16 bytes (128 bits) as it's a simd type
	__declspec(align(16)) struct hkVector4
	{
		float x;
		float y;
		float z;
		float w;
	};
	STATIC_ASSERT(sizeof(hkVector4) == 0x10);
	
	struct hkTransform
	{
		float m_rotation[12]; // 00 - 3x4 matrix, 3 rows of hkVector4
		hkVector4 m_translation; // 30
	};
	STATIC_ASSERT(sizeof(hkTransform) == 0x40);

	struct hkSweptTransform
	{
		hkVector4 m_centerOfMass0; // 00
		hkVector4 m_centerOfMass1; // 10
		hkVector4 m_rotation0; // 20 - Quaternion
		hkVector4 m_rotation1; // 30 - Quaternion
		hkVector4 m_centerOfMassLocal; // 40 - Often all 0's
	};
	STATIC_ASSERT(sizeof(hkSweptTransform) == 0x50);

	struct hkMotionState
	{
		hkTransform m_transform; // 00
		hkSweptTransform m_sweptTransform; // 40

		hkVector4 m_deltaAngle; // 90
		float m_objectRadius; // A0
		float m_linearDamping; // A4
		float m_angularDamping; // A8
								// These next 2 are hkUFloat8, 8-bit floats
		UInt8 m_maxLinearVelocity; // AC
		UInt8 m_maxAngularVelocity; // AD
		UInt8 m_deactivationClass; // AE
		UInt8 padAF;
	};
	STATIC_ASSERT(sizeof(hkMotionState) == 0xB0);
	
	struct hkArray
	{
		void * m_data;
		int m_size;
		int m_capacityAndFlags;
	};
	STATIC_ASSERT(sizeof(hkArray) == 0x10);

	struct hkpTypedBroadPhaseHandle
	{
		// Inherited from hkpBroadPhaseHandle
		UInt32 m_id; // 00

		SInt8 m_type; // 04
		SInt8 m_ownerOffset; // 05
		SInt8 m_objectQualityType; // 06
		UInt8 pad07;
		UInt32 m_collisionFilterInfo; // 08
	};
	STATIC_ASSERT(sizeof(hkpTypedBroadPhaseHandle) == 0x0C);
	
	struct hkpCdBody
	{
		void * m_shape; // 00
		UInt32 m_shapeKey; // 08
		UInt32 pad0C;
		void * m_motion; // 10
		hkpCdBody * m_parent; // 18
	};
	STATIC_ASSERT(sizeof(hkpCdBody) == 0x20);

	struct hkpCollidable : public hkpCdBody
	{
		struct BoundingVolumeData
		{
			UInt32 m_min[3]; // 00
			UInt8 m_expansionMin[3]; // 0C
			UInt8 m_expansionShift; // 0F
			UInt32 m_max[3]; // 10
			UInt8 m_expansionMax[3]; // 1C
			UInt8 m_padding; // 1F
			UInt16 m_numChildShapeAabbs; // 20
			UInt16 m_capacityChildShapeAabbs; // 22
			UInt32 pad24;
			void * m_childShapeAabbs; // 28 - it's a hkAabbUint32 *
			UInt32 * m_childShapeKeys; // 30
		};
		STATIC_ASSERT(sizeof(BoundingVolumeData) == 0x38);

		SInt8 m_ownerOffset; // 20
		SInt8 m_forceCollideOntoPpu; // 21
		SInt16 m_shapeSizeOnSpu; // 22
		hkpTypedBroadPhaseHandle m_broadPhaseHandle; // 24
		BoundingVolumeData m_boundingVolumeData; // 30
		float m_allowedPenetrationDepth; // 68
		UInt32 pad6C;
	};
	STATIC_ASSERT(sizeof(hkpCollidable) == 0x70);

	struct hkpLinkedCollidable : public hkpCollidable
	{
		hkArray m_collisionEntries; // 70
	};
	STATIC_ASSERT(sizeof(hkpLinkedCollidable) == 0x80);

	struct hkpSimpleShapePhantom
	{
		void * vtbl; // 00
		// These 3 inherited from hkReferencedObject
		UInt16 m_memSizeAndFlags; // 08
		SInt16 m_referenceCount; // 0A
		UInt32 pad0C; // 0C

		void * world; // 10

		void * userData; // 18

		hkpLinkedCollidable m_collidable; // 20

		UInt64 todo[10];

		hkMotionState m_motionState; // F0

									 // more...
	};
	STATIC_ASSERT(offsetof(hkpSimpleShapePhantom, m_motionState) == 0xF0);

	struct bhkSimpleShapePhantom
	{
		void * vtbl; // 00
		volatile SInt32    m_uiRefCount;    // 08
		UInt32    pad0C;    // 0C

		hkpSimpleShapePhantom * phantom; // 10
	};

	class bhkCollisionObject;

	class Projectile : public TESObjectREFR
	{
	public:

		struct ImpactData
		{
		public:
			// members
			UInt64						  unk00;	 // 00
			UInt64						  unk08;	 // 08
			UInt64						  unk10;	 // 10
			NiPointer<TESObjectREFR*>	  collidee;	 // 18
			NiPointer<bhkCollisionObject> colObj;	 // 20
			UInt64						  unk28;	 // 28
			UInt64						  unk30;	 // 30
			UInt64						  unk38;	 // 38
			UInt64						  unk40;	 // 40
			UInt64						  unk48;	 // 48
		};
		STATIC_ASSERT(sizeof(ImpactData) == 0x50);

		
		// A8
		class LaunchData
		{
		public:
			virtual ~LaunchData();

			UInt8	unk08[0xA0]; // 08 TODO                                                             
		};

		tList<ImpactData*>			impacts;			   // 098
		float					   unk0A8;			   // 0A8
		float					   unk0AC;			   // 0AC
		UInt64						unk0B0;				// 0B0
		float						unk0B8;				// 0B8
		float						unk0BC;				// 0BC
		UInt64						unk0C0;				// 0C0
		float						unk0C8;				// 0C8
		float						unk0CC;				// 0CC
		UInt64						unk0D0;				// 0D0
		float						unk0D8;				// 0D8
		float						unk0DC;				// 0DC
		bhkSimpleShapePhantom*		phantom;
		
		UInt8	unkE8[0xF0 - 0xE8];
		NiPoint3 point;
		NiPoint3 velocity;
		void*						unk108;				// 108 - smart ptr
		void*						unk110;				// 110 - smart ptr
		UInt8	unk118[0x120 - 0x118];
		UInt32				shooter;			// 120
		UInt8	unk124[0x140 - 0x124];
		UInt32*						unk140;				// 140
		InventoryEntryData*			unk148;				// 148
		UInt64						unk150;				// 150
		UInt64						unk158;				// 158
		UInt64						unk160;				// 160
		EffectSetting*				magicEffect;		// 168
		UInt8	unk170[0x178 - 0x170];
		UInt64						unk178;				// 178
		UInt64						unk180;				// 180
		float						unk188;				// 188
		float						unk18C;				// 18C
		float						range;				// 190
		UInt32						unk194;				// 194
		float						unk198;				// 198
		float						unk19C;				// 19C
		UInt64						unk1A0;				// 1A0
		UInt64						unk1A8;				// 1A8
		TESObjectWEAP*				weaponSource;		// 1B0
		TESAmmo*					ammoSource;			// 1B8
		float						distanceMoved;		// 1C0
		UInt32						unk1C4;				// 1C4
		UInt32						unk1C8;				// 1C8
		UInt32						flags;				// 1CC
		UInt64						unk1D0;				// 1D0
	};
	STATIC_ASSERT(offsetof(Projectile, point) == 0xF0);
	STATIC_ASSERT(offsetof(Projectile, velocity) == 0xFC);
	STATIC_ASSERT(offsetof(Projectile, shooter) == 0x120);
	STATIC_ASSERT(offsetof(Projectile, phantom) == 0xE0);
	STATIC_ASSERT(offsetof(Projectile, impacts) == 0x98);
	STATIC_ASSERT(offsetof(Projectile, distanceMoved) == 0x1C0);
	STATIC_ASSERT(sizeof(Projectile) == 0x1D8);
}
