#include "ActorManager.h"
#include "Actor/ActorTemplateData.h"
#include "Actor/SkillAttack.h"
#include "Actor/ActorObject.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneNode.h"
#include "Object/Root.h"
#include "Render/RenderQueue.h"
#include "Render/Material.h"
#include "Render/ShaderProgramManager.h"
#include "Effect/EffectSystemManager.h"
#include "MergeMeshManager.h"

namespace LORD
{
	// 角色管理器
	static unsigned int NextActorId = 0;

	Texture* ActorManager::s_StoneTexture = 0;
	Texture* ActorManager::s_FrozenTexture = 0;
	Texture* ActorManager::s_PoisonTexture = 0;
	RenderQueue* ActorManager::m_ActorRenderQueue = 0;
	RenderQueue* ActorManager::m_TranslateRenderQueue = 0;

	//---------------------------------------------------------------------
	static unsigned int GetNextActorID(void)
	{
		return NextActorId++;
	}
	//---------------------------------------------------------------------
	/*ActorManager* ActorManager::Instance()
	{
	return ms_pSingleton;
	}*/
	ActorManager::ActorManager(void)
		: mActorRoot(NULL)
		, m_WoudFuncp(NULL)
		, m_SkillNameCBFuncp(NULL)
		, m_SkillBreakCBFuncp(NULL)
		, m_SkillCameraEnable(true)
		, m_ATTReplaceMap(NULL)
		, m_fTimeScale(1.0f)
		, m_fAttackScale(1.0f)
	{
		m_pTemplateDataManager = LordNew ActorTemplateDataManager;
		m_pAttackManager = LordNew SkillAttackManager;
		m_pMergeMeshManager = LordNew MergeMeshManager;
	}
	//---------------------------------------------------------------------
	ActorManager::~ActorManager(void)
	{
		set<EffectSystem*>::type::const_iterator it = mSelectEffectSet.begin();
		for (; it != mSelectEffectSet.end(); ++it)
		{
			GetActorRootSceneNode()->destroyChild((*it)->getEffectSceneNode());
		}
		mSelectEffectSet.clear();

		FreeActors();
		if (mActorRoot)
		{
			LordDelete mActorRoot;
			mActorRoot = NULL;
		}

		LordSafeDelete(m_SkinMaterial);
		LordSafeDelete(m_SkinMaterial_depth);
		LordSafeDelete(m_SkinMaterial_transparent);
		LordSafeDelete(m_SkinMaterial_xray);

		LordSafeDelete(m_StaticMaterial);
		//LordSafeDelete(m_StaticMaterial_depth);
		LordSafeDelete(m_StaticMaterial_transparent);
		LordSafeDelete(m_StaticMaterial_xray);

		LordSafeDelete(m_pMergeMeshManager);
		LordSafeDelete(m_pAttackManager);

		LordSafeDelete(m_pTemplateDataManager);

		if (TextureManager::Instance()) {
			TextureManager::Instance()->releaseResource(s_StoneTexture);
			s_StoneTexture = NULL;
			TextureManager::Instance()->releaseResource(s_FrozenTexture);
			s_FrozenTexture = NULL;
			TextureManager::Instance()->releaseResource(s_PoisonTexture);
			s_PoisonTexture = NULL;
		}
	}
	//---------------------------------------------------------------------
	SceneNode* ActorManager::GetActorRootSceneNode(void)
	{
		if (!mActorRoot)
		{
			mActorRoot = LordNew SceneNode;
		}

		return mActorRoot;
	}
	//---------------------------------------------------------------------
	void ActorManager::FreeActors(void)
	{
		// 清理Actor
		for (ActorSet::iterator it = mActorActiveSet.begin(); it != mActorActiveSet.end(); ++it)
		{
			// 切场景删Actor时ActorClient也删除
			LordDelete *it;
		}
		mActorActiveSet.clear();
		DestroyDelayActors(true);
	}
	//---------------------------------------------------------------------
	ActorObject* ActorManager::CreateActor(const String &actorTemplate, const String &initSkillName, int AnUserData, bool isUIActor, bool isMainActor, bool bSingleThread, bool isHide)
	{
		ActorTemplateData* pActorTemplateData = m_pTemplateDataManager->LoadByFile(actorTemplate);

		if (!pActorTemplateData)
		{
			String msg = "ActorTemplate ";
			msg += actorTemplate;
			msg += " is not loaded when creating actor";
			LordLogError("ActorManager::CreateActor: %s", msg.c_str());

			return NULL;
		}

		ActorObject* pActor = LordNew ActorObject(isUIActor, (Root::Instance()->isEditorMode() || bSingleThread || isUIActor), AnUserData, isHide);
		if (!pActor)
		{
			LordLogError("ActorManager::CreateActor: ActorObject construct faile");
		}

		pActor->Init(pActorTemplateData, isMainActor);

		if (pActor->IsSkillExist(initSkillName))
		{
			pActor->m_defaultIdle = initSkillName;
		}
		else
		{
			if (StringUtil::Find(initSkillName, "idle_"))
			{
				if (pActor->IsSkillExist("idle"))
				{
					pActor->m_defaultIdle = "idle";
				}
				else
				{
					pActor->m_defaultIdle = "";// such as to furniture
				}
			}
		}

		//pActor->SetTimeScale(m_fTimeScale, true);
		//pActor->requestMergeMeshesEvent(); // 发起模型合并请求
		if (pActor->m_defaultIdle != "")
		{
			pActor->PlaySkill(pActor->m_defaultIdle);
		}
		// 2. 添加到活动集合
		mActorActiveSet.insert(pActor);
		//LordLogInfo("----------[CreateActor] actor(%x)", (ui32)pActor);
		return pActor;
	}

