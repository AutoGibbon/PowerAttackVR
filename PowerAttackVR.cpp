#include "PowerAttackVR.h"

/*
	The .esp included with this project modifies the power attack animation conditions to respect a new global form value

	The main function in this file "cbFuncGetPoses" tracks the positions of the VR controllers over time, and updates this global value accordingly

	things to try -> swing handler

*/

namespace PowerAttackVR
{
	//0x1416f2258

	UInt32 funcAddress = 0x0127B460;

	RelocAddr<uintptr_t*> addressStart = funcAddress + 0x38D;
	RelocAddr<uintptr_t*> addressEnd = funcAddress + 0x3BF;

	uint64_t buttonMask = 0;
	uint64_t buttonMaskSecondary = 0;

	std::atomic<bool> LeftButtonHold;
	std::atomic<bool> RightButtonHold;

	std::atomic<bool> LeftButtonGeneralHold;
	std::atomic<bool> RightButtonGeneralHold;

	//Default left-right controller variables. For left handed support to be changed later if needed.
	vr::ETrackedControllerRole leftControllerRole = vr::ETrackedControllerRole::TrackedControllerRole_LeftHand;
	vr::ETrackedControllerRole rightControllerRole = vr::ETrackedControllerRole::TrackedControllerRole_RightHand;

	//Openvr system variable
	vr::IVRSystem* ivrSystem = nullptr;

	//SkyrimVRTools hookManagerApi variable
	OpenVRHookManagerAPI* hookManager;

	std::deque<HandPosition> currentRightHandPos;
	std::deque<HandPosition> currentLeftHandPos;

	std::deque<HandPosition> rightHandPos;
	std::deque<HandPosition> leftHandPos;

	///////////////////////////

	PlayerCharacter::Node leftWandNode = PlayerCharacter::kNode_LeftMeleeWeaponOffsetNode;
	PlayerCharacter::Node rightWandNode = PlayerCharacter::kNode_RightMeleeWeaponOffsetNode;

	PlayerCharacter::Node leftRealWandNode = PlayerCharacter::kNode_LeftHandBone;
	PlayerCharacter::Node rightRealWandNode = PlayerCharacter::kNode_RightHandBone;

	BSFixedString leftHandBone("NPC L Hand [LHnd]");
	BSFixedString rightHandBone("NPC R Hand [RHnd]");

	//Timing stuff
	// Class members:
	LARGE_INTEGER m_hpcFrequency;
	LARGE_INTEGER m_prevTime;
	double m_frameTime = 0;
	//

	///Initializes openvr system.
	void InitSystem(vr::IVRSystem* vrSystem)
	{
		if ((ivrSystem = vrSystem))
			LOG("VR System is alive.");
		else
			LOG("No VR System found.");
	}

	AIProcessManager* AIProcessManager::GetSingleton()
	{
		static RelocPtr<AIProcessManager*> singleton(0x01F831B0);
		return *singleton;
	}

	PapyrusVR::Matrix34 GetMatrix(vr::HmdMatrix34_t mat)
	{
		PapyrusVR::Matrix34 matrix;
		matrix.m[0][0] = mat.m[0][0];
		matrix.m[0][1] = mat.m[0][1];
		matrix.m[0][2] = mat.m[0][2];
		matrix.m[0][3] = mat.m[0][3];
		matrix.m[1][0] = mat.m[1][0];
		matrix.m[1][1] = mat.m[1][1];
		matrix.m[1][2] = mat.m[1][2];
		matrix.m[1][3] = mat.m[1][3];
		matrix.m[2][0] = mat.m[2][0];
		matrix.m[2][1] = mat.m[2][1];
		matrix.m[2][2] = mat.m[2][2];
		matrix.m[2][3] = mat.m[2][3];
		return matrix;
	}


	AttackDirection GetAttackDirection(const std::deque<HandPosition>& deque) {
		NiPoint3 firstPos = deque.front().pos;
		NiPoint3 lastPos = deque.back().pos;

		//get x magnitude


		return AttackDirection::AttackBackward;
	}

