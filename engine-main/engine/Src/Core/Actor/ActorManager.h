/********************************************************************
filename: 	ActorManager.h
file path:	dev\engine\Src\Core\Actor\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ACTORMANAGER_H__
#define __LORD_ACTORMANAGER_H__

#include "Core.h"
#include "ActorObject.h"

namespace LORD
{
	class SkillAttackManager;
	class ActorTemplateDataManager;
	class MergeMeshManager;
	class ActorObject;

	typedef set<ActorObject*>::type ActorSet;
	typedef ActorSet::iterator ActorIterator;
	typedef vector<ActorObject::Attack_Event_Info>::type AttEventVector;
	typedef AttEventVector::iterator AttEventVectorIt;
	typedef map<int, String>::type AttReplaceMap;
	//////////////////////////////////////////////////////////////////////////
	// 角色管理器
	// 1. 以角色池的方式提供创建销毁接口
	// 2. 所有角色均挂接到mRootSceneNode下统一管理
	// 3. 管理所有角色提供查询和更新
	class LORD_CORE_API ActorManager : public Singleton<ActorManager>, public ObjectAlloc
	{
	public:
		inline static ActorManager* Instance()
		{
			return ms_pSingleton;
		}

		struct ActorPickInfo
		{
			ActorObject* pActor;
			float distance;

			bool operator < (const ActorPickInfo& rhs) const
			{
				return this->distance < rhs.distance;
			}
		};

		typedef void(*Wound_CallBack)(ActorObject* beginActor, const String& skillName, ActorObject* endActor, const String& attName, int callbackindex, int callbacknum, const String& name, int nUserData, int nUserData2);
		typedef void(*SkillName_CallBack)(const String& name, ActorObject* actor);
		typedef void(*SkillBreak_CallBack)(const String& name, ActorObject* actor);

	public:
		ActorManager(void);
		~ActorManager(void);
	public:
		// 创建actor
		ActorObject* CreateActor(const String &actorTemplate, const String &initSkillName = "", int AnUserData = 0, bool isUIActor = false, bool isMainActor = false, bool bSingleThread = false, bool isHide = false);
		ActorObject* CreateActor(const String& actorTemplate, const String& initSkillName, bool isUIActor, bool needUpdate = false);

		// 释放actor
		void DestroyActor(ActorObject *pActor, bool mark = true);

		void DestroyActorByID(const String &nameid);

		bool ValidActor(ActorObject* pActor);

		void SetActorsHide(bool bHide, const String& nameArray);

		// Actor拾取
		ActorObject* RayPickActor(const LORD::Ray& ray) const;

		// for lua interface
		ActorObject* RayPickActor(float screenPosX, float screenPosY) const;

		// 迭代遍历
		ActorIterator GetActorBegin();
		ActorIterator GetActorEnd();

		// 设置actor行为加速比
		void SetSpeedUp(float speedMultipler = 1.0f);
		float GetSpeedUp() const { return m_fTimeScale; }

		void SetAttackScale(float scale) { m_fAttackScale = scale; }
		float GetAttackScale() const { return m_fAttackScale; }

		// 释放所有的Actor，切场景时调用
		void FreeActors(void);

		SceneNode* GetActorRootSceneNode(void);

		Material* GetSkinMaterial() const { return m_SkinMaterial; }
		Material* GetSkinMaterial_xray() const { return m_SkinMaterial_xray; }
		Material* GetSkinMaterial_depth() const { return m_SkinMaterial_depth; }
		Material* GetSkinMaterial_transparent() const { return m_SkinMaterial_transparent; }


		Material* GetStaticMaterial() const { return m_StaticMaterial; }
		Material* GetStaticMaterial_xray() const { return m_StaticMaterial_xray; }
		//Material* GetStaticMaterial_depth() const { return m_StaticMaterial_depth; }
		Material* GetStaticMaterial_transparent() const { return m_StaticMaterial_transparent; }

		void DestroyDelayActors(bool bSingleThread = false);

		void AddCrameraShakeToAllActors(float scale, float durationTime, int shakeTimes);

		void AddCrameraPushToAllActors(LORD::ActorTemplateData::CameraPush* pData);

		//设置受击回调
		void registerWoundCallBack(Wound_CallBack woundFuncp) { m_WoudFuncp = woundFuncp; }

		Wound_CallBack getWoundCallBack() { return m_WoudFuncp; }

		void registerSkillNameCallBack(SkillName_CallBack skillName) { m_SkillNameCBFuncp = skillName; }

		SkillName_CallBack getSkillNameCallBack() { return m_SkillNameCBFuncp; }

		void registerSkillBreakCallBack(SkillBreak_CallBack skillBreak) { m_SkillBreakCBFuncp = skillBreak; }

		SkillBreak_CallBack getSkillBreakCallBack() { return m_SkillBreakCBFuncp; }

		void SetSelectEffect(const vector<String>::type& effectName, const Quaternion& quan = Quaternion::IDENTITY);

		EffectSystem* GetSelectEffect(const String& effectName, const String& templateName);

		void SetActorsSkillCameraEnable(bool enable) { m_SkillCameraEnable = enable; }

		bool GetActorsSkillCameraEnable() { return m_SkillCameraEnable; }

		AttReplaceMap* GetAttReplaceMap() { return m_ATTReplaceMap; }

		void		   SetAttReplaceMap(AttReplaceMap* attMap) { m_ATTReplaceMap = attMap; }

		int registerWoundCallBack(int handler)
		{
			_scriptHandler = handler;
			return _scriptHandler;
		}
		void unregisterWoundCallBack()
		{
			_scriptHandler = -1;
		}
		int getWoundCallBackHandler()
		{
			return _scriptHandler;
		}

		void UpdateActors(ui32 delta);

		void createMaterials();

		void stopCameraAnims();
	public:
		static Texture* s_StoneTexture;
		static Texture* s_FrozenTexture;
		static Texture* s_PoisonTexture;
		static RenderQueue* m_ActorRenderQueue;
		static RenderQueue* m_TranslateRenderQueue;
	protected:
		bool    m_SkillCameraEnable;
		int		_scriptHandler;
		float   m_fTimeScale;
		float	m_fAttackScale;

		ActorTemplateDataManager* m_pTemplateDataManager = nullptr;
		SkillAttackManager* m_pAttackManager = nullptr;
		MergeMeshManager* m_pMergeMeshManager = nullptr;
		AttReplaceMap* m_ATTReplaceMap = nullptr;
		Material* m_SkinMaterial = nullptr;
		Material* m_SkinMaterial_depth = nullptr;
		Material* m_SkinMaterial_transparent = nullptr;
		Material* m_SkinMaterial_xray = nullptr;

		Material* m_StaticMaterial = nullptr;
		//Material* m_StaticMaterial_depth;
		Material* m_StaticMaterial_transparent = nullptr;
		Material* m_StaticMaterial_xray = nullptr;
		SceneNode* mActorRoot = nullptr;

		Wound_CallBack m_WoudFuncp;
		SkillName_CallBack m_SkillNameCBFuncp;
		SkillBreak_CallBack m_SkillBreakCBFuncp;

		ActorSet mActorActiveSet;
		ActorSet mDestoryActorSet;
		set<EffectSystem*>::type mSelectEffectSet;
	};

}

#endif