	ActorObject* ActorManager::CreateActor(const String& actorTemplate, const String& initSkillName, bool isUIActor, bool needUpdate)
	{
		ActorObject* result = CreateActor(actorTemplate, initSkillName, 0, isUIActor, true);
		if (needUpdate && result)
		{
			result->m_needUpdate = true;
		}
		return result;
	}

	//---------------------------------------------------------------------
	void ActorManager::DestroyActor(ActorObject *pActor, bool mark)
	{

		//LordLogInfo("----------[DestroyActor] actor(%x)", pActor);
		LordAssert(pActor);
		if (mark)
		{
			pActor->m_MarkDestroy = true;
			pActor->StopSkill(true);
		}
		else
		{
			ActorSet::iterator itActor = mActorActiveSet.find(pActor);
			if (itActor != mActorActiveSet.end())
			{
				mActorActiveSet.erase(itActor);
				mDestoryActorSet.insert(pActor);
			}
		}
	}
	//---------------------------------------------------------------------
	bool ActorManager::ValidActor(ActorObject* pActor)
	{
		ActorSet::iterator itActor = mActorActiveSet.find(pActor);
		return  (itActor != mActorActiveSet.end());
	}
	//---------------------------------------------------------------------
	ActorIterator ActorManager::GetActorBegin()
	{
		return mActorActiveSet.begin();
	}
	//---------------------------------------------------------------------
	ActorIterator ActorManager::GetActorEnd()
	{
		return mActorActiveSet.end();
	}
	//---------------------------------------------------------------------
	void ActorManager::SetSpeedUp(float speedMultipler)
	{
		m_fTimeScale = speedMultipler;
		/*for (ActorSet::const_iterator it=mActorActiveSet.begin(); it!=mActorActiveSet.end(); ++it)
		{
		ActorObject *pActor = *it;
		if (pActor)
		pActor->SetTimeScale(speedMultipler, true);
		}*/
		//直接通过ROOT修改frameTime

	}

	ActorObject* ActorManager::RayPickActor(const LORD::Ray& ray) const
	{
		vector<ActorPickInfo>::type actorPickResults;
		// 通过ActorManager查询可防止重复
		for (ActorSet::const_iterator it = mActorActiveSet.begin(); it != mActorActiveSet.end(); ++it)
		{
			ActorObject* pObject = *it;
			if (!pObject->m_isHide && !pObject->m_isBackup)
			{
				LORD::HitInfo hitInfo;
				std::pair<bool, LORD::Real> result;
				result.first = false;
				result.second = 0.0f;

				result.first = ray.hitBox((*it)->getActorAABB(), result.second, hitInfo);
				if (result.first)
				{
					ActorPickInfo pickInfo;
					pickInfo.pActor = *it;
					pickInfo.distance = result.second;
					actorPickResults.push_back(pickInfo);
				}
			}
		}

		if (!actorPickResults.empty())
		{
			// 排序
			std::sort(actorPickResults.begin(), actorPickResults.end());
			return actorPickResults[0].pActor;
		}
		return NULL;
	}