	Orientation GetOrientationFromPoints(const std::deque<HandPosition>& deque, SpinningType& outSpinningType)
	{
		//triangle
		NiPoint3 normal;
		std::deque<NiPoint3> heightPoints;

		for (int i = 0; i < deque.size() - 1; i++)
		{
			normal = normal + crossProduct(deque[i].pos, deque[i + 1].pos);

			heightPoints.emplace_back(deque[i].pos);
		}
		normal = normal + crossProduct(deque[deque.size() - 1].pos, deque[0].pos);
		normal = normalize(normal);

		bool highArch = HighArch(heightPoints);

		NiMatrix33 firstRot = deque.front().rot;
		float firstrotHeading;
		float firstrotAttitude;
		float firstrotbank;
		firstRot.GetEulerAngles(&firstrotHeading, &firstrotAttitude, &firstrotbank);
		firstrotHeading = firstrotHeading * 57.295776f;
		firstrotAttitude = firstrotAttitude * 57.295776f;
		firstrotbank = firstrotbank * 57.29577f;

		NiMatrix33 midRot = deque[(deque.size() - 1) / 2].rot;
		float midrotHeading;
		float midrotAttitude;
		float midrotbank;
		midRot.GetEulerAngles(&midrotHeading, &midrotAttitude, &midrotbank);
		midrotHeading = midrotHeading * 57.295776f;
		midrotAttitude = midrotAttitude * 57.295776f;
		midrotbank = midrotbank * 57.29577f;

		NiMatrix33 lastRot = deque.back().rot;
		float rotHeading;
		float rotAttitude;
		float rotbank;
		lastRot.GetEulerAngles(&rotHeading, &rotAttitude, &rotbank);
		rotHeading = rotHeading * 57.295776f;
		rotAttitude = rotAttitude * 57.295776f;
		rotbank = rotbank * 57.295776f;

		bool bladeLookingRight = rotAttitude > 0;

		LOG("rotHeading:%g rotAttitude:%g rotbank:%g", rotHeading, rotAttitude, rotbank);

		const float angle = angleBetweenVectors(NiPoint3(0, 0, 1), normal);
		LOG("Throw plane normal: x:%g y:%g z:%g - angle:%g", normal.x, normal.y, normal.z, angle);

		//LOG_ERR("======ROTBANK: %g -> %g -> %g  ======Forward: %s", firstrotbank, midrotbank, rotbank, forward ? "true" : "false");
		LOG("------High Arch: %s--Blade: %s----", highArch ? "true" : "false", bladeLookingRight ? "Left" : "Right");
		LOG("======ROTBANK: %g -> %g -> %g => %s", firstrotbank, midrotbank, rotbank, AreAnglesTurningClockwise(firstrotbank, midrotbank, rotbank) ? "CLOCKWISE" : "COUNTERCLOCKWISE");
		LOG("======ROTHEADING: %g -> %g -> %g => %s", firstrotHeading, midrotHeading, rotHeading, AreAnglesTurningClockwise(firstrotHeading, midrotHeading, rotHeading) ? "CLOCKWISE" : "COUNTERCLOCKWISE");
		LOG("======ROTATTITUDE: %g -> %g -> %g => %s", firstrotAttitude, midrotAttitude, rotAttitude, AreAnglesTurningClockwise(firstrotAttitude, midrotAttitude, rotAttitude) ? "CLOCKWISE" : "COUNTERCLOCKWISE");

		if (angle >= 45.0f - SpinOrientationDeciderAngle && angle <= 135.0f + SpinOrientationDeciderAngle) //Checking angle between the normal and vertical plane (reverse)
		{
			if (AreAnglesTurningClockwise(firstrotbank, midrotbank, rotbank))
			{
				if (!highArch)
				{
					outSpinningType = Upwards;
					LOG("======Spin type: Upwards==Orientation type: VerticalDown");
					return VerticalDown;
				}
				else
				{
					outSpinningType = Downwards;
					LOG("======Spin type: Downwards==Orientation type: VerticalUp");
					return VerticalUp;
				}
			}
			else
			{
				if (highArch)
				{
					outSpinningType = Downwards;
					LOG("======Spin type: Downwards==Orientation type: VerticalDown");
					return VerticalDown;
				}
				else
				{
					outSpinningType = Upwards;
					LOG("======Spin type: Upwards==Orientation type: VerticalUp");
					return VerticalUp;
				}
			}
		}
		else
		{
			if (AreAnglesTurningClockwise(firstrotHeading, midrotHeading, rotHeading))
			{
				if (!bladeLookingRight)
				{
					outSpinningType = Left;
					LOG("======Spin type: Left==Orientation type: HorizontalLeft");
					return HorizontalLeft;
				}
				else
				{
					outSpinningType = Left;
					LOG("======Spin type: Left==Orientation type: HorizontalRight");
					return HorizontalRight;
				}
			}
			else
			{
				if (!bladeLookingRight)
				{
					outSpinningType = Right;
					LOG("======Spin type: Right==Orientation type: HorizontalLeft");
					return HorizontalLeft;
				}
				else
				{
					outSpinningType = Right;
					LOG("======Spin type: Right==Orientation type: HorizontalRight");
					return HorizontalRight;
				}
			}
		}
	}

