#include "Core.h"
#include "ActorTemplateData.h"
#include "Resource/ResourceManager.h"
#include "Resource/ResourceGroupManager.h"
#include "Image/Color.h"
#include "Anim/Skeleton.h"
#include "Anim/Bone.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#pragma warning(disable:4996)

namespace LORD
{

	ActorTemplateData::ActorTemplateData(void)
		: OriginalScale(1.0f, 1.0f, 1.0f)
		, mAttackScale(1.0f)
		, mSelectedRingSize(1.0f)
		, mTextHeight(0.0f)
		, mbShowShadow(true)
		, mbInstanceCreate(false)
		, mTextHeightOriginal(0.0f)
		, mMainRootBonePosition(0.0f, 0.0f, 0.0f)
		, mWoundAngle(25.0f)
		, mfAlpha(1.0f)
	{
		mWoundSound.init();
	}

	//---------------------------------------------------------------------
	ActorTemplateData::~ActorTemplateData(void)
	{
		Free();
	}

	//---------------------------------------------------------------------
	ActorTemplateData::BodyPart::~BodyPart()
	{
		vector<Skin*>::type::iterator itSkin;
		for(itSkin = Skins.begin(); itSkin != Skins.end(); ++itSkin)
		{
			LORD_DELETE_T(*itSkin, Skin);
		}
		Skins.clear();

		vector<ActorEffect*>::type::iterator itEffect;
		for(itEffect = ActorEffects.begin(); itEffect != ActorEffects.end(); ++itEffect)
		{
			LORD_DELETE_T(*itEffect, ActorEffect);
		}
		ActorEffects.clear();

		vector<StaticMesh*>::type::iterator itMesh;
		for(itMesh = StaticMeshs.begin(); itMesh != StaticMeshs.end(); ++itMesh)
		{
			LORD_DELETE_T(*itMesh, StaticMesh);
		}
		StaticMeshs.clear();
	}
	//---------------------------------------------------------------------
	ActorTemplateData::Skill::~Skill()
	{
		vector<ActorEffect*>::type::iterator itEffect;
		for(itEffect = ActorEffects.begin(); itEffect != ActorEffects.end(); ++itEffect)
		{
			LORD_DELETE_T(*itEffect, ActorEffect);
		}
		ActorEffects.clear();

		vector<Sound*>::type::iterator itSound;
		for(itSound = Sounds.begin(); itSound != Sounds.end(); ++itSound)
		{
			LORD_DELETE_T(*itSound, Sound);
		}
		Sounds.clear();

		vector<ColorChange*>::type::iterator itColorChange;
		for(itColorChange = ColorChanges.begin(); itColorChange != ColorChanges.end(); ++itColorChange)
		{
			LORD_DELETE_T(*itColorChange, ColorChange);
		}
		ColorChanges.clear();

		vector<SlowDown*>::type::iterator itSlowDown;
		for(itSlowDown = SlowDowns.begin(); itSlowDown != SlowDowns.end(); ++itSlowDown)
		{
			LORD_DELETE_T(*itSlowDown, SlowDown);
		}
		SlowDowns.clear();

		vector<HitFly*>::type::iterator itHitFly;
		for(itHitFly = HitFlys.begin(); itHitFly != HitFlys.end(); ++itHitFly)
		{
			LORD_DELETE_T(*itHitFly, HitFly);
		}
		HitFlys.clear();

		vector<SkillBreak*>::type::iterator itSkillBreak;
		for(itSkillBreak = SkillBreaks.begin(); itSkillBreak != SkillBreaks.end(); ++itSkillBreak)
		{
			LORD_DELETE_T(*itSkillBreak, SkillBreak);
		}
		SkillBreaks.clear();

		vector<CameraPush*>::type::iterator itCameraPush;
		for(itCameraPush = CameraPushs.begin(); itCameraPush != CameraPushs.end(); ++itCameraPush)
		{
			LORD_DELETE_T(*itCameraPush, CameraPush);
		}
		CameraPushs.clear();

		vector<CameraShake*>::type::iterator itCameraShake;
		for(itCameraShake = CameraShakes.begin(); itCameraShake != CameraShakes.end(); ++itCameraShake)
		{
			LORD_DELETE_T(*itCameraShake, CameraShake);
		}
		CameraShakes.clear();

		vector<CameraAnim*>::type::iterator itCameraAnim;
		for (itCameraAnim = CameraAnims.begin(); itCameraAnim != CameraAnims.end(); ++itCameraAnim)
		{
			LORD_DELETE_T(*itCameraAnim, CameraAnim);
		}
		CameraAnims.clear();

		// ???≧??÷??
		map<String, AnimationInfo*>::type::iterator itAnimation;
		for(itAnimation = Animations.begin(); itAnimation != Animations.end(); ++itAnimation)
		{
			LORD_DELETE_T(itAnimation->second, AnimationInfo);
		}
		Animations.clear();

		// 羽?????????÷?
		vector<AttackEvent*>::type::iterator itAttackEvent;
		for(itAttackEvent = AttackEvents.begin(); itAttackEvent != AttackEvents.end(); ++itAttackEvent)
		{
			LORD_DELETE_T(*itAttackEvent, AttackEvent);
		}
		AttackEvents.clear();

		vector<ChangeDark*>::type::iterator itDarks;
		for (itDarks = ChangeDarks.begin(); itDarks != ChangeDarks.end(); ++itDarks)
		{
			LORD_DELETE_T(*itDarks, ChangeDark);
		}
		ChangeDarks.clear();

		if(pSetScale)
		{
			LORD_DELETE_T(pSetScale,Vector2);
			pSetScale = NULL;
		}
	}
	//---------------------------------------------------------------------
	void ActorTemplateData::Free()
	{
		// ??﹉＞BodyPart
		map<String, BodyPart*>::type::iterator itBodyPart;
		for(itBodyPart = BodyPartsMap.begin(); itBodyPart != BodyPartsMap.end(); ++itBodyPart)
		{
			LORD_DELETE_T(itBodyPart->second, BodyPart);
		}
		BodyPartsMap.clear();
		ChildBodyPartsMap.clear();

		// ??﹉＞Skill
		map<String, Skill*>::type::iterator itSkill;
		for(itSkill = SkillsMap.begin(); itSkill != SkillsMap.end(); ++itSkill)
		{
			LORD_DELETE_T(itSkill->second, Skill);
		}
		SkillsMap.clear();

		AnimsList.clear();
		// ??﹉＞＃?﹢?＃?羽???
		mUpperBodyBonesName.clear();
		m_UpperBoneIndices.clear();
		m_LowerBoneIndices.clear();

		// ??﹉＞﹊次?§每?羽?
		list<ActorEffect*>::type::iterator itAppendEffect;
		for(itAppendEffect = mAppendEffects.begin(); itAppendEffect != mAppendEffects.end(); ++itAppendEffect)
		{
			LORD_DELETE_T(*itAppendEffect, ActorEffect);
		}
		mAppendEffects.clear();

		// ??﹉＞?㏑???????
		TemplateName.clear();
		ParentTemplateName.clear();
		OriginalScale = Vector3(1.0f, 1.0f, 1.0f);
		mAttackScale = 1.0f;
		mSelectedRingSize = 1.0f;

		mTextHeight = 0.0f;
		mbInstanceCreate = false;
		mTextHeightOriginal = 0.0f;
		mMainRootBoneName.clear();
		mWoundBoneName.clear();
		mWoundAngle = 25.0f;
		mWoundSound.init();
	}