	ActorObject* ActorManager::RayPickActor(float screenPosX, float screenPosY) const
	{
		Ray ray;
		SceneManager::Instance()->getMainCamera()->getCameraRay(ray, Vector2(screenPosX, screenPosY));

		return RayPickActor(ray);
	}

	void ActorManager::DestroyDelayActors(bool bSingleThread)
	{
		ActorSet reserveActor;
		ActorSet::iterator itActor = mDestoryActorSet.begin();
		for (; itActor != mDestoryActorSet.end(); ++itActor)
		{
			ActorObject *pActor = *itActor;

			if (pActor->mbSingleThread || bSingleThread)
			{
				//LordLogInfo("----------[singleThread Call] actor(%x)", pActor);
				pActor->EnableAllBodyParts(false);
				pActor->bodyPartString = "Delete by singleThread Call";
				LordSafeDelete(pActor);
			}
			else
			{
				if (pActor->mergeFinished())
				{
					//LordLogInfo("----------[Create ActorPreDestoryEvent] actor(%x)", pActor);
					StreamThread* pThread = StreamThread::Instance();
					if (pThread && pThread->IsRunning())
					{
						ActorPreDestoryEvent* pEvent = LordNew ActorPreDestoryEvent(pActor);
						pThread->AddRequest(pEvent);
					}
				}
				else
				{
					reserveActor.insert(pActor);
				}
			}
		}
		mDestoryActorSet.clear();
		mDestoryActorSet.insert(reserveActor.begin(), reserveActor.end());
	}

	void ActorManager::AddCrameraShakeToAllActors(float scale, float durationTime, int shakeTimes)
	{
		for (ActorSet::const_iterator it = mActorActiveSet.begin(); it != mActorActiveSet.end(); ++it)
		{
			if ((*it)->mSkillSystem.GetCameraShakeEnable())
			{
				(*it)->AddCameraShake(0.0, scale, durationTime, shakeTimes);
			}
		}
	}

	void ActorManager::SetSelectEffect(const vector<String>::type& effectNames, const Quaternion& quan)
	{
		vector<String>::type::const_iterator it = effectNames.begin();
		for (; it != effectNames.end(); ++it)
		{
			EffectSystem* pEffect = EffectSystemManager::Instance()->createEffectSystem(*it, *it);
			SceneNode* pNode = GetActorRootSceneNode()->createChild();
			pNode->setWorldOrientation(quan);
			pNode->update();
			pEffect->setParentSceneNode(pNode);
			mSelectEffectSet.insert(pEffect);
		}
	}

	EffectSystem* ActorManager::GetSelectEffect(const String& effectName, const String& templateName)
	{
		EffectSystem* pEffect;
		set<EffectSystem*>::type::const_iterator it = mSelectEffectSet.begin();
		for (; it != mSelectEffectSet.end(); ++it)
		{
			pEffect = *it;
			if (pEffect->getName() == effectName)
			{
				return pEffect;
			}
		}

		pEffect = EffectSystemManager::Instance()->createEffectSystem(effectName, templateName);
		SceneNode* pNode = GetActorRootSceneNode()->createChild();
		pEffect->setParentSceneNode(pNode);
		mSelectEffectSet.insert(pEffect);

		return pEffect;
	}

	void ActorManager::AddCrameraPushToAllActors(LORD::ActorTemplateData::CameraPush* pData)
	{
		for (ActorSet::const_iterator it = mActorActiveSet.begin(); it != mActorActiveSet.end(); ++it)
		{
			if ((*it)->mSkillSystem.GetCameraShakeEnable())
			{
				(*it)->AddCameraPush(pData);
			}
		}
	}