	std::atomic<double> rightHandTimer = 0;
	std::atomic<double> leftHandTimer = 0;
	
	///This function is only used to detect controller movement and save positions into a deque.
	vr::EVRCompositorError cbFuncGetPoses(VR_ARRAY_COUNT(unRenderPoseArrayCount) vr::TrackedDevicePose_t* pRenderPoseArray, uint32_t unRenderPoseArrayCount, VR_ARRAY_COUNT(unGamePoseArrayCount) vr::TrackedDevicePose_t* pGamePoseArray, uint32_t unGamePoseArrayCount)
	{
		LARGE_INTEGER newTime;
		QueryPerformanceCounter(&newTime);
		m_frameTime = (double)(newTime.QuadPart - m_prevTime.QuadPart) / m_hpcFrequency.QuadPart;
		if (m_frameTime > 0.05)
		{
			m_frameTime = 0.05;
		}
		m_prevTime = newTime;

		if (!isGameStopped())
		{
			if (ivrSystem)
			{
				const vr::TrackedDeviceIndex_t rightController = hookManager->GetVRSystem()->GetTrackedDeviceIndexForControllerRole(rightControllerRole);
				const vr::TrackedDeviceIndex_t leftController = hookManager->GetVRSystem()->GetTrackedDeviceIndexForControllerRole(leftControllerRole);

				if (pGamePoseArray)
				{
					if ((int)rightController >= 0)
					{
						if (pGamePoseArray[rightController].bDeviceIsConnected && pGamePoseArray[rightController].bPoseIsValid && pGamePoseArray[rightController].eTrackingResult == vr::TrackingResult_Running_OK)
						{
							if (RightButtonHold.load())
							{
								rightHandTimer.store(m_frameTime + rightHandTimer.load());
								const auto playerRefr = DYNAMIC_CAST((*g_thePlayer), Actor, TESObjectREFR);

								if (playerRefr && playerRefr->loadedState)
								{
									auto& nodeList = (*g_thePlayer)->nodeList;

									NiAVObject* hnode = nodeList[rightWandNode];

									if (hnode)
									{
										const NiPoint3 point = hnode->m_worldTransform.pos - playerRefr->pos;
										HandPosition hps;
										hps.pos = point;
										PapyrusVR::Matrix34 matrix = GetMatrix(pGamePoseArray[rightController].mDeviceToAbsoluteTracking);
										NiTransform transform;
										PapyrusVR::OpenVRUtils::CopyMatrix34ToNiTrasform(&matrix, &transform);
										hps.rot = transform.rot;
										hps.pos2 = transform.pos;
										hps.time = rightHandTimer.load();

										currentRightHandPos.push_back(hps);
										LOG_INFO("Right: %g %g %g ---- %g %g %g Time: %g", point.x, point.y, point.z, transform.pos.x, transform.pos.y, transform.pos.z, hps.time);
									}
									else
									{
										LOG("rightWandNode not found");
									}

									if (currentRightHandPos.size() % 10 == 0) {
										if (!(leftHandedMode && LeftButtonHold.load())) {
											Orientation rightHandAttackOrientation = VerticalDown;
											SpinningType spinningType = Right;

											//rightHandPos.clear();
											//rightHandPos.swap(currentRightHandPos);
											//currentRightHandPos.clear();

											rightHandAttackOrientation = GetOrientationFromPoints(currentRightHandPos, spinningType);
											powerAttackControlValueGlobal->value = (float)rightHandAttackOrientation;
										}
									}
								}
							}
							else
							{
								rightHandTimer.store(0);
							}
						}
						else
						{
							currentRightHandPos.clear();
							rightHandTimer.store(0);
						}
					}
					else
					{
						currentRightHandPos.clear();
						rightHandTimer.store(0);
					}

					if ((int)leftController >= 0)
					{
						if (pGamePoseArray[leftController].bDeviceIsConnected && pGamePoseArray[leftController].bPoseIsValid && pGamePoseArray[leftController].eTrackingResult == vr::TrackingResult_Running_OK)
						{
							if (LeftButtonHold.load())
							{
								leftHandTimer.store(m_frameTime + leftHandTimer.load());
								const auto playerRefr = DYNAMIC_CAST((*g_thePlayer), Actor, TESObjectREFR);

								if (playerRefr && playerRefr->loadedState)
								{
									auto& nodeList = (*g_thePlayer)->nodeList;

									NiAVObject* hnode = nodeList[leftWandNode];

									if (hnode)
									{
										const NiPoint3 point = hnode->m_worldTransform.pos - playerRefr->pos;
										HandPosition hps;
										hps.pos = point;
										PapyrusVR::Matrix34 matrix = GetMatrix(pGamePoseArray[leftController].mDeviceToAbsoluteTracking);
										NiTransform transform;
										PapyrusVR::OpenVRUtils::CopyMatrix34ToNiTrasform(&matrix, &transform);
										hps.rot = transform.rot;
										hps.pos2 = transform.pos;
										hps.time = leftHandTimer.load();

										currentLeftHandPos.push_back(hps);
										LOG_INFO("Left: %g %g %g ---- %g %g %g Time: %g", point.x, point.y, point.z, transform.pos.x, transform.pos.y, transform.pos.z, hps.time);
									}
									else
									{
										LOG("LeftWandNode not found");
									}

									if (currentLeftHandPos.size() % 10 == 0) {
										if (!(!leftHandedMode && RightButtonHold.load())) {
											Orientation leftHandAttackOrientation = VerticalDown;
											SpinningType spinningType = Right;

											//leftHandPos.clear();
											//leftHandPos.swap(currentLeftHandPos);
										//	currentLeftHandPos.clear();

											leftHandAttackOrientation = GetOrientationFromPoints(currentLeftHandPos, spinningType);
											powerAttackControlValueGlobal->value = (float)leftHandAttackOrientation;
										}
									}
								}
							}
							else
							{
								leftHandTimer.store(0);
							}
						}
						else
						{
							currentLeftHandPos.clear();
							leftHandTimer.store(0);
						}
					}
					else
					{
						currentLeftHandPos.clear();
						leftHandTimer.store(0);
					}
				}
				else
				{
					currentRightHandPos.clear();
					currentLeftHandPos.clear();
					leftHandTimer.store(0);
					rightHandTimer.store(0);
				}
			}
		}
		return vr::EVRCompositorError::VRCompositorError_None;
	}


