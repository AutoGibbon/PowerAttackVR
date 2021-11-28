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

	OriginalAttackBlockHandler g_originalAttackBlockHandler = nullptr;
		
	const uintptr_t AttackBlockHandlerVtbl_Offset = 0x016f2258;

	RelocPtr<OriginalAttackBlockHandler> OriginalAttackBlockHandler_vtbl(AttackBlockHandlerVtbl_Offset + 0x04);

	uint64_t buttonMask = 0;
	uint64_t buttonMaskSecondary = 0;

	std::atomic<bool> LeftButtonHold;
	std::atomic<bool> RightButtonHold;

	std::atomic<bool>* mainButtonHold;
	std::atomic<bool>* secondaryButtonHold;

	std::atomic<bool> LeftButtonGeneralHold;
	std::atomic<bool> RightButtonGeneralHold;

	std::atomic<bool>* mainButtonGeneralHold;
	std::atomic<bool>* secondaryButtonGeneralHold;

	std::atomic<NiPoint3> mainLastPosition;
	std::atomic<NiPoint3> secondaryLastPosition;
		
	//Default left-right controller variables. For left handed support to be changed later if needed.
	vr::ETrackedControllerRole leftControllerRole = vr::ETrackedControllerRole::TrackedControllerRole_LeftHand;
	vr::ETrackedControllerRole rightControllerRole = vr::ETrackedControllerRole::TrackedControllerRole_RightHand;

	//Openvr system variable
	vr::IVRSystem* ivrSystem = nullptr;

	//SkyrimVRTools hookManagerApi variable
	OpenVRHookManagerAPI* hookManager;	

	NiPoint3 playerLastPos;
	NiPoint3 playerLastSpeed;

	std::atomic<float> leftLastSpeed;
	std::atomic<float> rightLastSpeed;

	std::atomic<float>* mainLastSpeed;
	std::atomic<float>* secondaryLastSpeed;

	std::atomic<float> leftLastProjSpeed;
	std::atomic<float> rightLastProjSpeed;

	std::atomic<float>* mainLastProjSpeed;
	std::atomic<float>* secondaryLastProjSpeed;

	std::deque<HandPosStamina> currentRightHandPos;
	std::deque<HandPosStamina> currentLeftHandPos;

	std::deque<HandPosStamina>* currentMainHandPos;
	std::deque<HandPosStamina>* currentSecondaryHandPos;

	std::deque<HandPosStamina> rightHandPos;
	std::deque<HandPosStamina> leftHandPos;

	std::deque<HandPosStamina>* mainHandPos;
	std::deque<HandPosStamina>* secondaryHandPos;
	
	///////////////////////////

	PlayerCharacter::Node leftWandNode = PlayerCharacter::kNode_LeftMeleeWeaponOffsetNode;
	PlayerCharacter::Node rightWandNode = PlayerCharacter::kNode_RightMeleeWeaponOffsetNode;

	PlayerCharacter::Node leftRealWandNode = PlayerCharacter::kNode_LeftHandBone;
	PlayerCharacter::Node rightRealWandNode = PlayerCharacter::kNode_RightHandBone;

	BSFixedString leftHandBone("NPC L Hand [LHnd]");
	BSFixedString rightHandBone("NPC R Hand [RHnd]");

	std::atomic<bool> powerAttackHappening;

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

	std::atomic<double> rightHandTimer = 0;
	std::atomic<double> leftHandTimer = 0;
	LARGE_INTEGER lastProjectileCheckTime;
	///This function is only used to detect controller movement and save positions into a deque.
	vr::EVRCompositorError cbFuncGetPoses(VR_ARRAY_COUNT(unRenderPoseArrayCount) vr::TrackedDevicePose_t* pRenderPoseArray, uint32_t unRenderPoseArrayCount, VR_ARRAY_COUNT(unGamePoseArrayCount) vr::TrackedDevicePose_t* pGamePoseArray, uint32_t unGamePoseArrayCount)
	{
		LARGE_INTEGER newTime;
		QueryPerformanceCounter(&newTime);
		m_frameTime = (double)(newTime.QuadPart - m_prevTime.QuadPart) / m_hpcFrequency.QuadPart;
		if (m_frameTime > 0.05) {
			m_frameTime = 0.05;
		}
		m_prevTime = newTime;
		
		if (!isGameStopped())
		{
			if (ivrSystem)
			{
				if ((double)(m_prevTime.QuadPart - lastProjectileCheckTime.QuadPart) * 1000.0 / (double)m_hpcFrequency.QuadPart >= m_frameTime*100)
				{
					lastProjectileCheckTime = m_prevTime;
				}
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
									auto & nodeList = (*g_thePlayer)->nodeList;

									NiAVObject* hnode = nodeList[rightWandNode];

									if (hnode)
									{
										const NiPoint3 point = hnode->m_worldTransform.pos - playerRefr->pos;
										HandPosStamina hps;
										hps.pos = point;
										PapyrusVR::Matrix34 matrix = GetMatrix(pGamePoseArray[rightController].mDeviceToAbsoluteTracking);
										NiTransform transform;
										PapyrusVR::OpenVRUtils::CopyMatrix34ToNiTrasform(&matrix, &transform);
										hps.rot = transform.rot;
										hps.time = rightHandTimer.load();										
										
										currentRightHandPos.push_back(hps);
										LOG_INFO("Right: %g %g %g  Time: %g", point.x, point.y, point.z, hps.time);
									}
									else
									{
										LOG("rightWandNode not found");
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
									auto & nodeList = (*g_thePlayer)->nodeList;

									NiAVObject* hnode = nodeList[leftWandNode];

									if (hnode)
									{
										const NiPoint3 point = hnode->m_worldTransform.pos - playerRefr->pos;
										HandPosStamina hps;
										hps.pos = point;
										PapyrusVR::Matrix34 matrix = GetMatrix(pGamePoseArray[leftController].mDeviceToAbsoluteTracking);
										NiTransform transform;
										PapyrusVR::OpenVRUtils::CopyMatrix34ToNiTrasform(&matrix, &transform);
										hps.rot = transform.rot;
										hps.time = leftHandTimer.load();

										currentLeftHandPos.push_back(hps);
										LOG_INFO("Left: %g %g %g  Time: %g", point.x, point.y, point.z, hps.time);
									}
									else
									{
										LOG("LeftWandNode not found");
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
		else
		{
			lastProjectileCheckTime = m_prevTime;
		}
		return vr::EVRCompositorError::VRCompositorError_None;
	}

	Orientation GetOrientationFromPoints(const std::deque<HandPosStamina>& deque, SpinningType &outSpinningType)
	{
		//triangle
		NiPoint3 normal;
		std::deque<NiPoint3> heightPoints;
		
		for(int i=0; i<deque.size()-1; i++)
		{
			normal = normal + crossProduct(deque[i].pos,deque[i+1].pos);

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
									
									//if (RightButtonHold.load())
									//{
									//	pOutputControllerState->ulButtonPressed &= ~buttonMask;
									//}
								}
								else if (((releaseWithNoTouch && !(pControllerState->ulButtonTouched & buttonMask)) || (!releaseWithNoTouch && !(pControllerState->ulButtonPressed & buttonMask))) && RightButtonHold.load())
								{
									RightButtonHold.store(false);

									rightHandPos.clear();
									rightHandPos.swap(currentRightHandPos);
									currentRightHandPos.clear();

									if (rightHandPos.size() > 2)
									{
										const float minTravelDist = 1.0f;
										const float minThrowSpeed = 1.0f;

										float FirstStamina = rightHandPos.front().stamina;
										float LastStamina = rightHandPos.back().stamina;
										double endTime = rightHandPos.back().time;
										if (minTravelDist > 0 && minThrowSpeed>0)
										{											
											std::deque<HandPosStamina> tempDeque;

											int cutOffIndexEnd = 0;
											int cutOffIndexStart = 0;
											bool endFound = false;
											for (int i = rightHandPos.size() - 2; i >= 0; i--)
											{
												if (!endFound)
												{
													if (distanceNoSqrt(rightHandPos[i + 1].pos, rightHandPos[i].pos) / (rightHandPos[i + 1].time - rightHandPos[i].time) >= 32.0f)
													{
														cutOffIndexEnd = i + 1;
														endFound = true;
													}
												}
												else
												{
													if (distanceNoSqrt(rightHandPos[i + 1].pos, rightHandPos[i].pos) / (rightHandPos[i + 1].time - rightHandPos[i].time) < 32.0f)
													{
														cutOffIndexStart = i + 1;
														break;
													}
												}
											}

											for (int i = cutOffIndexStart; i<cutOffIndexEnd; i++)
											{
												tempDeque.push_back(rightHandPos[i]);
											}
											tempDeque.swap(rightHandPos);
											tempDeque.clear();
											
											float posDist = 0;
											float posVelocity = 0;
											Orientation rightHandThrowOrientation = VerticalDown;
											SpinningType spinningType = Right;
											
											if (rightHandPos.size() > 3)
											{
												/*LOG("------These are the points selected:----");
												for (int i = 0; i < rightHandPos.size(); i++)
												{
													LOG("Selected: %g %g %g  Time: %g", rightHandPos[i].pos.x, rightHandPos[i].pos.y, rightHandPos[i].pos.z, rightHandPos[i].time);
												}*/
												
												if (endTime - rightHandPos.back().time < 0.3 || rightHandPos.front().time > 1.0f)
												{
													posDist = sqrtf(distanceNoSqrt(rightHandPos.front().pos, rightHandPos.back().pos));
													posVelocity = (posDist / (rightHandPos.back().time - rightHandPos.front().time))*0.7f;
													rightHandThrowOrientation = GetOrientationFromPoints(rightHandPos, spinningType);
												}
												else
												{
													LOG("Right: I think you are trying to do a power attack, so I'm not throwing. Because trimmed the end by: %g", endTime - rightHandPos.back().time);
												}
												LOG("Right Pos TravelDist: %g - minTravelDist: %g - ThrowSpeed: %g - minThrowSpeed: %g - First stamina: %g Last Stamina: %g", posDist, minTravelDist, posVelocity, minThrowSpeed, FirstStamina, LastStamina);
											}


											if (posDist >= minTravelDist && posVelocity >= minThrowSpeed && rightHandPos.size() > 3)
											{
												
												playerLastSpeed = playerPos - playerLastPos;

												float calculatedStaminaCost = 0.0f;

                                                //TODO: entry point for power attack
												/*std::thread t6(ThrowWeapon, leftHandedMode ? true : false, (posVelocity / minThrowSpeed), rightHandThrowOrientation, spinningType);
												t6.detach();
												std::thread t7(HandHapticFeedbackEffect, true, leftHandedMode ? true : false);
												t7.detach();*/
													
											
											}
										}
									}
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
									
									/*if (LeftButtonHold.load())
									{
										pOutputControllerState->ulButtonPressed &= ~buttonMaskSecondary;
									}*/
								}
								else if (((releaseWithNoTouch && !(pControllerState->ulButtonTouched & buttonMaskSecondary)) || (!releaseWithNoTouch && !(pControllerState->ulButtonPressed & buttonMaskSecondary))) && LeftButtonHold.load())
								{
									LeftButtonHold.store(false);

									leftHandPos.clear();
									leftHandPos.swap(currentLeftHandPos);
									currentLeftHandPos.clear();

									if (leftHandPos.size() > 2)
									{
										const float minTravelDist = 1.0f;
										const float minThrowSpeed = 1.0f;

										float FirstStamina = leftHandPos.front().stamina;
										float LastStamina = leftHandPos.back().stamina;
										double endTime = leftHandPos.back().time;

										if (minTravelDist > 0 && minThrowSpeed > 0)
										{
											std::deque<HandPosStamina> tempDeque;

											int cutOffIndexEnd = 0;
											int cutOffIndexStart = 0;
											bool endFound = false;
											for (int i = leftHandPos.size() - 2; i >= 0; i--)
											{
												if (!endFound)
												{
													if (distanceNoSqrt(leftHandPos[i + 1].pos, leftHandPos[i].pos) / (leftHandPos[i + 1].time - leftHandPos[i].time) >= 32.0f)
													{
														cutOffIndexEnd = i + 1;
														endFound = true;
													}
												}
												else
												{
													if (distanceNoSqrt(leftHandPos[i + 1].pos, leftHandPos[i].pos) / (leftHandPos[i + 1].time - leftHandPos[i].time) < 32.0f)
													{
														cutOffIndexStart = i + 1;
														break;
													}
												}
											}

											for (int i = cutOffIndexStart; i < cutOffIndexEnd; i++)
											{
												tempDeque.push_back(leftHandPos[i]);
											}
											tempDeque.swap(leftHandPos);
											tempDeque.clear();

											float posDist = 0;
											float posVelocity = 0;
											Orientation leftHandThrowOrientation = VerticalDown;
											SpinningType spinningType = Left;
											
											if (leftHandPos.size() > 3)
											{
												if (endTime - leftHandPos.back().time < 0.3f || leftHandPos.front().time > 1.0f)
												{
													posDist = sqrtf(distanceNoSqrt(leftHandPos.front().pos, leftHandPos.back().pos));
													posVelocity = posDist / (leftHandPos.back().time - leftHandPos.front().time);
													leftHandThrowOrientation = GetOrientationFromPoints(leftHandPos, spinningType);
												}
												else
												{
													LOG("Left: I think you are trying to do a power attack, so I'm not throwing.");
												}
												LOG("Left Pos TravelDist: %g - minTravelDist: %g - ThrowSpeed: %g - minThrowSpeed: %g - First stamina: %g Last Stamina: %g", posDist, minTravelDist, posVelocity, minThrowSpeed, FirstStamina, LastStamina);
											}
											if (posDist >= minTravelDist && posVelocity >= minThrowSpeed && leftHandPos.size() > 3)
											{
												/*std::thread t6(ThrowWeapon, leftHandedMode ? false : true, (posVelocity / minThrowSpeed), leftHandThrowOrientation, spinningType);
												t6.detach();
												std::thread t7(HandHapticFeedbackEffect, true, leftHandedMode ? false : true);
												t7.detach();*/
											}
										}
									}
								}
							}
						}
						playerLastPos = playerPos;
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

	void InitializePointers()
	{
		mainButtonHold = leftHandedMode ? &LeftButtonHold : &RightButtonHold;
		secondaryButtonHold = leftHandedMode ? &RightButtonHold : &LeftButtonHold;

		mainButtonGeneralHold = leftHandedMode ? &LeftButtonGeneralHold : &RightButtonGeneralHold;
		secondaryButtonGeneralHold = leftHandedMode ? &RightButtonGeneralHold : &LeftButtonGeneralHold;

		currentMainHandPos = leftHandedMode ? &currentLeftHandPos : &currentRightHandPos;
		currentSecondaryHandPos = leftHandedMode ? &currentRightHandPos : &currentLeftHandPos;

		mainHandPos = leftHandedMode ? &leftHandPos : &rightHandPos;
		secondaryHandPos = leftHandedMode ? &rightHandPos : &leftHandPos;

		if (mainButtonHold != nullptr)
			mainButtonHold->store(false);

		if (secondaryButtonHold != nullptr)
			secondaryButtonHold->store(false);

		if (mainButtonGeneralHold != nullptr)
			mainButtonGeneralHold->store(false);

		if (secondaryButtonGeneralHold != nullptr)
			secondaryButtonGeneralHold->store(false);
	}

	void LeftHandedModeChange()
	{
		const int value = vlibGetSetting("bLeftHandedMode:VRInput");
		if (value != leftHandedMode)
		{
			leftHandedMode = value;
			LOG_ERR("Left Handed Mode is %s.", leftHandedMode ? "ON" : "OFF");
			InitializePointers();
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

	void Hooks()
	{
		/*g_originalAttackBlockHandler = *OriginalAttackBlockHandler_vtbl;
		_MESSAGE("Hooking function attack block handler: %p", (uintptr_t)g_originalAttackBlockHandler - uintptr_t(GetModuleHandle(NULL)));
		SafeWrite64(OriginalAttackBlockHandler_vtbl.GetUIntPtr(), GetFnAddr(&AttackBlockHandler_Hook));*/
	}

	void AttackBlockHandler_Hook(TESObjectREFR* ref, NiPoint3* velocity)
	{
		LOG("ATTACK BLOCK HANDLE");
		g_originalAttackBlockHandler(ref, velocity);
	}

	void AfterGameLoad()
	{
		LeftHandedModeChange();
	}

	//SKSEActionEvent stuff
	SKSEActionEventHandler g_SKSEActionEventHandler;

	EventResult SKSEActionEventHandler::ReceiveEvent(SKSEActionEvent* evn, EventDispatcher<SKSEActionEvent>* dispatcher) 
	{
		if (evn) {
			if (evn->type == evn->kType_WeaponSwing) {
				LOG("Got Weapon Swing Event! Setting attack direction to 'backwards'");
				powerAttackControlValueGlobal->value = (float)AttackDirection::AttackBackward;

				if (evn->slot == evn->kSlot_Left) {
					Orientation leftHandAttackOrientation = VerticalDown;
					SpinningType spinningType = Right;

					leftHandPos.clear();
					leftHandPos.swap(currentLeftHandPos);
					currentLeftHandPos.clear();

					leftHandAttackOrientation = GetOrientationFromPoints(leftHandPos, spinningType);
					powerAttackControlValueGlobal->value = (float)leftHandAttackOrientation;
				}
				else if (evn->slot == evn->kSlot_Right) {
					Orientation rightHandAttackOrientation = VerticalDown;
					SpinningType spinningType = Right;

					rightHandPos.clear();
					rightHandPos.swap(currentRightHandPos);
					currentRightHandPos.clear();

					rightHandAttackOrientation = GetOrientationFromPoints(rightHandPos, spinningType);
					powerAttackControlValueGlobal->value = (float)rightHandAttackOrientation;
				}
				/*
				* figure out which hand is swinging - exmaine slot in
				* 
									leftHandPos.clear();
									leftHandPos.swap(currentLeftHandPos);
									currentLeftHandPos.clear();
				
				*/
			}
		}
		
		return EventResult::kEvent_Continue;
	}

	//HitEvent variables
	EventDispatcher<TESHitEvent> *g_HitEventDispatcher;
	HitEventHandler g_HitEventHandler;

	EventResult HitEventHandler::ReceiveEvent(TESHitEvent* evn, EventDispatcher<TESHitEvent>* dispatcher)
	{
		return EventResult::kEvent_Continue;
		if (evn)
		{
			if (evn->caster && evn->caster->formID == 0x14)
			{
				//LOG("HitEvent - caster:%x, projectile:%x, source:%x, target:%x, Attack: %s", evn->caster ? evn->caster->formID : 0, evn->projectileFormID, evn->sourceFormID, evn->target ? evn->target->formID : 0, (evn->flags & evn->kFlag_PowerAttack) ? "Power Attack" : "Normal Attack");
				if ((evn->flags & evn->kFlag_PowerAttack)) //If it was a power attack by the player that hit someone
				{
					LOG("Power Attack Happening.");
					//std::thread t4(SetPowerAttackHappening);
					//t4.detach();
				}
			}
		}

		return EventResult::kEvent_Continue;
	}

	
	void HitEventFunc(bool secondaryHand, Actor * targetActor, TESObjectREFR* targetRefr)
	{
		LOG("attack direction at time of hit event:  %g", powerAttackControlValueGlobal->value);
	}
}