	//---------------------------------------------------------------------
	void ActorTemplateData::setUpperBodyBonesName(const vector<String>::type& namelist)
	{
		mUpperBodyBonesName.clear();
		for(size_t i=0; i<namelist.size(); ++i)
		{
			mUpperBodyBonesName.insert(namelist[i]);
		}
	}

	//---------------------------------------------------------------------
	void ActorTemplateData::GetMeshNames(const String& masterName,const String &slaveName, StringArray& retMeshNames) const
	{
		retMeshNames.clear();

		String Key = masterName + LORD::String(".") + slaveName;;
		map<String, BodyPart*>::type::const_iterator it = BodyPartsMap.find(Key);
		if(it != BodyPartsMap.end())
		{
			vector<Skin*>::type& Skins = it->second->Skins;
			size_t Num = Skins.size();
			for(size_t i = 0; i < Num; ++i)
			{
				retMeshNames.push_back(Skins[i]->MeshName);
			}
		}
	}

	//---------------------------------------------------------------------
	void ActorTemplateData::InitBySkeleton(Skeleton* pSkel)
	{
		mMainRootBoneName = "";
		Bone* pBone = pSkel->getBoneById((int)0);
		if (pBone)
		{
			if (!pBone->getParent())
			{
				mMainRootBoneName = pBone->getName();
			}

			if (!mMainRootBoneName.empty())
			{
				mMainRootBonePosition = pBone->getPosition();
			}
		}
	}
	//---------------------------------------------------------------------
	bool ActorTemplateData::LoadActorFile(const String& FileName)
	{
		try 
		{
			Free();

			DataStream* pXMLStream = ResourceGroupManager::Instance()->openResource(FileName);
			if(pXMLStream == NULL)
			{
				LordLogError("Actor file not found [%s].", FileName.c_str());
				return NULL;
			}

			size_t fileSize = pXMLStream->size();
			char* xmlData = (char*)LordMalloc(fileSize + 1);
			pXMLStream->read(xmlData, fileSize);

			xmlData[fileSize] = 0;

			rapidxml::xml_document<>* doc = LORD_NEW_T(rapidxml::xml_document<>);        // character type defaults to cha
			doc->parse<0>(xmlData);

			rapidxml::xml_node<> *pRootNode = doc->first_node();
			if(!pRootNode)
			{
				LordLogError("The Actor file content is valid.");
				throw 0;
			}


			bool ret = ActorTemplateData::Load(pRootNode->first_node());
			bool retParent = true;
			if (!ParentTemplateName.empty())
			{
				retParent = LoadParentTemplateFile(ParentTemplateName);
			}

			LORD_DELETE_T(doc, xml_document<>);
			LordFree(xmlData);
			LordDelete pXMLStream;

			return ret && retParent;
		}
		catch(bool) 
		{
			Free();
			LordLogError("ActorTemplateData::LoadActorFile: Fail to Parse actor file [%s].", FileName.c_str());
			return false;
		}

		return false;
	}