	void ActorManager::DestroyActorByID(const String& nameid)
	{
		for (ActorSet::iterator it = mActorActiveSet.begin(); it != mActorActiveSet.end(); ++it)
		{
			ActorObject* pActor = *it;
			if (pActor->getActorNameID() == nameid)
			{
				mActorActiveSet.erase(it);
			}
			DestroyActor(pActor);
		}
	}

	void ActorManager::UpdateActors(ui32 delta)
	{
		for (ActorSet::iterator it = mActorActiveSet.begin(); it != mActorActiveSet.end(); )
		{
			ActorObject* pActor = *it;
			if (pActor->m_MarkDestroy)
			{
				mActorActiveSet.erase(it++);
				mDestoryActorSet.insert(pActor);
			}
			else
			{
				if (!pActor->isUpdateManually())
					pActor->Update(delta);
				++it;
			}
		}
	}

	void ActorManager::SetActorsHide(bool bHide, const String& nameArray)
	{
		if (nameArray == "all")
		{
			for (ActorSet::iterator it = mActorActiveSet.begin(); it != mActorActiveSet.end(); ++it)
			{
				(*it)->setCamAnimHide(bHide);
			}
		}
		else
		{
			StringArray array = StringUtil::Split(nameArray, "|", NextActorId);
			for (ActorSet::iterator it = mActorActiveSet.begin(); it != mActorActiveSet.end(); ++it)
			{
				ActorObject* pActor = *it;
				StringArray::const_iterator itfind = std::find(array.begin(), array.end(), pActor->getActorNameID());
				if (itfind == array.end())
				{
					pActor->setCamAnimHide(bHide);
				}
			}
		}
	}