	///This function is used to detect controller button presses and call the ThrowWeapon function.
	bool cbFunc(vr::TrackedDeviceIndex_t unControllerDeviceIndex, const vr::VRControllerState_t* pControllerState, uint32_t unControllerStateSize, vr::VRControllerState_t* pOutputControllerState)
	{
		if (!isGameStopped())
		{
			if (ivrSystem)
			{
				if ((*g_thePlayer) && (*g_thePlayer)->loadedState && (*g_thePlayer)->loadedState->node)
				{
					const vr::TrackedDeviceIndex_t rightController = hookManager->GetVRSystem()->GetTrackedDeviceIndexForControllerRole(rightControllerRole);
					const vr::TrackedDeviceIndex_t leftController = hookManager->GetVRSystem()->GetTrackedDeviceIndexForControllerRole(leftControllerRole);

					if ((*g_thePlayer)->loadedState && (*g_thePlayer)->loadedState->node)
					{
						const NiPoint3 playerPos = (*g_thePlayer)->loadedState->node->m_worldTransform.pos;
						if (unControllerDeviceIndex == rightController)
						{
							if (pControllerState)
							{
								if ((pControllerState->ulButtonPressed & buttonMask) && !RightButtonGeneralHold.load())
								{
									LOG_INFO("Right Button is pressed right now.");
									RightButtonGeneralHold.store(true);
								}
								else if (((releaseWithNoTouch && !(pControllerState->ulButtonTouched & buttonMask)) || (!releaseWithNoTouch && !(pControllerState->ulButtonPressed & buttonMask))) && RightButtonGeneralHold.load())
								{
									LOG_INFO("Right Button is let go.");
									RightButtonGeneralHold.store(false);
								}

								//////////////////////////////////////

								if ((pControllerState->ulButtonPressed & buttonMask) && !RightButtonHold.load())
								{
									RightButtonHold.store(true);

								}
								else if (((releaseWithNoTouch && !(pControllerState->ulButtonTouched & buttonMask)) || (!releaseWithNoTouch && !(pControllerState->ulButtonPressed & buttonMask))) && RightButtonHold.load())
								{
									RightButtonHold.store(false);

									rightHandPos.clear();
									currentRightHandPos.clear();
								}
							}
						}
						else if (unControllerDeviceIndex == leftController)
						{
							if (pControllerState)
							{
								if ((pControllerState->ulButtonPressed & buttonMaskSecondary) && !LeftButtonGeneralHold.load())
								{
									LOG_INFO("Left Button is pressed right now.");
									LeftButtonGeneralHold.store(true);
								}
								else if (((releaseWithNoTouch && !(pControllerState->ulButtonTouched & buttonMaskSecondary)) || (!releaseWithNoTouch && !(pControllerState->ulButtonPressed & buttonMaskSecondary))) && LeftButtonGeneralHold.load())
								{
									LOG_INFO("Left Button is let go.");
									LeftButtonGeneralHold.store(false);
								}

								///////////////////////////////////////

								if ((pControllerState->ulButtonPressed & buttonMaskSecondary) && !LeftButtonHold.load())
								{
									LeftButtonHold.store(true);

								}
								else if (((releaseWithNoTouch && !(pControllerState->ulButtonTouched & buttonMaskSecondary)) || (!releaseWithNoTouch && !(pControllerState->ulButtonPressed & buttonMaskSecondary))) && LeftButtonHold.load())
								{
									LeftButtonHold.store(false);

									leftHandPos.clear();
									currentLeftHandPos.clear();
								}
							}
						}
					}
				}
			}
		}

		return true;
	}

	///This is the function to register SkyrimVRTools callback
	void RegisterCallback()
	{
		if (hookManager->IsInitialized())
		{
			hookManager = RequestOpenVRHookManagerObject();
			if (hookManager)
			{
				InitSystem(hookManager->GetVRSystem());

				hookManager->RegisterGetPosesCB(cbFuncGetPoses);
				hookManager->RegisterControllerStateCB(cbFunc);
			}
		}
	}

	void LeftHandedModeChange()
	{
		const int value = vlibGetSetting("bLeftHandedMode:VRInput");
		if (value != leftHandedMode)
		{
			leftHandedMode = value;
			LOG_ERR("Left Handed Mode is %s.", leftHandedMode ? "ON" : "OFF");
			
		}
	}

	void ButtonMaskChange()
	{
		buttonMask = GetButtonMaskFromId(HoldButton);
		buttonMaskSecondary = GetButtonMaskFromId(HoldButtonSecondary);
	}

	void StartMod()
	{

		ButtonMaskChange();

		RegisterCallback();

		QueryPerformanceFrequency(&m_hpcFrequency);
		QueryPerformanceCounter(&m_prevTime);
	}

	void AfterGameLoad()
	{
		LeftHandedModeChange();
	}
}