	bool ActorTemplateData::LoadParentTemplateFile(const String& FileName)
	{
		try
		{
			map<String, Skill*>::type::iterator itSkill;
			for (itSkill = SkillsMap.begin(); itSkill != SkillsMap.end(); ++itSkill)
			{
				LORD_DELETE_T(itSkill->second, Skill);
			}
			SkillsMap.clear();
			AnimsList.clear();

			DataStream* pXMLStream = ResourceGroupManager::Instance()->openResource(FileName);
			if (pXMLStream == NULL)
			{
				LordLogError("Actor file not found [%s].", FileName.c_str());
				return NULL;
			}

			size_t fileSize = pXMLStream->size();
			char* xmlData = (char*)LordMalloc(fileSize + 1);
			pXMLStream->read(xmlData, fileSize);

			xmlData[fileSize] = 0;

			rapidxml::xml_document<>* doc = LORD_NEW_T(rapidxml::xml_document<>);        // character type defaults to cha
			doc->parse<0>(xmlData);

			rapidxml::xml_node<> *pRootNode = doc->first_node();
			if (!pRootNode)
			{
				LordLogError("The Actor file content is valid.");
				throw 0;
			}

			bool ret = ActorTemplateData::Load(pRootNode->first_node(), true);

			LORD_DELETE_T(doc, xml_document<>);
			LordFree(xmlData);
			LordDelete pXMLStream;

			return ret;
		}
		catch (bool)
		{
			Free();
			LordLogError("ActorTemplateData::LoadActorFile: Fail to Parse actor file [%s].", FileName.c_str());
			return false;
		}
	}
	//---------------------------------------------------------------------
	bool ActorTemplateData::Load(void* pNode, bool isParentTemplate)
	{
		rapidxml::xml_node<>* pSubNode = static_cast<rapidxml::xml_node<>*>(pNode);
		try
		{
			rapidxml::xml_attribute<>* pVarNode = pSubNode->first_attribute();
			while(pVarNode)
			{
				const char *str = pVarNode->value();
				String strName = pVarNode->name();
				if(strName == "name")
				{
					if (isParentTemplate)
					{
						ParentTemplateName = str;
					}
					else
					{
						TemplateName = str;
					}
				}
				else if (strName == "template")
				{
					if (!isParentTemplate)
					{
						ParentTemplateName = str;
					}
				}
				else if(strName == "Originalscale")
				{
					OriginalScale = StringUtil::ParseVec3(str);
				}
				else if(strName == "Skeleton")
				{
					SkeletonName = str;
				}
				else if(strName == "AttackScale")
				{
					mAttackScale = StringUtil::ParseReal(str);
				}
				else if(strName == "SelectedRingSize")
				{
					mSelectedRingSize = StringUtil::ParseReal(str);
				}
				else if(strName == "TextHeight")
				{
					mTextHeight = StringUtil::ParseReal(str);
				}
				else if(strName == "IsShowShadow")
				{
					mbShowShadow = StringUtil::ParseBool(str);
				}
				else if(strName == "WoundBone")
				{
					mWoundBoneName = str;
				}
				else if(strName == "WoundAngle")
				{
					mWoundAngle = StringUtil::ParseReal(str);
				}
				else if(strName == "SoundName1")
				{
					mWoundSound.soundName1 = str;
				}
				else if(strName == "SoundName2")
				{
					mWoundSound.soundName2 = str;
				}
				else if(strName == "SoundName3")
				{
					mWoundSound.soundName3 = str;
				}
				else if(strName == "SoundName4")
				{
					mWoundSound.soundName4 = str;
				}
				else if(strName == "SoundName5")
				{
					mWoundSound.soundName5 = str;
				}
				else if(strName == "SoundFileNum")
				{
					mWoundSound.soundFileNum = StringUtil::ParseInt(str);
				}
				else if(strName == "Volume")
				{
					mWoundSound.volume = StringUtil::ParseReal(str);
				}
				else if(strName == "MinDistance")
				{
					mWoundSound.minDistance = StringUtil::ParseReal(str);
				}
				else if(strName == "MaxDistance")
				{
					mWoundSound.maxDistance = StringUtil::ParseReal(str);
				}
				else if(strName == "IntervalTime")
				{
					mWoundSound.intervalTime = StringUtil::ParseReal(str);
				}
				else if (strName == "Alpha")
				{
					mfAlpha = StringUtil::ParseReal(str);
				}
				pVarNode = pVarNode->next_attribute();
			}
			rapidxml::xml_node<>* pNode = pSubNode->first_node();

			if (pNode)
			{
				do
				{
					String strName = pNode->name();
					if(strName == "BodyParts")
					{
						if(!LoadBodyParts(pNode, isParentTemplate))
						{
							throw false;
						}
					}
					else if(strName == "Skills")
					{
						if(!LoadSkills(pNode))
						{
							throw false;
						}
					}
					else if(strName == "Anims")
					{
						if(!LoadAnims(pNode))
						{
							throw false;
						}
					}

					/*	else if(strName == _T("BoneGroups"))
					{
					if(!LoadBoneGroups(pNode))
					{
					throw false;
					}
					}*/
					/*else if(strName == _T("AppendEffects"))
					{
					if(!LoadAppendEffects(pNode))
					{
					throw false;
					}
					}*/

					pNode = pNode->next_sibling();
				} 
				while(pNode);
			}

			return true;
		}
		catch(bool)
		{
			Free();
			return false;
		}
	}
	//-----------------------------------------------------------------------
	bool ActorTemplateData::LoadBodyParts(void* pNode, bool isParentTemplate)
	{
		rapidxml::xml_node<>* pSubNode = static_cast<rapidxml::xml_node<>*>(pNode);
		const char *str = NULL;
		try
		{
			rapidxml::xml_node<>* pNode = pSubNode->first_node();
			if(pNode)
			{
				do
				{
					String strName = pNode->name();
					if(strName == "BodyPart")
					{
						rapidxml::xml_attribute<>* pVarNode = pNode->first_attribute();
						BodyPart* pBodyPart = LORD_NEW_T(BodyPart);

						map<String, BodyPart*>::type::const_iterator iter;
						bool isIgnore = false;
						// ??Key每＞??
						while(pVarNode)
						{
							str = pVarNode->value();
							String varName = pVarNode->name();
							if(varName == "SlaveName")
							{
								pBodyPart->SlaveName = str;
								String Key = pBodyPart->MasterName + "." + pBodyPart->SlaveName;
								iter = BodyPartsMap.find(Key);
								if (iter != BodyPartsMap.end())
								{
									LORD_DELETE_T(pBodyPart, BodyPart);
									break;
								}
								if (isParentTemplate)
								{
									if (pBodyPart->MasterName.compare("gun"))
									{
										isIgnore = true;
										LORD_DELETE_T(pBodyPart, BodyPart);
										break;
									}
								}
								// ≒????次Map
								BodyPartsMap.insert(std::pair<String, BodyPart*>(Key, pBodyPart));
								if (!isParentTemplate)
								{
									ChildBodyPartsMap.insert(std::pair<String, BodyPart*>(Key, pBodyPart));
								}
							}
							else if(varName == "DefaultUse")
							{
								pBodyPart->bDefaultUse = StringUtil::ParseBool(str);
							}
							else if(varName == "MasterName")
							{
								pBodyPart->MasterName = str;
							}
							else if(varName == "EdgeColor")
							{
								pBodyPart->m_edgeColor = StringUtil::ParseColor(str);
							}
							else if(varName == "ColorParams")
							{
								pBodyPart->m_colorParams = StringUtil::ParseVec4(str);
							}
							else if (varName == "Group")
							{
								pBodyPart->m_group = str;
							}
							pVarNode = pVarNode->next_attribute();
						}

						if (iter != BodyPartsMap.end() || isIgnore)
						{
							pNode = pNode->next_sibling();
							continue;
						}

						// ?§次????
						rapidxml::xml_node<>* pChildNode = pNode->first_node();
						do
						{
							if(!pChildNode)
								continue;
							String childName = pChildNode->name();
							if(childName == "Skin")
							{
								rapidxml::xml_attribute<>* pSkinElement = pChildNode->first_attribute();
								Skin* pSkin = LORD_NEW_T(Skin);
								pBodyPart->Skins.push_back(pSkin);

								while(pSkinElement)
								{
									str = pSkinElement->value();
									String strSkinName = pSkinElement->name();
									if(strSkinName == "MeshName")
									{
										pSkin->MeshName = str;
									}
									else if(strSkinName == "ChangeColor")
									{
										pSkin->ChangeColor = StringUtil::ParseColor(str);
									}
									else if (strSkinName == "UVSpeed")
									{
										pSkin->UVspeed = StringUtil::ParseVec2(str);
									}
									pSkinElement = pSkinElement->next_attribute();
								}
							}
							else if(childName == "Effect")
							{
								ActorEffect* pEffect = LoadEffect(pChildNode);
								if(pEffect)
								{
									// ?﹢㊣﹊??每?※??＃??0?㊣???????“????〞≧?﹉≒?﹉??
									pEffect->beginTime = 0.0f;
									pEffect->times = -1;
									pBodyPart->ActorEffects.push_back(pEffect);
								}
							}
							else if(childName == "StaticMesh")
							{
								StaticMesh* pMesh = LoadStaticMesh(pChildNode);
								if(pMesh)
								{
									pBodyPart->StaticMeshs.push_back(pMesh);
								}
							}

							pChildNode = pChildNode->next_sibling();
						}
						while(pChildNode);
					}
					pNode = pNode->next_sibling();
				}
				while(pNode);
			}

			return true;
		}
		catch(bool)
		{
			return false;
		}
	}
	//-----------------------------------------------------------------------
	bool ActorTemplateData::LoadSkills(void* pNode)
	{
		rapidxml::xml_node<>* pSubNode = static_cast<rapidxml::xml_node<>*>(pNode);
		const char *str = NULL;
		try
		{
			rapidxml::xml_node<>* pNode = pSubNode->first_node();

			if(pNode)
			{		
				do
				{
					String strName = pNode->name();
					if(strName == "Skill")
					{
						rapidxml::xml_attribute<>* pVarNode = pNode->first_attribute();

						map<String, Skill*>::type::const_iterator iter;
						// ??Key每＞??
						Skill* pSkill = LORD_NEW_T(Skill);
						while(pVarNode)
						{
							str = pVarNode->value();
							String varName = pVarNode->name();
							if(varName == "Name")
							{
								if(!str) continue;

								// ≒????次Map
								String Key(str);
								iter = SkillsMap.find(Key);
								if (iter != SkillsMap.end())
								{
									LORD_DELETE_T(pSkill, Skill);
									break;
								}
								SkillsMap.insert(std::pair<String, Skill*>(Key, pSkill));
							}
							else if(varName == "EnableWound")
							{
								pSkill->bWoundEnable = StringUtil::ParseBool(str);
							}				
							else if(varName == "UserDefineTime") // ???～?＆??※??????㊣???
							{
								pSkill->UserDefineTime = StringUtil::ParseReal(str);
							}
							else if(varName == "TextMode") // ???～?﹉???¯???次
							{
								pSkill->TextMoveMode = StringUtil::ParseInt(str);
							}

							pVarNode = pVarNode->next_attribute();
						}

						if (iter != SkillsMap.end())
						{
							pNode = pNode->next_sibling();
							continue;
						}

						// ?§次????
						rapidxml::xml_node<>* pChildNode = pNode->first_node();
						do
						{
							String childName = pChildNode->name();
							if(childName == "Animation")
							{
								rapidxml::xml_attribute<>* pSkinElement = pChildNode->first_attribute();
								AnimationInfo* pAnim = LORD_NEW_T(AnimationInfo);

								while(pSkinElement)
								{
									str = pSkinElement->value();
									String attributeName = pSkinElement->name();
									if(attributeName == "Name")
									{
										pAnim->AnimName = str;
									}
									else if(attributeName == "BeginTime")
									{
										pAnim->beginTime = (ui32)(StringUtil::ParseReal(str) * 1000);
									}
									else if(attributeName == "Times")
									{
										pAnim->times = StringUtil::ParseInt(str);
									}
									else if(attributeName == "Length")
									{
										pAnim->Length = (ui32)(StringUtil::ParseReal(str) * 1000);
									}
									else if(attributeName == "Channel")
									{
										pAnim->channel = str;
									}
									else if(attributeName == "FadeTime")
									{
										pAnim->FadeTime = (ui32)(StringUtil::ParseReal(str) * 1000);
									}
									else if(attributeName == "TimeScale")
									{
										pAnim->TimeScale = StringUtil::ParseReal(str);
									}
									pSkinElement = pSkinElement->next_attribute();
								}						
								pSkill->Animations.insert(std::make_pair(pAnim->AnimName, pAnim));
							}
							else if(childName == "Effect")
							{
								ActorEffect* pEffect = LoadEffect(pChildNode);
								if(pEffect)
								{
									pSkill->ActorEffects.push_back(pEffect);
								}
							}
							else if(childName == "AttackEvent")
							{
								rapidxml::xml_attribute<>* pAttackEventElement = pChildNode->first_attribute();
								AttackEvent* pAttackEvent = LORD_NEW_T(AttackEvent);
								pSkill->AttackEvents.push_back(pAttackEvent);

								// 每??§??≒?﹉＞¯???÷??????“??÷?※＆?﹢?????“＃?¯﹟?“??¯?
								pAttackEvent->times = 1;
								pAttackEvent->CycleTime = 1.0f;

								while(pAttackEventElement)
								{
									str = pAttackEventElement->value();
									String AttackEventName = pAttackEventElement->name();
									if(AttackEventName == "BeginTime")
									{
										pAttackEvent->beginTime = StringUtil::ParseReal(str);
									}
									else if(AttackEventName == "FileName")
									{
										pAttackEvent->AttackFileName = str;
									}
									else if(AttackEventName == "Times")
									{
										pAttackEvent->times = StringUtil::ParseInt(str);
									}	
									else if(AttackEventName == "CycleTime")
									{
										pAttackEvent->CycleTime = StringUtil::ParseReal(str);
									}
									else if (AttackEventName == "IsCallBack")
									{
										pAttackEvent->isCallback = StringUtil::ParseBool(str);
									}

									pAttackEventElement = pAttackEventElement->next_attribute();
								}
							}
							else if(childName == "CameraShake")
							{
								rapidxml::xml_attribute<>* pCameraShakeElement = pChildNode->first_attribute();
								CameraShake* pCameraShake = LORD_NEW_T(CameraShake);
								pSkill->CameraShakes.push_back(pCameraShake);

								while(pCameraShakeElement)
								{
									str = pCameraShakeElement->value();
									String varName = pCameraShakeElement->name();
									if(varName == "BeginTime")
									{
										pCameraShake->beginTime = StringUtil::ParseReal(str);
									}
									else if(varName == "DurationTime")
									{
										pCameraShake->durationTime = StringUtil::ParseReal(str);
									}	
									else if(varName == "ShakeScale")
									{
										pCameraShake->shakescale = StringUtil::ParseReal(str);
									}
									else if(varName == "IsAddToAllActors")
									{
										pCameraShake->isAddToAllActors = StringUtil::ParseBool(str);
									}
									else if(varName == "ShakeTimes")
									{
										pCameraShake->shakeTimes = StringUtil::ParseInt(str);
									}
									pCameraShakeElement = pCameraShakeElement->next_attribute();
								}
							}
							else if(childName == "ActorScale")
							{
								rapidxml::xml_attribute<>* pActorScaleElement = pChildNode->first_attribute();
								ActorScale* pActorScale = LORD_NEW_T(ActorScale);
								pSkill->ActorScales.push_back(pActorScale);

								while(pActorScaleElement)
								{
									str = pActorScaleElement->value();
									String varName = pActorScaleElement->name();
									if(varName == "beginTime")
									{
										pActorScale->beginTime = StringUtil::ParseReal(str);
									}
									else if(varName == "durationTime")
									{
										pActorScale->durationTime = StringUtil::ParseReal(str);
									}	
									else if(varName == "beginScale")
									{
										pActorScale->beginScale = StringUtil::ParseReal(str);
									}
									else if(varName == "endScale")
									{
										pActorScale->endScale = StringUtil::ParseReal(str);
									}
									pActorScaleElement = pActorScaleElement->next_attribute();
								}
							}
							else if(childName == "Sound")
							{
								rapidxml::xml_attribute<>* pSoundSkillElement = pChildNode->first_attribute();
								Sound* pSound = LORD_NEW_T(Sound);
								pSound->volume = 0.5f;
								pSound->soundFileNum = 1;
								//pSound->minDistance = 2.0f;
								//pSound->maxDistance = 5.0f;
								pSound->showRate = 1.0f;
								pSkill->Sounds.push_back(pSound);

								while(pSoundSkillElement)
								{
									str = pSoundSkillElement->value();
									String strSound = pSoundSkillElement->name();
									if(strSound == "BeginTime")
									{
										pSound->beginTime = StringUtil::ParseReal(str);
									}
									else if(strSound == "Name")
									{
										pSound->soundName = StringUtil::ParseInt(str);
									}
									else if (strSound == "Name2")
									{
										pSound->soundName2 = StringUtil::ParseInt(str);
									}
									else if (strSound == "SoundFileNum")
									{
										pSound->soundFileNum = StringUtil::ParseInt(str);
									}
									else if(strSound == "loop")
									{
										pSound->bloop = StringUtil::ParseBool(str);
									}	
									else if(strSound == "Volume")
									{
										pSound->volume = StringUtil::ParseReal(str);
									}
									else if(strSound == "ShowRate")
									{
										pSound->showRate = StringUtil::ParseReal(str);
									}
									/*else if (strSound == "MinDistance")
									{
										pSound->minDistance = StringUtil::ParseReal(str);
									}
									else if (strSound == "MaxDistance")
									{
										pSound->maxDistance = StringUtil::ParseReal(str);
									}*/
									else if (strSound == "isAddToAllActors")
									{
										pSound->isAddToAllActors = StringUtil::ParseBool(str);
									}
									else if (strSound == "addToAllActorVolume")
									{
										pSound->addToAllActorVolume = StringUtil::ParseReal(str);
									}
									pSoundSkillElement = pSoundSkillElement->next_attribute();
								}
							}
							else if(childName == "ColorChange")
							{
								rapidxml::xml_attribute<>* pColorChangeElement = pChildNode->first_attribute();
								ColorChange* pColorChange = LORD_NEW_T(ColorChange);
								pSkill->ColorChanges.push_back(pColorChange);

								while(pColorChangeElement)
								{
									str = pColorChangeElement->value();
									String ColorChange = pColorChangeElement->name();
									if(ColorChange == "BeginTime")
									{
										pColorChange->beginTime = StringUtil::ParseReal(str);
									}
									else if(ColorChange == "R")
									{
										pColorChange->color.r = StringUtil::ParseReal(str);
									}
									else if(ColorChange == "G")
									{
										pColorChange->color.g = StringUtil::ParseReal(str);
									}
									else if(ColorChange == "B")
									{
										pColorChange->color.b = StringUtil::ParseReal(str);
									}
									else if(ColorChange == "A")
									{
										pColorChange->color.a = StringUtil::ParseReal(str);
									}
									else if(ColorChange == "Cascade")
									{
										pColorChange->bCascade = StringUtil::ParseBool(str);
									}
									else if(ColorChange == "SmoothTime")
									{
										pColorChange->smoothTime = StringUtil::ParseReal(str);
									}
									else if(ColorChange == "DurationTime")
									{
										pColorChange->durationTime = StringUtil::ParseReal(str);
									}
									else if(ColorChange == "ResumeTime")
									{
										pColorChange->resumeTime = StringUtil::ParseReal(str);
									}
									pColorChangeElement = pColorChangeElement->next_attribute();
								}
							}
							else if(childName == "SlowDown")
							{
								rapidxml::xml_attribute<>* pSlowDownElement = pChildNode->first_attribute();
								SlowDown* pSlowDown = LORD_NEW_T(SlowDown);
								pSkill->SlowDowns.push_back(pSlowDown);
								while(pSlowDownElement)
								{
									str = pSlowDownElement->value();
									String mSlowDown = pSlowDownElement->name();
									if(mSlowDown == "timeScale")
									{
										pSlowDown->timeScale = StringUtil::ParseReal(str);
									}
									else if(mSlowDown == "beginTime")
									{
										pSlowDown->beginTime = StringUtil::ParseReal(str);
									}
									else if(mSlowDown == "durationTime")
									{
										pSlowDown->durationTime = StringUtil::ParseReal(str);
									}
									else if(mSlowDown == "equalizeTime")
									{
										pSlowDown->equalizeTime = StringUtil::ParseReal(str);
									}
									else if(mSlowDown == "Cascade")
									{
										pSlowDown->bCascade = StringUtil::ParseBool(str);
									}
									else if(mSlowDown == "ShowRate")
									{
										pSlowDown->showRate = StringUtil::ParseReal(str);
									}
									pSlowDownElement = pSlowDownElement->next_attribute();
								}
							}
							else if(childName == "HitFly")
							{
								rapidxml::xml_attribute<>* pHitFlyElement = pChildNode->first_attribute();
								HitFly* pHitfly = LORD_NEW_T(HitFly);
								pSkill->HitFlys.push_back(pHitfly);
								while(pHitFlyElement)
								{
									str = pHitFlyElement->value();
									String mHitfly = pHitFlyElement->name();
									if(mHitfly == "beginTime")
									{
										pHitfly->beginTime = StringUtil::ParseReal(str);
									}
									else if(mHitfly == "durationTime")
									{
										pHitfly->durationTime = StringUtil::ParseReal(str);
									}
									else if(mHitfly == "high")
									{
										pHitfly->high = StringUtil::ParseReal(str);
									}
									pHitFlyElement = pHitFlyElement->next_attribute();
								}
							}
							else if(childName == "SkillBreak")
							{
								rapidxml::xml_attribute<>* pSkillBreakElement = pChildNode->first_attribute();
								SkillBreak* pSkillBreak = LORD_NEW_T(SkillBreak);
								pSkill->SkillBreaks.push_back(pSkillBreak);
								while(pSkillBreakElement)
								{
									str = pSkillBreakElement->value();
									String mSkillBreak = pSkillBreakElement->name();
									if(mSkillBreak == "beginTime")
									{
										pSkillBreak->beginTime = StringUtil::ParseReal(str);
									}
									else if(mSkillBreak == "isCallBlack")
									{
										pSkillBreak->isCallBlack = StringUtil::ParseBool(str);
									}
									else if(mSkillBreak == "isFrozen")
									{
										pSkillBreak->isFrozen = StringUtil::ParseBool(str);
									}
									else if(mSkillBreak == "isHide")
									{
										pSkillBreak->isHide = StringUtil::ParseBool(str);
									}
									pSkillBreakElement = pSkillBreakElement->next_attribute();
								}
							}
							else if(childName == "CameraPush")
							{
								rapidxml::xml_attribute<>* pCameraPushElement = pChildNode->first_attribute();
								CameraPush* pCameraPush = LORD_NEW_T(CameraPush);
								pSkill->CameraPushs.push_back(pCameraPush);
								while(pCameraPushElement)
								{
									str = pCameraPushElement->value();
									String mCameraPush = pCameraPushElement->name();
									if(mCameraPush == "beginTime")
									{
										pCameraPush->beginTime = StringUtil::ParseReal(str);
									}
									else if(mCameraPush == "durationTime")
									{
										pCameraPush->durationTime = StringUtil::ParseReal(str);
									}
									else if(mCameraPush == "closeTime")
									{
										pCameraPush->closeTime = StringUtil::ParseReal(str);
									}
									else if(mCameraPush == "farawayTime")
									{
										pCameraPush->farawayTime = StringUtil::ParseReal(str);
									}
									else if(mCameraPush == "scale")
									{
										pCameraPush->scale = StringUtil::ParseReal(str);
									}
									else if(mCameraPush == "isInversion")
									{
										pCameraPush->isInversion = StringUtil::ParseBool(str);
									}
									else if (mCameraPush == "isAddToAllActors")
									{
										pCameraPush->isAddToAllActors = StringUtil::ParseBool(str);
									}
									pCameraPushElement = pCameraPushElement->next_attribute();
								}
							}
							else if(childName == "ActorScaleChange")
							{
								rapidxml::xml_attribute<>* pActorScaleChangeElement = pChildNode->first_attribute();
								Vector2* pActorScaleChange = LORD_NEW_T(Vector2(1.0f,1.0f));
								pSkill->pSetScale = pActorScaleChange;
								while(pActorScaleChangeElement)
								{
									str = pActorScaleChangeElement->value();
									String mActorScaleChange = pActorScaleChangeElement->name();
									if(mActorScaleChange == "beginScale")
									{
										pActorScaleChange->x = StringUtil::ParseReal(str);
									}
									else if(mActorScaleChange == "endScale")
									{
										pActorScaleChange->y = StringUtil::ParseReal(str);
									}
									pActorScaleChangeElement = pActorScaleChangeElement->next_attribute();
								}
							}
							else if (childName == "CameraAnim")
							{
								rapidxml::xml_attribute<>* pCameraAnimElement = pChildNode->first_attribute();
								CameraAnim* anim = LORD_NEW_T(CameraAnim);
								pSkill->CameraAnims.push_back(anim);
								while (pCameraAnimElement)
								{
									str = pCameraAnimElement->value();
									if (String(pCameraAnimElement->name()) == "beginTime")
									{
										anim->beginTime = StringUtil::ParseReal(str);
									}
									else
									{
										anim->animName = str;
									}
									pCameraAnimElement = pCameraAnimElement->next_attribute();
								}
							}
							else if (childName == "ChangeDark")
							{
								rapidxml::xml_attribute<>* pChangeDarkElement = pChildNode->first_attribute();
								ChangeDark* pchange = LORD_NEW_T(ChangeDark);
								pSkill->ChangeDarks.push_back(pchange);
								while (pChangeDarkElement)
								{
									str = pChangeDarkElement->value();
									String changename = pChangeDarkElement->name();
									if (changename == "beginTime")
									{
										pchange->beginTime = StringUtil::ParseReal(str);
									}
									else if (changename == "darkness")
									{
										pchange->darkness = StringUtil::ParseReal(str);
									}
									else if (changename == "darktime")
									{
										pchange->darkTime = StringUtil::ParseReal(str);
									}
									else if (changename == "lightness")
									{
										pchange->lightness = StringUtil::ParseReal(str);
									}
									else if (changename == "lighttime")
									{
										pchange->lightTime = StringUtil::ParseReal(str);
									}
									else if (changename == "duration")
									{
										pchange->duration = StringUtil::ParseReal(str);
									}
									pChangeDarkElement = pChangeDarkElement->next_attribute();
								}
							}
							pChildNode = pChildNode->next_sibling();
						}
						while(pChildNode);
					}

					pNode = pNode->next_sibling();
				}
				while(pNode);
			}
			return true;
		}
		catch(bool)
		{
			return false;
		}
	}
	//-----------------------------------------------------------------------
	bool ActorTemplateData::LoadAnims(void* pNode)
	{
		rapidxml::xml_node<>* pSubNode = static_cast<rapidxml::xml_node<>*>(pNode);
		try
		{
			rapidxml::xml_node<>* pNode = pSubNode->first_node();
			if(pNode)
			{
				do
				{
					String strName = pNode->name();
					if(strName == "Anim")
					{
						rapidxml::xml_attribute<>* pVarNode = pNode->first_attribute();
						// ??Key每＞??
						while(pVarNode)
						{
							const char* str = pVarNode->value();
							String AnimName = pVarNode->name();
							if(AnimName == "Name")
							{
								vector<String>::type::iterator iter = std::find(AnimsList.begin(), AnimsList.end(), str);
								if (iter == AnimsList.end())
								{
									AnimsList.push_back(str);
								}
							}
							pVarNode = pVarNode->next_attribute();
						}
					}

					pNode = pNode->next_sibling();
				}
				while(pNode);
			}
			return true;
		}
		catch(...)
		{
			return false;
		}
	}
	//-----------------------------------------------------------------------
	ActorTemplateData::StaticMesh* ActorTemplateData::LoadStaticMesh(void* pNode)
	{
		rapidxml::xml_node<>* pSubNode = static_cast<rapidxml::xml_node<>*>(pNode);
		StaticMesh* pMesh = LORD_NEW_T(StaticMesh);
		LordAssert(pMesh);
		try
		{
			rapidxml::xml_node<>* pNode = pSubNode->first_node();
			if(!pNode)
				return NULL;

			do
			{
				String NodeName = pNode->name();
				if("FileName" == NodeName)
				{
					rapidxml::xml_attribute<>* pMeshElement = pNode->first_attribute();
					String valName = pMeshElement->name();
					while("value" != valName)
					{
						pMeshElement = pMeshElement->next_attribute();
						valName = pMeshElement->name();
					}
					pMesh->MeshName = pMeshElement->value();
				}
				else if("SocketName" == NodeName)
				{
					rapidxml::xml_attribute<>* pMeshElement = pNode->first_attribute();
					String valName = pMeshElement->name();
					while("value" != valName)
					{
						pMeshElement = pMeshElement->next_attribute();
						valName = pMeshElement->name();
					}
					pMesh->SocketName = pMeshElement->value();
				}
				else if("Position_X" == NodeName)
				{
					rapidxml::xml_attribute<>* pMeshElement = pNode->first_attribute();
					String valName = pMeshElement->name();
					while("value" != valName)
					{
						pMeshElement = pMeshElement->next_attribute();
						valName = pMeshElement->name();
					}
					pMesh->position.x = StringUtil::ParseReal(String(pMeshElement->value()));
				}
				else if("Position_Y" == NodeName)
				{
					rapidxml::xml_attribute<>* pMeshElement = pNode->first_attribute();
					String valName = pMeshElement->name();
					while("value" != valName)
					{
						pMeshElement = pMeshElement->next_attribute();
						valName = pMeshElement->name();
					}
					pMesh->position.y = StringUtil::ParseReal(String(pMeshElement->value()));
				}
				else if("Position_Z" == NodeName)
				{
					rapidxml::xml_attribute<>* pMeshElement = pNode->first_attribute();
					String valName = pMeshElement->name();
					while("value" != valName)
					{
						pMeshElement = pMeshElement->next_attribute();
						valName = pMeshElement->name();
					}
					pMesh->position.z = StringUtil::ParseReal(String(pMeshElement->value()));
				}
				else if("yaw" == NodeName)
				{
					rapidxml::xml_attribute<>* pMeshElement = pNode->first_attribute();
					String valName = pMeshElement->name();
					while("value" != valName)
					{
						pMeshElement = pMeshElement->next_attribute();
						valName = pMeshElement->name();
					}
					pMesh->yaw = StringUtil::ParseReal(String(pMeshElement->value()));
				}
				else if("pitch" == NodeName)
				{
					rapidxml::xml_attribute<>* pMeshElement = pNode->first_attribute();
					String valName = pMeshElement->name();
					while("value" != valName)
					{
						pMeshElement = pMeshElement->next_attribute();
						valName = pMeshElement->name();
					}
					pMesh->pitch = StringUtil::ParseReal(String(pMeshElement->value()));
				}
				else if("roll" == NodeName)
				{
					rapidxml::xml_attribute<>* pMeshElement = pNode->first_attribute();
					String valName = pMeshElement->name();
					while("value" !=  valName)
					{
						pMeshElement = pMeshElement->next_attribute();
						valName = pMeshElement->name();
					}
					pMesh->roll = StringUtil::ParseReal(String(pMeshElement->value()));
				}
				else if("scale" == NodeName)
				{
					rapidxml::xml_attribute<>* pMeshElement = pNode->first_attribute();
					String valName = pMeshElement->name();
					while("value" != valName)
					{
						pMeshElement = pMeshElement->next_attribute();
						valName = pMeshElement->name();
					}
					pMesh->scale = StringUtil::ParseReal(String(pMeshElement->value()));
				}

				pNode = pNode->next_sibling();
			}
			while(pNode);

			return pMesh;
		}
		catch(bool)
		{
			LORD_DELETE_T(pMesh, StaticMesh); 
			return NULL;
		}
	}
	//-----------------------------------------------------------------------
	ActorTemplateData::ActorEffect* ActorTemplateData::LoadEffect(void* pNode)
	{
		rapidxml::xml_node<>* pSubNode = static_cast<rapidxml::xml_node<>*>(pNode);

		ActorEffect* pEffect = LORD_NEW_T(ActorEffect); LordAssert(pEffect);
		pEffect->Alpha = 1.0f;
		pEffect->TimeScale = 1.0f;
		pEffect->CycleTime = -1.0f;
		pEffect->FollowActor = true;
		pEffect->Persist = false;
		try
		{
			rapidxml::xml_node<>* pNode = pSubNode->first_node();
			if(!pNode)
				return NULL;

			do
			{
				String NodeName = pNode->name();
				if("FileName" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->EffectFileName = pEffectElement->value();
				}
				else if("SocketName" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->SocketName = pEffectElement->value();
				}
				else if("BeginTime" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->beginTime = StringUtil::ParseReal(String(pEffectElement->value()));
				}
				else if("Times" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->times = StringUtil::ParseInt(String(pEffectElement->value()));
				}
				else if("Position_X" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->position.x = StringUtil::ParseReal(String(pEffectElement->value()));
				}
				else if("Position_Y" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->position.y = StringUtil::ParseReal(String(pEffectElement->value()));
				}
				else if("Position_Z" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->position.z = StringUtil::ParseReal(String(pEffectElement->value()));
				}
				else if("yaw" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->yaw = StringUtil::ParseReal(String(pEffectElement->value()));
				}
				else if("pitch" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->pitch = StringUtil::ParseReal(String(pEffectElement->value()));
				}
				else if("roll" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->roll = StringUtil::ParseReal(String(pEffectElement->value()));
				}
				else if("scale" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->scale = StringUtil::ParseReal(String(pEffectElement->value()));
				}
				else if("TimeScale" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->TimeScale = StringUtil::ParseReal(String(pEffectElement->value()));
				}
				else if("Alpha" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->Alpha = StringUtil::ParseReal(String(pEffectElement->value()));
				}
				else if("CycleTime" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->CycleTime = StringUtil::ParseReal(String(pEffectElement->value()));
				}
				else if("FollowActor" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->FollowActor = StringUtil::ParseBool(String(pEffectElement->value()));
				}
				else if("Persist" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->Persist = StringUtil::ParseBool(String(pEffectElement->value()));
				}
				else if("isSlowdownValuable" == NodeName)
				{
					rapidxml::xml_attribute<>* pEffectElement = pNode->first_attribute();
					String valName = pEffectElement->name();
					while("value" != valName)
					{
						pEffectElement = pEffectElement->next_attribute();
						valName = pEffectElement->name();
					}
					pEffect->isSlowdownValuable = StringUtil::ParseBool(String(pEffectElement->value()));
				}

				pNode = pNode->next_sibling();
			}
			while(pNode);

			return pEffect;
		}
		catch(bool)
		{
			LORD_DELETE_T(pEffect, ActorEffect); 
			return NULL;
		}
	}
	//-----------------------------------------------------------------------
	bool ActorTemplateData::LoadBoneGroups(void* pNode)
	{
		rapidxml::xml_node<>* pSubNode = static_cast<rapidxml::xml_node<>*>(pNode);
		try
		{

			rapidxml::xml_node<> *pNode = pSubNode->first_node();
			if(!pNode)
				return false;

			do
			{
				String nodeName = pNode->name();
				if("BoneGroup" == nodeName)
				{
					rapidxml::xml_attribute<>* pBoneGroupElement = pNode->first_attribute();

					// ??Key每＞??
					while("Name" == nodeName)
					{
						pBoneGroupElement = pBoneGroupElement->next_attribute();
					}

					const char* str = pBoneGroupElement->value();
					if(!str)
						continue;
					String Key(str);
					// ???﹢¯?§每※???＃?﹢?＃???羽???
					if(Key != "UpperBody")
						continue;					

					// ?§次????
					rapidxml::xml_node<> *pBoneGroupChildNode = pNode->first_node();
					if(!pBoneGroupChildNode)
					{
						continue;
					}
					do
					{
						if(!strcmp("Bone", pBoneGroupChildNode->name()))
						{
							rapidxml::xml_attribute<> *pBoneElement = pBoneGroupChildNode->first_attribute();

							while(!strcmp("Name", pBoneElement->name()))
							{
								pBoneElement = pBoneElement->next_attribute();
							}
							mUpperBodyBonesName.insert(pBoneElement->value());
						}

						pBoneGroupChildNode = pBoneGroupChildNode->next_sibling();
					}
					while(pBoneGroupChildNode);
				}

				pNode = pNode->next_sibling();
			}
			while(!pNode);

			return true;
		}
		catch(bool)
		{
			return false;
		}
	}
	//-----------------------------------------------------------------------
	bool ActorTemplateData::LoadAppendEffects(void* pNode)
	{
		try
		{
			rapidxml::xml_node<>* pSubNode = static_cast<rapidxml::xml_node<>*>(pNode);
			rapidxml::xml_node<> *pNode = pSubNode->first_node();
			if(!pNode)
			{
				return false;
			}

			do
			{
				if(!strcmp("Effect", pNode->name()))
				{
					ActorEffect* pEffect = LORD_NEW_T(ActorEffect); LordAssert(pEffect);
					pEffect->times = -1;
					pEffect->beginTime = 0.0f;
					pEffect->CycleTime = -1.0f;
					pEffect->FollowActor = true;
					pEffect->Persist = false;

					// ?§次????
					rapidxml::xml_node<> *pEffectChildNode = pNode->first_node();
					if(!pEffectChildNode)
						continue;
					do
					{
						if(!strcmp("FileName", pEffectChildNode->name()))
						{
							rapidxml::xml_attribute<> *pAttribute = pEffectChildNode->first_attribute();
							while(!strcmp("value", pAttribute->name()))
							{
								pAttribute = pAttribute->next_attribute();
							}
							pEffect->EffectFileName = pAttribute->value();
						}
						else if(!strcmp("SocketName", pEffectChildNode->name()))
						{
							rapidxml::xml_attribute<> *pAttribute = pEffectChildNode->first_attribute();
							while(!strcmp("value", pAttribute->name()))
							{
								pAttribute = pAttribute->next_attribute();
							}
							pEffect->SocketName = pAttribute->value();
						}
						else if(!strcmp("Position_X", pEffectChildNode->name()))
						{
							rapidxml::xml_attribute<> *pAttribute = pEffectChildNode->first_attribute();
							while(!strcmp("value", pAttribute->name()))
							{
								pAttribute = pAttribute->next_attribute();
							}
							pEffect->position.x = StringUtil::ParseReal(String(pAttribute->value()));
						}
						else if(!strcmp("Position_Y", pEffectChildNode->name()))
						{
							rapidxml::xml_attribute<> *pAttribute = pEffectChildNode->first_attribute();
							while(!strcmp("value", pAttribute->name()))
							{
								pAttribute = pAttribute->next_attribute();
							}
							pEffect->position.y = StringUtil::ParseReal(String(pAttribute->value()));
						}
						else if(!strcmp("Position_Z", pEffectChildNode->name()))
						{
							rapidxml::xml_attribute<> *pAttribute = pEffectChildNode->first_attribute();
							while(!strcmp("value", pAttribute->name()))
							{
								pAttribute = pAttribute->next_attribute();
							}
							pEffect->position.z = StringUtil::ParseReal(String(pAttribute->value()));
						}
						else if(!strcmp("yaw", pEffectChildNode->name()))
						{
							rapidxml::xml_attribute<> *pAttribute = pEffectChildNode->first_attribute();
							while(!strcmp("value", pAttribute->name()))
							{
								pAttribute = pAttribute->next_attribute();
							}
							pEffect->yaw = StringUtil::ParseReal(String(pAttribute->value()));
						}
						else if(!strcmp("pitch", pEffectChildNode->name()))
						{
							rapidxml::xml_attribute<> *pAttribute = pEffectChildNode->first_attribute();
							while(!strcmp("value", pAttribute->name()))
							{
								pAttribute = pAttribute->next_attribute();
							}
							pEffect->pitch = StringUtil::ParseReal(String(pAttribute->value()));
						}
						else if(!strcmp("roll", pEffectChildNode->name()))
						{
							rapidxml::xml_attribute<> *pAttribute = pEffectChildNode->first_attribute();
							while(!strcmp("value", pAttribute->name()))
							{
								pAttribute = pAttribute->next_attribute();
							}
							pEffect->roll = StringUtil::ParseReal(String(pAttribute->value()));
						}
						else if(!strcmp("scale", pEffectChildNode->name()))
						{
							rapidxml::xml_attribute<> *pAttribute = pEffectChildNode->first_attribute();
							while(!strcmp("value", pAttribute->name()))
							{
								pAttribute = pAttribute->next_attribute();
							}
							pEffect->scale = StringUtil::ParseReal(String(pAttribute->value()));
						}
						else if(!strcmp("TimeScale", pEffectChildNode->name()))
						{
							rapidxml::xml_attribute<> *pAttribute = pEffectChildNode->first_attribute();
							while(!strcmp("value", pAttribute->name()))
							{
								pAttribute = pAttribute->next_attribute();
							}
							pEffect->TimeScale = StringUtil::ParseReal(String(pAttribute->value()));
						}
						else if(!strcmp("Alpha", pEffectChildNode->name()))
						{
							rapidxml::xml_attribute<> *pAttribute = pEffectChildNode->first_attribute();
							while(!strcmp("value", pAttribute->name()))
							{
								pAttribute = pAttribute->next_attribute();
							}
							pEffect->Alpha = StringUtil::ParseReal(String(pAttribute->value()));
						}

						pEffectChildNode = pEffectChildNode->next_sibling();
					}
					while(pEffectChildNode);

					mAppendEffects.push_back(pEffect);
				}

				pNode = pNode->next_sibling();
			}
			while(pNode);

			return true;
		}
		catch(bool)
		{
			return false;
		}
	}