	void ActorManager::createMaterials()
	{
		m_ActorRenderQueue = SceneManager::Instance()->getRenderQueue("Scene");
		m_TranslateRenderQueue = SceneManager::Instance()->getRenderQueue("SkinnedMeshSpecial");

		ShaderProgramManager* programManager = ShaderProgramManager::Instance();

		m_SkinMaterial = LordNew Material;
		m_SkinMaterial_xray = LordNew Material;
		m_SkinMaterial_depth = LordNew Material;
		m_SkinMaterial_transparent = LordNew Material;

		RasterizerState::RasterizerDesc rastate = m_SkinMaterial->getRasterizerState()->getDesc();
		rastate.cullMode = RasterizerState::CULL_BACK;
		SamplerState::SamplerDesc samdesc = m_SkinMaterial->getSamplerState(0)->getDesc();
		samdesc.minFilter = SamplerState::FO_LINEAR;
		samdesc.magFilter = SamplerState::FO_LINEAR;
		samdesc.mipFilter = SamplerState::FO_NONE;
		samdesc.addrUMode = SamplerState::AM_MIRROR;
		samdesc.addrVMode = SamplerState::AM_MIRROR;
		m_SkinMaterial->setRasterizerState(rastate);
		m_SkinMaterial->setSamplerState(0, samdesc);
		m_SkinMaterial->setSamplerState(1, samdesc);

		BlendState::BlendDesc xrayblend = m_SkinMaterial_xray->getBlendState()->getDesc();
		xrayblend.bBlendEnable = true;
		xrayblend.srcBlend = BlendState::BF_SRC_ALPHA;
		xrayblend.dstBlend = BlendState::BF_INV_DST_ALPHA;
		DepthStencilState::DepthStencilDesc xraydepth = m_SkinMaterial_xray->getDepthState()->getDesc();
		xraydepth.bDepthEnable = true;
		xraydepth.bWriteDepth = false;
		xraydepth.depthFunc = DepthStencilState::CF_GREATER;
		m_SkinMaterial_xray->setBlendState(xrayblend);
		m_SkinMaterial_xray->setRasterizerState(rastate);
		m_SkinMaterial_xray->setDepthState(xraydepth);

		BlendState::BlendDesc depthblend = m_SkinMaterial_depth->getBlendState()->getDesc();
		depthblend.colorWriteMask = BlendState::CMASK_NONE;
		RasterizerState::RasterizerDesc depthraster = m_SkinMaterial_depth->getRasterizerState()->getDesc();
		depthraster.cullMode = RasterizerState::CULL_BACK;
		m_SkinMaterial_depth->setBlendState(depthblend);
		m_SkinMaterial_depth->setRasterizerState(depthraster);

		BlendState::BlendDesc transparenblend = m_SkinMaterial_transparent->getBlendState()->getDesc();
		transparenblend.bBlendEnable = true;
		transparenblend.blendOP = BlendState::BOP_ADD;
		transparenblend.srcBlend = BlendState::BF_SRC_ALPHA;
		transparenblend.dstBlend = BlendState::BF_INV_SRC_ALPHA;
		transparenblend.alphaBlendOP = BlendState::BOP_ADD;
		transparenblend.srcAlphaBlend = BlendState::BF_ONE;
		transparenblend.dstAlphaBlend = BlendState::BF_ZERO;
		DepthStencilState::DepthStencilDesc transparendepth = m_SkinMaterial_transparent->getDepthState()->getDesc();
		transparendepth.depthFunc = DepthStencilState::CF_LESS_EQUAL;
		m_SkinMaterial_transparent->setBlendState(transparenblend);
		m_SkinMaterial_transparent->setRasterizerState(depthraster);
		m_SkinMaterial_transparent->setDepthState(transparendepth);
		m_SkinMaterial_transparent->setSamplerState(0, samdesc);
		m_SkinMaterial_transparent->setSamplerState(1, samdesc);

		m_StaticMaterial = LordNew Material;
		m_StaticMaterial_xray = LordNew Material;
		//m_StaticMaterial_depth = LordNew Material;
		m_StaticMaterial_transparent = LordNew Material;

		m_StaticMaterial->setSamplerState(0, samdesc);
		m_StaticMaterial->setSamplerState(1, samdesc);

		m_StaticMaterial_xray->setBlendState(xrayblend);
		m_StaticMaterial_xray->setRasterizerState(rastate);
		m_StaticMaterial_xray->setDepthState(xraydepth);

		m_StaticMaterial_transparent->setBlendState(transparenblend);
		m_StaticMaterial_transparent->setRasterizerState(depthraster);
		m_StaticMaterial_transparent->setSamplerState(0, samdesc);
		m_StaticMaterial_transparent->setSamplerState(1, samdesc);

		bool bRenderHighLevel = SceneManager::Instance()->isRenderActorHigh();
		bool bComputeFog = SceneManager::Instance()->isComputeActorFog();
		if (bRenderHighLevel)
		{
			m_SkinMaterial->setShaderProgram(programManager->getShaderProgram("SkinedMeshForActorHigh"));
			m_StaticMaterial->setShaderProgram(programManager->getShaderProgram("StaticMeshForActorHigh"));
		}
		else if (bComputeFog)
		{
			m_SkinMaterial->setShaderProgram(programManager->getShaderProgram("SkinedMeshForActorFog"));
			m_StaticMaterial->setShaderProgram(programManager->getShaderProgram("StaticMeshForActorFog"));
		}
		else
		{
			m_SkinMaterial->setShaderProgram(programManager->getShaderProgram("SkinedMeshForActor"));
			m_StaticMaterial->setShaderProgram(programManager->getShaderProgram("StaticMeshForActor"));
		}

		m_SkinMaterial_xray->setShaderProgram(programManager->getShaderProgram("SkinedMeshForActorXray"));
		m_SkinMaterial_depth->setShaderProgram(programManager->getShaderProgram("SkinedMeshForActor_WriteZ"));
		m_SkinMaterial_transparent->setShaderProgram(programManager->getShaderProgram("SkinedMeshForActorTransparent"));

		m_StaticMaterial_xray->setShaderProgram(programManager->getShaderProgram("StaticMeshForActorXray"));
		m_StaticMaterial_transparent->setShaderProgram(programManager->getShaderProgram("StaticMeshForActorTransparent"));
	}

	void ActorManager::stopCameraAnims()
	{
		for (ActorSet::iterator it = mActorActiveSet.begin(); it != mActorActiveSet.end(); ++it)
		{
			(*it)->stopCameraAnim();
		}
	}

}