	// ================================ actor??﹢?羽??????=====================================================
	//---------------------------------------------------------------------
	ActorTemplateDataManager* ActorTemplateDataManager::mSingleton = NULL;

	//---------------------------------------------------------------------
	ActorTemplateDataManager* ActorTemplateDataManager::getSingletonPtr()
	{
		return mSingleton;
	}

	//---------------------------------------------------------------------
	ActorTemplateDataManager& ActorTemplateDataManager::getSingleton()
	{
		return *mSingleton;
	}

	//---------------------------------------------------------------------
	ActorTemplateDataManager::ActorTemplateDataManager(void)
	{
		mSingleton = this;
	}

	//---------------------------------------------------------------------
	ActorTemplateDataManager::~ActorTemplateDataManager(void)
	{
		Clear();
	}

	//---------------------------------------------------------------------
	void ActorTemplateDataManager::Clear()
	{
		map<String, ActorTemplateData*>::type::iterator it;
		for(it = mActorTemplateData.begin(); it != mActorTemplateData.end(); ++it)
		{
			LordDelete it->second;
		}
		mActorTemplateData.clear();
	}

	//---------------------------------------------------------------------
	ActorTemplateData* ActorTemplateDataManager::LoadByFile(const String& FileName)
	{
		map<String, ActorTemplateData*>::type::iterator it = mActorTemplateData.find(FileName);
		if(it != mActorTemplateData.end())
		{
			return it->second;
		}
		else
		{
			ActorTemplateData* pNewData = LordNew ActorTemplateData;

			try 
			{
				DataStream* pXMLStream = ResourceGroupManager::Instance()->openResource(FileName);
				if(pXMLStream == NULL)
				{
					LordLogError("ActorTemplate file not found [%s].", FileName.c_str());
					return NULL;
				}

				size_t fileSize = pXMLStream->size();
				char* xmlData = (char*)LordMalloc(fileSize + 1);
				pXMLStream->read(xmlData, fileSize);

				xmlData[fileSize] = 0;

				rapidxml::xml_document<>* doc = LordNewT(rapidxml::xml_document<>);        // character type defaults to cha
				doc->parse<0>(xmlData);

				rapidxml::xml_node<> *pRootNode = doc->first_node();
				if(!pRootNode)
				{
					LordLogError("The ActorTemplate file content is valid.");
					throw 0;
				}

				rapidxml::xml_node<> *pNode = pRootNode->first_node();

				bool ret = pNewData->Load(pNode);

				bool retParent = true;
				if (!pNewData->GetParentTemplateName().empty())
				{
					retParent = pNewData->LoadParentTemplateFile(pNewData->GetParentTemplateName());
				}

				if(ret && retParent)
				{
					mActorTemplateData.insert(std::pair<String, ActorTemplateData*>(FileName, pNewData));
					LordDeleteT(doc, xml_document<>);
					LordFree(xmlData);
					LordDelete pXMLStream;
					return pNewData;
				}
				else
				{
					throw false;
				}
			}
			catch(bool) 
			{
				LordDelete pNewData;
				LordLogError("ActorTemplateDataManager::LoadByFile: Fail to Parse actor file %s!", FileName.c_str());
				return NULL;
			}

		}
	}

	ActorTemplateData* ActorTemplateDataManager::LoadByStream(const String& FileName, DataStream* datastream)
	{
		map<String, ActorTemplateData*>::type::iterator it = mActorTemplateData.find(FileName);
		if (it != mActorTemplateData.end())
		{
			return it->second;
		}
		else
		{
			ActorTemplateData* pNewData = LordNew ActorTemplateData;
			
			try
			{
				size_t fileSize = datastream->size();
				char* xmlData = (char*)LordMalloc(fileSize + 1);
				datastream->read(xmlData, fileSize);

				xmlData[fileSize] = 0;

				rapidxml::xml_document<>* doc = LordNewT(rapidxml::xml_document<>);        // character type defaults to cha
				doc->parse<0>(xmlData);

				rapidxml::xml_node<> *pRootNode = doc->first_node();
				if (!pRootNode)
				{
					throw 0;
				}

				rapidxml::xml_node<> *pNode = pRootNode->first_node();

				bool ret = pNewData->Load(pNode);

				bool retParent = true;
				if (!pNewData->GetParentTemplateName().empty())
				{
					retParent = pNewData->LoadParentTemplateFile(pNewData->GetParentTemplateName());
				}

				if (ret && retParent)
				{
					mActorTemplateData.insert(std::pair<String, ActorTemplateData*>(FileName, pNewData));
					LordDeleteT(doc, xml_document<>);
					LordFree(xmlData);
					return pNewData;
				}
				else
				{
					throw false;
				}
			}
			catch (...)
			{
				LordDelete pNewData;
				LordDelete datastream;
				return NULL;
			}
		}
	}

}
