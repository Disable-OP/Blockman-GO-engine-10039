#include "Core.h"
#include "EffectSystemManager.h"
#include "EffectSystem.h"
#include "EffectParticleControllerSet.h"
#include "EffectLayer.h"
#include "EffectMaterial.h"
#include "Object/Exception.h"
#include "Render/Renderer.h"
#include "Render/ShaderProgram.h"
#include "Render/ShaderProgramManager.h"
#include "Scene/SceneManager.h"
#include "EffectLayer2DBillBoard.h"
#include "EffectLayer3DBillBoard.h"
#include "EffectLayerColumn.h"
#include "EffectLayerTrail.h"
#include "EffectLayerParaboloidal.h"
#include "EffectLayer3DGrid.h"
#include "EffectLayer2DGrid.h"
#include "EffectLayerLighting.h"
#include "EffectLayerRope.h"
#include "EffectLayerParticlesPoint.h"
#include "EffectLayerParticlesCube.h"
#include "EffectLayerParticlesSphere.h"
#include "EffectLayerParticlesCylinder.h"
#include "EffectLayerParticlesCurve.h"
#include "EffectLayerModel.h"
#include "EffectLayerSkin.h"
#include "EffectLayerSpriteAnimation.h"
#include "EffectLayerChild.h"
#include "Resource/ResourceGroupManager.h"
#include "Render/Material.h"
#include "Render/RenderTarget.h"
#include "Render/TextureManager.h"

namespace LORD
{
	void EffectManualRender::render(bool uiRender)
	{
		m_pEffectSystemManager->endRender(uiRender);
	}

	void EffectManualRender_Special::render(bool uiRender)
	{
		m_pEffectSystemManager->endRender_special(uiRender);
	}

//#define MERGE_VERTEX

	// all effect render vertices cast 1M memory
	const size_t EffectSystemManager::sRedneralbeNum = 32;
	const size_t EffectSystemManager::sRenderalbeQuadSize = 2000;

	//---------------------------------------------------------------------
	/*EffectSystemManager* EffectSystemManager::Instance()
	{
		return ms_pSingleton;
	}*/

	EffectSystemManager::EffectSystemManager (void)
		: mEffectRenderInput(NULL)
		, mEffectSamplerState(NULL)
		, mEffectAlphaLighting(NULL)
		, mEffectHighLighting(NULL)
		, mEffectDepthState(NULL)
		, mEffectRasterizerState(NULL)
		, mRenderalbeSetCreated(false)
		, mMtlDistortion(0)
		, mTextureNoise(0)
		, mEnableRender( true )
		, mEnableDistortionRender(false)
		, m_VertexSysMemory(NULL)
		, m_VertexBuffer(NULL)
	{
		// nothing to do
		mEffectManualRender.m_pEffectSystemManager = this;
		mEffectManualRender_Special.m_pEffectSystemManager = this;
	}
	//-----------------------------------------------------------------------
	EffectSystemManager::~EffectSystemManager (void)
	{
		destroyAllEffectSystemTemplates();
		destroyAllEffectSystems();
		destroyRenderableSet();
		LordSafeDelete(mMtlDistortion);

	}
	//-----------------------------------------------------------------------
	void EffectSystemManager::destroyEffectSystemTemplate(const String& templateName)
	{		
		EffectSystemMap::iterator t = mEffectSystemTemplates.find(templateName);
		if (t != mEffectSystemTemplates.end())
		{
			LordDelete t->second;
			mEffectSystemTemplates.erase(t);
		}
	}
	//-----------------------------------------------------------------------
	void EffectSystemManager::destroyAllEffectSystemTemplates() 
	{ 
		EffectSystemMap::iterator t;
		for (t = mEffectSystemTemplates.begin(); t != mEffectSystemTemplates.end(); ++t)
		{
			LordDelete t->second;
		}
		mEffectSystemTemplates.clear();
	}
	//-----------------------------------------------------------------------
	EffectKeyFrame* EffectSystemManager::createKeyFrame(void)
	{
		EffectKeyFrame* keyFrame = LordNewT(EffectKeyFrame);
		return keyFrame;
	}
	EffectKeyFrame* EffectSystemManager::cloneKeyFrame(EffectKeyFrame *keyFrame)
	{
		if(!keyFrame)
			return 0;

		EffectKeyFrame* clonekeyframe = createKeyFrame();
		keyFrame->copyAttributesTo(clonekeyframe);
		return clonekeyframe;
	}
	void EffectSystemManager::destroyKeyFrame(EffectKeyFrame* keyFrame)
	{
		LordDeleteT(keyFrame, EffectKeyFrame);
	}
	//-----------------------------------------------------------------------
	EffectParticleControllerSet* EffectSystemManager::createParticleControllerSet(void)
	{
		EffectParticleControllerSet* particleControllerSet = 
			LordNewT(EffectParticleControllerSet);
		return particleControllerSet;
	}
	EffectParticleControllerSet* EffectSystemManager::cloneParticleControllerSet(
		EffectParticleControllerSet* particleControllerSet)
	{
		if(!particleControllerSet)
			return 0;

		EffectParticleControllerSet* cloneparticleControllerSet = createParticleControllerSet();
		particleControllerSet->copyAttributesTo(cloneparticleControllerSet);
		return cloneparticleControllerSet;
	}
	void EffectSystemManager::destroyParticleControllerSet(EffectParticleControllerSet* particleControllerSet)
	{
		LordDeleteT(particleControllerSet, EffectParticleControllerSet);
	}
	//-----------------------------------------------------------------------
	EffectMaterial* EffectSystemManager::createMaterial()
	{
		EffectMaterial* material = LordNewT(EffectMaterial);
		return material;
	}
	EffectMaterial* EffectSystemManager::cloneMaterial(EffectMaterial* material)
	{
		if(!material)
			return 0;

		EffectMaterial* cloneMaterial = createMaterial();
		material->copyAttributesTo(cloneMaterial);
		return cloneMaterial;
	}
	void EffectSystemManager::destroyMaterial(EffectMaterial* material)
	{
		LordDeleteT(material, EffectMaterial);
	}
	EffectLayer* EffectSystemManager::createLayer(EFFECT_LAYER_TYPE layerType)
	{
		EffectLayer* pResult = NULL;
		switch(layerType)
		{
		case ELT_2DBillboard:	pResult = LordNew EffectLayer2DBillBoard; break;
		case ELT_3DBillboard:	pResult = LordNew EffectLayer3DBillBoard; break;
		case ELT_Column:		pResult = LordNew EffectLayerColumn; break;
		case ELT_TRAIL:			pResult = LordNew EffectLayerTrail; break;
		case ELT_Paraboloidal:	pResult = LordNew EffectLayerParaboloidal; break;
		case ELT_3DEditableSurface: pResult = LordNew EffectLayer3DGrid; break;
		case ELT_2DEditableSurface: pResult = LordNew EffectLayer2DGrid; break;
		case ELT_Lighting:		pResult = LordNew EffectLayerLighting; break;
		case ELT_Rope:			pResult = LordNew EffectLayerRope; break;
		case ELT_ParticlesPoint: pResult = LordNew EffectLayerParticlesPoint; break;
		case ELT_ParticlesCube:	 pResult = LordNew EffectLayerParticlesCube; break;
		case ELT_ParticlesSphere: pResult= LordNew EffectLayerParticlesSphere; break;
		case ELT_ParticlesColumn: pResult = LordNew EffectLayerParticlesCylinder; break;
		case ELT_ParticlesCruve:  pResult = LordNew EffectLayerParticlesCurve; break;
		case ELT_Model:			pResult = LordNew EffectLayerModel; break;
		case ELT_Skin:			pResult = LordNew EffectLayerSkin; break;
		case ELT_SpriteAnimation: pResult = LordNew EffectLayerSpriteAnimation; break;
		case ELT_Child:			pResult = LordNew EffectLayerChild; break;
		default: break;
		}
		return pResult;
	}
	EffectLayer* EffectSystemManager::cloneLayer(EffectLayer* layer)
	{
		if (!layer)
			return 0;

		EffectLayer* clonedLayer = createLayer(layer->getLayerType());
		layer->copyAttributesTo(clonedLayer);
		return clonedLayer;
	}
	void EffectSystemManager::destroyLayer(EffectLayer* layer)
	{
		LordDelete layer;
	}
	EFFECT_LAYER_TYPE EffectSystemManager::getLayerType(const String& typeName)
	{
		if(typeName == "2DBillboard")		return ELT_2DBillboard;
		if(typeName == "3DBillboard")		return ELT_3DBillboard;
		if(typeName == "Trail")				return ELT_TRAIL;
		if(typeName == "Column")			return ELT_Column;
		if(typeName == "Paraboloidal")		return ELT_Paraboloidal;
		if(typeName == "ParticlesPoint")	return ELT_ParticlesPoint;
		if(typeName == "ParticlesCube")		return ELT_ParticlesCube;
		if(typeName == "ParticlesSphere")	return ELT_ParticlesSphere;
		if(typeName == "ParticleColumn")	return ELT_ParticlesColumn;
		if(typeName == "ParticlesCruve")	return ELT_ParticlesCruve;
		if(typeName == "Model")				return ELT_Model;
		if(typeName == "Skin" )				return ELT_Skin;
		if(typeName == "Lighting")			return ELT_Lighting;
		if(typeName == "Rope")				return ELT_Rope;
		if(typeName == "2DEditableSurface") return ELT_2DEditableSurface;
		if(typeName == "3DEditableSurface") return ELT_3DEditableSurface;
		if(typeName == "SpriteAnimation")	return ELT_SpriteAnimation;
		if(typeName == "Child")				return ELT_Child;
		return ELT_Unknow;
	}
	String EffectSystemManager::getLayerTypeString(EFFECT_LAYER_TYPE type)
	{
		switch(type)
		{
		case ELT_2DBillboard:		return "2DBillboard";
		case ELT_3DBillboard:		return "3DBillboard";
		case ELT_TRAIL:				return "Trail";
		case ELT_Column:			return "Column";
		case ELT_Paraboloidal:		return "Paraboloidal";
		case ELT_ParticlesPoint:	return "ParticlesPoint";
		case ELT_ParticlesCube:		return "ParticlesCube";
		case ELT_ParticlesSphere:	return "ParticlesSphere";
		case ELT_ParticlesColumn:	return "ParticleColumn";
		case ELT_ParticlesCruve:	return "ParticlesCruve";
		case ELT_Model:				return "Model";
		case ELT_Skin:				return "Skin";
		case ELT_SpriteAnimation:	return "SpriteAnimation";
		case ELT_Lighting:			return "Lighting";
		case ELT_Rope:				return "Rope";
		case ELT_2DEditableSurface:	return "2DEditableSurface";
		case ELT_3DEditableSurface:	return "3DEditableSurface";
		case ELT_Child:				return "Child";
		default:	return "Unknow";
		}
		return "Unknow";
	}
	EffectController* EffectSystemManager::createController(EFFECT_CONTROL_TYPE controlType)
	{
		EffectController* pResult = NULL;
		switch(controlType)
		{
		case ECT_Translation:		pResult = LordNew EffectTranslation; break;
		case ECT_RotateSelf:		pResult = LordNew EffectRotationSelf; break;
		case ECT_RotateAxis:		pResult = LordNew EffectRotationAxis; break;
		case ECT_RotateRevolution:	pResult = LordNew EffectRotationRevolution; break;
		case ECT_Offcenter:			pResult = LordNew EffectOffcenter; break;
		case ECT_ColorChange:		pResult = LordNew EffectColorChange; break;
		case ECT_ColorNoise:		pResult = LordNew EffectColorNoise; break;
		case ECT_ColorSet:			pResult = LordNew EffectColorSet; break;
		case ECT_ScaleChange:		pResult = LordNew EffectScaleChange; break;
		case ECT_ScaleNoise:		pResult = LordNew EffectScaleNoise; break;
		case ECT_ScaleSet:			pResult = LordNew EffectScaleSet; break;
		default: LordException("Error: no supperted controller type!"); 
		}
		return pResult;
	}
	EffectController* EffectSystemManager::cloneController(EffectController* controller)
	{
		if (!controller)
			return 0;

		EffectController* clonedController = createController(controller->getControllerType());
		controller->copyAttributesTo(clonedController);
		return clonedController;
	}
	void EffectSystemManager::destroyController(EffectController* controller)
	{
		LordDelete controller;
	}
	void EffectSystemManager::getControllerTypes(StringArray& stringArry)
	{
		stringArry.clear();
		stringArry.push_back("Translation");
		stringArry.push_back("RotateSelf");
		stringArry.push_back("RotateAxis");
		stringArry.push_back("RotateRevolution");
		stringArry.push_back("Offcenter");
		stringArry.push_back("ColorChange");
		stringArry.push_back("ColorNoise");
		stringArry.push_back("ColorSet");
		stringArry.push_back("ScaleChange");
		stringArry.push_back("ScaleNoise");
		stringArry.push_back("ScaleSet");
	}
	EFFECT_CONTROL_TYPE EffectSystemManager::getControllerType(const String& typeName)
	{
		if(typeName == "Translation") return ECT_Translation;
		if(typeName == "RotateSelf") return ECT_RotateSelf;
		if(typeName == "RotateAxis") return ECT_RotateAxis;
		if(typeName == "RotateRevolution") return ECT_RotateRevolution;
		if(typeName == "Offcenter") return ECT_Offcenter;
		if(typeName == "ColorChange") return ECT_ColorChange;
		if(typeName == "ColorNoise") return ECT_ColorNoise;
		if(typeName == "ColorSet") return ECT_ColorSet;
		if(typeName == "ScaleChange") return ECT_ScaleChange;
		if(typeName == "ScaleNoise") return ECT_ScaleNoise;
		if(typeName == "ScaleSet") return ECT_ScaleSet;
		return ECT_Unknow;
	}
	String EffectSystemManager::getControllerTypeString(EFFECT_CONTROL_TYPE type)
	{
		switch(type)
		{
		case ECT_Translation:		return "Translation";
		case ECT_RotateSelf:		return "RotateSelf";
		case ECT_RotateAxis:		return "RotateAxis";
		case ECT_RotateRevolution:	return "RotateRevolution";
		case ECT_Offcenter:			return "Offcenter";
		case ECT_ColorChange:		return "ColorChange";
		case ECT_ColorNoise:		return "ColorNoise";
		case ECT_ColorSet:			return "ColorSet";
		case ECT_ScaleChange:		return "ScaleChange";
		case ECT_ScaleNoise:		return "ScaleNoise";
		case ECT_ScaleSet:			return "ScaleSet";
		default: LordException("Error: no supperted controller type!"); 
		}
		return "Unknow";
	}
	//-----------------------------------------------------------------------
	void EffectSystemManager::replaceEffectSystemTemplate(const String& name, EffectSystem* system)
	{
		EffectSystem* systemTemplate = getEffectSystemTemplate(name);
		if (systemTemplate)
		{
			*systemTemplate = *system;
		}
	}
	EffectSystem* EffectSystemManager::createEffectSystemTemplate(const String& name)
	{
		// Validate name and add a prefix if needed.
		if(mEffectSystemTemplates.find(name) != mEffectSystemTemplates.end())
		{
			LordLogInfo("WARNING: Effect: Effect system template %s already exists. return older.", name.c_str());
			return NULL;
		}

		EffectSystem* particleSystemTemplate = LordNew EffectSystem(name);
		mEffectSystemTemplates[name] = particleSystemTemplate;
		return particleSystemTemplate;
	}
	//-----------------------------------------------------------------------
	EffectSystem* EffectSystemManager::parseEffectTemplate(const String& templateName)
	{
		// load the effect from sd-disk.
		DataStream* pEffectStream = ResourceGroupManager::Instance()->openResource(templateName);
		if (!pEffectStream)
		{
			//LordException("Effect file isn't exist. [%s]", templateName.c_str());
			LordLogError("Effect file isn't exist. [%s]", templateName.c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
			char buf[MAX_PATH];
			sprintf_s(buf, MAX_PATH, "Effect file isn't exist. [%s]", templateName.c_str());
			MessageBox(0, buf, "Error", 0);
#endif
			return NULL;
		}
		EffectSystem* pTemplate = LordNew EffectSystem(templateName);
		pTemplate->importData(pEffectStream);
		LordSafeDelete(pEffectStream);
		// insert the template.
		mEffectSystemTemplates[templateName] = pTemplate;

		return pTemplate;
	}
	//-----------------------------------------------------------------------
	EffectSystem* EffectSystemManager::getEffectSystemTemplate(const String& templateName)
	{
		EffectSystemMap::iterator i = mEffectSystemTemplates.find(templateName);
		if (i != mEffectSystemTemplates.end())
		{
			return i->second;
		}

		return 0;
	}
	//-----------------------------------------------------------------------
	EffectSystem* EffectSystemManager::createEffectSystem(const String& name, const String& templateName,bool isUIeffect)
	{
		// Validate name
		if (mEffectSystems.find(name) != mEffectSystems.end())
		{
			//LordException("Error: Effect: EffectSystem with name %s already exists.", name.c_str());
			LordLogError("Error: Effect: EffectSystem with name %s already exists.", name.c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
			char buf[MAX_PATH];
			sprintf_s(buf, MAX_PATH, "EffectSystem with name %s already exists.", name.c_str());
			MessageBox(0, buf, "Error", 0);
#endif
			return NULL;
		}

		EffectSystem* pTemplate = getEffectSystemTemplate(templateName);
		if (!pTemplate)
		{
			pTemplate = parseEffectTemplate(templateName);
		}

		EffectSystem* system = nullptr;
		if (pTemplate)
		{
			system = LordNew EffectSystem(name, isUIeffect);
			// need to repared.
			system->setTemplateName(templateName);
			mEffectSystems[name] = system;
			*system = *pTemplate;
		}

		return system;
	}
	//-----------------------------------------------------------------------
	EffectSystem* EffectSystemManager::createEffectSystem(const String& name)
	{
		if (mEffectSystems.find(name) != mEffectSystems.end())
		{
			LordLogError("Error: Effect: EffectSystem with name %s already exists.", name.c_str());
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
			char buf[MAX_PATH];
			sprintf_s(buf, MAX_PATH, "EffectSystem with name %s already exists.", name.c_str());
			MessageBox(0, buf, "Error", 0);
#endif
			return NULL;
			//LordException("Error: Effect: EffectSystem with name %s already exists.", name.c_str());
		}

		EffectSystem* system = LordNew EffectSystem(name);
		// need to repared.
		mEffectSystems[name] = system;

		return system;
	}
	//-----------------------------------------------------------------------
	EffectSystem* EffectSystemManager::getEffectSystem(const String& name)
	{
		if (name == StringUtil::BLANK)
			return 0;

		EffectSystemMap::iterator i = mEffectSystems.find(name);
		if (i != mEffectSystems.end())
		{
			return i->second;
		}

		return 0;
	}
	//-----------------------------------------------------------------------
	void EffectSystemManager::destroyEffectSystem(EffectSystem* particleSystem)
	{
		if (!particleSystem || particleSystem->mIsDestroying)
			return;

		if(particleSystem->mIsPreparing)
		{			
			// add by ajohn for childlayer
			particleSystem->setParentSceneNode(NULL);

			EffectDestroyEvent* pEffectDestroyEvent = LordNew EffectDestroyEvent(particleSystem);
			StreamThread* streamThread = StreamThread::Instance();
			streamThread->AddRequest(pEffectDestroyEvent);
		}
		else
		{
			EffectSystemMap::iterator i = mEffectSystems.find(particleSystem->getName());
			if (i != mEffectSystems.end())
			{
				LordDelete particleSystem;
				mEffectSystems.erase(i);
			}
		}
	}
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	void EffectSystemManager::destroyEffectSystemSingleThread(EffectSystem* particleSystem)
	{
		if (!particleSystem)
			return;

		EffectSystemMap::iterator i = mEffectSystems.find(particleSystem->getName());
		if (i != mEffectSystems.end())
		{
			LordDelete particleSystem;
			mEffectSystems.erase(i);
		}
	}
	//-----------------------------------------------------------------------

	void EffectSystemManager::destroyAllEffectSystems()
	{ 
		// Delete all Effect Systems. This doesnot include the templates
		EffectSystemMap::iterator t;
		for (t = mEffectSystems.begin(); t != mEffectSystems.end(); ++t)
		{
			if (!t->second->isChild())
				LordDelete t->second;
		}
		mEffectSystems.clear();
	}
	//-----------------------------------------------------------------------
	EffectSystem* EffectSystemManager::_createSystemImpl(const String& name)
	{
		EffectSystem* sys = LordNew EffectSystem(name);

		mEffectSystems[name] = sys;
		return sys;
	}
	//-----------------------------------------------------------------------
	void EffectSystemManager::_destroySystemImpl(EffectSystem* particleSystem)
	{
		EffectSystemMap::iterator i = mEffectSystems.find(particleSystem->getName());
		if (i != mEffectSystems.end())
		{
			mEffectSystems.erase(i);
		}

		LordDelete particleSystem;
	}

	void EffectSystemManager::createRenderableSet()
	{
		if(!mRenderalbeSetCreated)
		{
			Renderer* pRenderer = Renderer::Instance();
			SceneManager* pSceneManager = SceneManager::Instance();
			RenderQueue* pRenderQueue = pSceneManager->getRenderQueue("Effect");
			RenderQueue* pQueue_BeforeActor = pSceneManager->getRenderQueue("EffectSpecial"); // 特例！ 角色半透明之前的特效渲染通道
			ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Effect");

			mSPMatIndex = shaderProgram->getParamPhysicsIndex("matWVP");
			mSPTextureIndex = shaderProgram->getParamPhysicsIndex("texSampler");

			RenderInput::VertexElement posElm, colorElm, uvElm;
			posElm.semantic = RenderInput::VS_POSITION;
			posElm.pixFmt = PF_RGB32_FLOAT;
			colorElm.semantic = RenderInput::VS_COLOR;
			colorElm.pixFmt = PF_RGBA8_UNORM;
			uvElm.semantic = RenderInput::VS_TEXCOORD;
			uvElm.pixFmt = PF_RG32_FLOAT;
			m_VertexElement.push_back(posElm);
			m_VertexElement.push_back(colorElm);
			m_VertexElement.push_back(uvElm);
			mEffectRenderInput = pRenderer->createRenderInput(m_VertexElement, shaderProgram);

			// Sampler State use default.
			SamplerState::SamplerDesc desc;
            desc.mipFilter = SamplerState::FO_NONE;
			mEffectSamplerState = pRenderer->createSamplerState(desc);

			// AlphaBlendState
			BlendState::BlendDesc bsDescAlphaLighting;
			bsDescAlphaLighting.bBlendEnable = true;
			bsDescAlphaLighting.srcBlend = BlendState::BF_SRC_ALPHA;
			bsDescAlphaLighting.dstBlend = BlendState::BF_INV_SRC_ALPHA;
			bsDescAlphaLighting.srcAlphaBlend = BlendState::BF_SRC_ALPHA;
			bsDescAlphaLighting.dstAlphaBlend = BlendState::BF_INV_SRC_ALPHA;
			mEffectAlphaLighting = pRenderer->createBlendState(bsDescAlphaLighting);

			// ColorBlendState
			BlendState::BlendDesc bsDescHighLighting;
			bsDescHighLighting.bBlendEnable = true;
			bsDescHighLighting.srcBlend = BlendState::BF_SRC_ALPHA;
			bsDescHighLighting.dstBlend = BlendState::BF_ONE;
			bsDescHighLighting.srcAlphaBlend = BlendState::BF_ONE;
			bsDescHighLighting.dstAlphaBlend = BlendState::BF_ZERO;
			mEffectHighLighting = pRenderer->createBlendState(bsDescHighLighting);

			// Depth State
			DepthStencilState::DepthStencilDesc dssDesc;
			dssDesc.bWriteDepth	= false;
			dssDesc.bDepthEnable = true;
			mEffectDepthState = pRenderer->createDepthStencilState(dssDesc);

			// Rasterizer State
			RasterizerState::RasterizerDesc rasDesc;
			rasDesc.cullMode = RasterizerState::CULL_NONE;
			mEffectRasterizerState = pRenderer->createRasterizerState(rasDesc);

#ifdef MERGE_VERTEX
			m_VertexSysMemory = LordMalloc(sRedneralbeNum*sRenderalbeQuadSize * 4 * sizeof(EffectVertexFmt));
			Buffer vertBuff(sRedneralbeNum*sRenderalbeQuadSize * 4 * sizeof(EffectVertexFmt), m_VertexSysMemory);
			m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, vertBuff);
			for (size_t i = 0; i < sRedneralbeNum; ++i)
			{
				EffectRenderable* pRenderable = LordNewT(EffectRenderable);
				pRenderable->createVertexPoolEx(m_VertexSysMemory, sRenderalbeQuadSize * 4, i);
				pRenderable->createIndexPoolEx(sRenderalbeQuadSize * 6);
				mUnUsedRenderables.push_back(pRenderable);
			}
#else
			for(size_t i=0; i<sRedneralbeNum; ++i)
			{
				EffectRenderable* pRenderable = LordNewT(EffectRenderable);
				pRenderable->createVertexPool(sRenderalbeQuadSize * 4);
				pRenderable->createIndexPool(sRenderalbeQuadSize * 6);
				mUnUsedRenderables.push_back(pRenderable);
			}
#endif // MERGE_VERTEX

			mRenderalbeSetCreated = true;

			// add the manual render process
			pRenderQueue->setManalRenderBegin(&mEffectManualRender);
			pQueue_BeforeActor->setManalRenderBegin(&mEffectManualRender_Special);

			if( mMtlDistortion == NULL )
			{
				mMtlDistortion = LordNew Material;
				LordAssert( mMtlDistortion );

				ShaderProgram* pShaderProgram = ShaderProgramManager::Instance()->getShaderProgram("DistortionEffect");
				mMtlDistortion->setShaderProgram(pShaderProgram);
				LordAssert( pShaderProgram );

				pShaderProgram->getParamPhysicsIndex("matWVP");
				pShaderProgram->getParamPhysicsIndex("texSampler");
				pShaderProgram->getParamPhysicsIndex("noiseSampler");
				pShaderProgram->getParamPhysicsIndex("distortionUVScaleOffset");
				pShaderProgram->getParamPhysicsIndex("scSampler");
				pShaderProgram->getParamPhysicsIndex("a_threshold_time");

			}

			if( mTextureNoise == NULL )
			{
				mTextureNoise = TextureManager::Instance()->createTexture("noise.jpg");
				mTextureNoise->load();
			}
		}
	}

	void EffectSystemManager::destroyRenderableSet()
	{
		if(mRenderalbeSetCreated)
		{
			m_VertexElement.clear();
			LordSafeFree(m_VertexSysMemory);
			LordSafeDelete(m_VertexBuffer);

			LordSafeDelete(mEffectRasterizerState);
			LordSafeDelete(mEffectDepthState);
			LordSafeDelete(mEffectHighLighting);
			LordSafeDelete(mEffectAlphaLighting);
			LordSafeDelete(mEffectSamplerState);
			EffectRenderable* pRenderable;
			for(RenderableList::iterator it=mUnUsedRenderables.begin(); it!=mUnUsedRenderables.end(); ++it)
			{
				pRenderable = *it;
				LordDeleteT(pRenderable, EffectRenderable);
			}
			mUnUsedRenderables.clear();

			for(RenderableMap::iterator jt = mUsedRenderables.begin(); jt!=mUsedRenderables.end(); ++jt)
			{
				vector<EffectRenderable*>::type::iterator iter = jt->second.begin();
				for (; iter != jt->second.end(); ++iter)
				{
					LordDeleteT(*iter, EffectRenderable);
				}
				jt->second.clear();
			}
			mUsedRenderables.clear();
			mEffectRenderInput->unbindGPUBuffers();
			LordSafeDelete(mEffectRenderInput);

			mRenderalbeSetCreated = false;
		}
	}

	EffectRenderable* EffectSystemManager::getRenderable(Texture* texture, EFFECT_RENDER_GROUP rg, EFFECT_BLEND_MODE bm, EffectMaterial* pEm, int vertexNum, int indexNum, bool isViewSpace)
	{
		ui32 testKey = 0;
		ui32 materialkey = (ui32)((ui64)texture);
		materialkey &= 0x3FFFFFFF;
		if(bm == HighLightMode) materialkey |= 0x40000000;
		if(rg == ERG_GROUPUP) materialkey |= 0x80000000;

		if( pEm->getDistortionEnable() )
		{
			materialkey |= 0xA0000000;
		}
		

		EffectRenderable* pRenderable = NULL;
		RenderableMap::iterator it = mUsedRenderables.find(materialkey);
		if (it == mUsedRenderables.end())
		{
			if (mUnUsedRenderables.empty())
				return pRenderable;
			pRenderable = mUnUsedRenderables.front();
			mUnUsedRenderables.pop_front();
			mUsedRenderables[materialkey] = { pRenderable };
			if (pRenderable)
			{
				pRenderable->setMaterialParam(texture, rg, bm, pEm);
				pRenderable->beginPolygon();
				pRenderable->setViewSpace(isViewSpace);
			}
		}
		else
		{
			bool usedRenderableIsAvailable = false;
			vector<EffectRenderable*>::type::iterator iter;
			for (iter = it->second.begin(); iter != it->second.end(); ++iter)
			{
				pRenderable = *iter;
				if (pRenderable && pRenderable->isViewSpace() == isViewSpace)
				{
					bool vertexIsAvailable = pRenderable->getVertexNum() + vertexNum < pRenderable->getVertexPoolSize();
					bool indexIsAvailable = pRenderable->getIndexNum() + indexNum < pRenderable->getIndexPoolSize();
					if (vertexIsAvailable && indexIsAvailable)
					{
						usedRenderableIsAvailable = true;
						break;
					}
				}
			}
			if (!usedRenderableIsAvailable)
			{
				if (mUnUsedRenderables.empty())
					return pRenderable;
				pRenderable = mUnUsedRenderables.front();
				mUnUsedRenderables.pop_front();
				it->second.push_back(pRenderable);
				if (pRenderable)
				{
					pRenderable->setMaterialParam(texture, rg, bm, pEm);
					pRenderable->beginPolygon();
					pRenderable->setViewSpace(isViewSpace);
				}
			}
		}

/*
		pRenderable->setDistortion(pEm->getDistortionEnable());

		if( pRenderable->getDistortion() )
		{
			pRenderable->setDistortionParam(pEm);
		}*/

		return pRenderable;
	}

	void EffectSystemManager::beginRender()
	{
		RenderableMap::iterator it;
		for(it=mUsedRenderables.begin(); it!=mUsedRenderables.end(); ++it)
		{
			vector<EffectRenderable*>::type::iterator iter;
			for (iter = it->second.begin(); iter != it->second.end(); ++iter)
			{
				mUnUsedRenderables.push_back(*iter);
			}
			it->second.clear();
		}
		mUsedRenderables.clear();
	}

	void EffectSystemManager::endRender(bool isUIEffect, const Matrix4* viewproj)
	{
		if( !mEnableRender )
		{
			beginRender();

			return ;
		}

		//flush all renderalbes to draw.
		if(mUsedRenderables.empty())
			return;

#ifdef MERGE_VERTEX
		updateVertexBuffer();
		mEffectRenderInput->bind();
#endif

		Renderer* pRender = Renderer::Instance();
		SceneManager* pSceneManager = SceneManager::Instance();
		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Effect");
		shaderProgram->bind();

		Camera* camera = isUIEffect ? SceneManager::Instance()->getGUICamera() : SceneManager::Instance()->getMainCamera();
		Matrix4 viewProjMat = viewproj ? *viewproj : camera->getViewProjMatrix();
		shaderProgram->setUniform(mSPMatIndex, &viewProjMat, SPT_MAT4, 1);
		shaderProgram->setUniform(mSPTextureIndex, (void*)&TEXTURE_UNIT0, SPT_TEXTURE, 1);
		shaderProgram->bindUniforms();

		pRender->setDepthStencilState(mEffectDepthState);
		pRender->setRasterizerState(mEffectRasterizerState);

		//render all used renderables
		RenderableMap::iterator it;
		EffectRenderable* pRenderable;
		EFFECT_BLEND_MODE blendMode;
		for(it=mUsedRenderables.begin(); it!=mUsedRenderables.end(); ++it)
		{
			vector<EffectRenderable*>::type::iterator iter;
			for (iter = it->second.begin(); iter != it->second.end(); ++iter)
			{
				pRenderable = *iter;
				
				if (!pRenderable)
				{
					continue;
				}

				Matrix4 viewProjMatTemp;
				if (pRenderable->isViewSpace())
				{
					viewProjMatTemp = viewproj ? *viewproj : camera->getViewProjMatrix2();
				}
				else
				{
					viewProjMatTemp = viewproj ? *viewproj : camera->getViewProjMatrix();
				}
				shaderProgram->setUniform(mSPMatIndex, &viewProjMatTemp, SPT_MAT4, 1);
				shaderProgram->bindUniforms();

				pRenderable->endPolygon();
				blendMode = pRenderable->getBlendMode();
				Texture* texture = pRenderable->getTexture();
				texture->setSamplerState(mEffectSamplerState);

				if (pRenderable->getDistortion())
				{
					continue;
				}

				pRender->setTexture(0, texture);

				if (blendMode == AlphaLightMode)
					pRender->setBlendState(mEffectAlphaLighting);
				else if (blendMode == HighLightMode)
					pRender->setBlendState(mEffectHighLighting);

#ifdef MERGE_VERTEX
				ui16 vertexNum = pRenderable->getVertexNum();
				if (vertexNum > 0)
				{
					pRender->render(mEffectRenderInput, pRenderable->getIndexSysMemory(), pRenderable->getIndexNum());
				}
			}
		}
		mEffectRenderInput->unbind();
#else
				mEffectRenderInput->unbindGPUBuffers();
				ui16 vertexNum = pRenderable->getVertexNum();
				if (vertexNum > 0)
				{
					mEffectRenderInput->bindGPUBuffers(pRenderable->getVertexBuffer(), sizeof(EffectVertexFmt), pRenderable->getIndexBuffer(), sizeof(Word));
					mEffectRenderInput->setVertexCount(pRenderable->getVertexNum());
					mEffectRenderInput->setIndexCount(pRenderable->getIndexNum());
					pRender->render(mEffectRenderInput);
				}
			}
		}
#endif

		if( !mEnableDistortionRender )
		{
			beginRender();
		}
	}

	void EffectSystemManager::beginRender_special()
	{
		RenderableMap::iterator it;
		for (it = mUsedRenderables.begin(); it != mUsedRenderables.end();)
		{
			vector<EffectRenderable*>::type::iterator iter;
			for (iter = it->second.begin(); iter != it->second.end();)
			{
				if ((*iter)->isSpecialRender())
				{
					mUnUsedRenderables.push_back(*iter);
					it->second.erase(iter++);
				}
				else
				{
					++iter;
				}
			}

			if (it->second.size() == 0)
			{
				mUsedRenderables.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}

	void EffectSystemManager::endRender_special(bool uiRender)
	{
		if (!mEnableRender)
		{
			beginRender_special();

			return;
		}

		//flush all renderalbes to draw.
		if (mUsedRenderables.empty())
			return;

#ifdef MERGE_VERTEX
		updateVertexBuffer();
		mEffectRenderInput->bind();
#endif

		Renderer* pRender = Renderer::Instance();
		SceneManager* pSceneManager = SceneManager::Instance();
		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Effect");
		shaderProgram->bind();

		//get the camera to fill the matrix.
		Matrix4 viewProjMat = SceneManager::Instance()->getMainCamera()->getViewProjMatrix();
		shaderProgram->setUniform(mSPMatIndex, &viewProjMat, SPT_MAT4, 1);
		shaderProgram->setUniform(mSPTextureIndex, (void*)&TEXTURE_UNIT0, SPT_TEXTURE, 1);
		shaderProgram->bindUniforms();

		pRender->setDepthStencilState(mEffectDepthState);
		pRender->setRasterizerState(mEffectRasterizerState);

		//render all used renderables
		RenderableMap::iterator it;
		EffectRenderable* pRenderable;
		Texture* texture;
		EFFECT_BLEND_MODE blendMode;
		for (it = mUsedRenderables.begin(); it != mUsedRenderables.end(); ++it)
		{
			vector<EffectRenderable*>::type::iterator iter;
			for (iter = it->second.begin(); iter != it->second.end(); ++iter)
			{
				pRenderable = *iter;

				if (!pRenderable)
				{
					continue;
				}

				Matrix4 viewProjMatTemp;
				if (pRenderable->isViewSpace())
				{
					viewProjMatTemp = SceneManager::Instance()->getMainCamera()->getViewProjMatrix2();
				}
				else
				{
					viewProjMatTemp = SceneManager::Instance()->getMainCamera()->getViewProjMatrix();
				}
				shaderProgram->setUniform(mSPMatIndex, &viewProjMatTemp, SPT_MAT4, 1);
				shaderProgram->bindUniforms();

				if (pRenderable->isSpecialRender())
				{
					pRenderable->endPolygon();
					blendMode = pRenderable->getBlendMode();
					texture = pRenderable->getTexture();

					if (pRenderable->getDistortion())
					{
						continue;
					}

					pRender->setTexture(0, texture);

					if (blendMode == AlphaLightMode)
						pRender->setBlendState(mEffectAlphaLighting);
					else if (blendMode == HighLightMode)
						pRender->setBlendState(mEffectHighLighting);

#ifdef MERGE_VERTEX
					ui16 vertexNum = pRenderable->getVertexNum();
					if (vertexNum > 0)
					{
						pRender->render(mEffectRenderInput, pRenderable->getIndexSysMemory(), pRenderable->getIndexNum());
					}
#else
					mEffectRenderInput->unbindGPUBuffers();
					ui16 vertexNum = pRenderable->getVertexNum();
					if (vertexNum > 0)
					{
						mEffectRenderInput->bindGPUBuffers(pRenderable->getVertexBuffer(), sizeof(EffectVertexFmt), pRenderable->getIndexBuffer(), sizeof(Word));
						mEffectRenderInput->setVertexCount(pRenderable->getVertexNum());
						mEffectRenderInput->setIndexCount(pRenderable->getIndexNum());
						pRender->render(mEffectRenderInput);
					}
#endif
				}
			}
		}
	}

	void EffectSystemManager::renderDistortionLayer()
	{
		if( !mEnableRender ) return ;

		if(mUsedRenderables.empty())
			return;

#ifdef MERGE_VERTEX
		updateVertexBuffer();
#endif

		Renderer* pRender = Renderer::Instance();
		ShaderProgram* shaderProgram = mMtlDistortion->getShaderProgram();
		shaderProgram->bind();

		pRender->setDepthStencilState(mEffectDepthState);

		//get the camera to fill the matrix.
		int texIndex = shaderProgram->getParamPhysicsIndex("texSampler");
		int scIndex = shaderProgram->getParamPhysicsIndex("scSampler");
		int noiseIndex = shaderProgram->getParamPhysicsIndex("noiseSampler");
		int offsetIndex = shaderProgram->getParamPhysicsIndex("distortionUVScaleOffset");
		int timeIndex = shaderProgram->getParamPhysicsIndex("a_threshold_time");
		Camera* camera = SceneManager::Instance()->getMainCamera();
		Matrix4 viewProjMat = camera->getViewProjMatrix();
		shaderProgram->setUniform(shaderProgram->getParamPhysicsIndex("matWVP"), &viewProjMat, SPT_MAT4, 1);
		shaderProgram->setUniform(texIndex, &TEXTURE_UNIT0, SPT_TEXTURE, 1);
		shaderProgram->setUniform(scIndex, &TEXTURE_UNIT1, SPT_TEXTURE, 1);
		shaderProgram->setUniform(noiseIndex, &TEXTURE_UNIT2, SPT_TEXTURE, 1);

		Texture* scTexture = RenderTargetManager::Instance()->getRenderTargetByID(RTI_SceneColorMap)->getBindTexture();
		LordAssert(scTexture);
		pRender->setTexture(1, scTexture);

		Renderer::Instance()->setRasterizerState(mEffectRasterizerState);
		//render all used renderables
		RenderableMap::iterator it;
		EffectRenderable* pRenderable;
		EFFECT_BLEND_MODE blendMode;

		for( it = mUsedRenderables.begin(); it != mUsedRenderables.end(); ++it )
		{
			vector<EffectRenderable*>::type::iterator iter;
			for (iter = it->second.begin(); iter != it->second.end(); ++iter)
			{
				pRenderable = *iter;

				if (!pRenderable)
				{
					continue;
				}

				Matrix4 viewProjMatTemp;
				if (pRenderable->isViewSpace())
				{
					viewProjMatTemp = camera->getViewProjMatrix2();
				}
				else
				{
					viewProjMatTemp = camera->getViewProjMatrix();
				}
				shaderProgram->setUniform(shaderProgram->getParamPhysicsIndex("matWVP"), &viewProjMatTemp, SPT_MAT4, 1);
				shaderProgram->bindUniforms();

				blendMode = pRenderable->getBlendMode();

				if (!pRenderable->getDistortion()) continue;

				Texture* noiseTexture = pRenderable->getDistortionTexturePtr();
				if (noiseTexture == NULL)
				{
					noiseTexture = mTextureNoise;
				}

				pRender->setTexture(0, pRenderable->getTexture());
				pRender->setTexture(2, noiseTexture);

				Vector4 alpha_threshold_time(pRenderable->getDistortionAlphaThreshold(), float(Root::Instance()->getFrameTime()), 0.0f, 0.0f);
				shaderProgram->setUniform(offsetIndex, (void *)&pRenderable->getDistortionUVScaleOffset(), SPT_VEC4, 1);
				shaderProgram->setUniform(timeIndex, (const void *)&alpha_threshold_time, SPT_VEC4, 1);
				shaderProgram->bindUniforms();

#ifdef MERGE_VERTEX
				ui16 vertexNum = pRenderable->getVertexNum();
				if (vertexNum > 0)
				{
					pRender->render(mEffectRenderInput, pRenderable->getIndexSysMemory(), pRenderable->getIndexNum());
				}
#else
				mEffectRenderInput->unbindGPUBuffers();
				ui16 vertexNum = pRenderable->getVertexNum();
				if (vertexNum > 0)
				{
					mEffectRenderInput->bindGPUBuffers(pRenderable->getVertexBuffer(), sizeof(EffectVertexFmt), pRenderable->getIndexBuffer(), sizeof(Word));
					mEffectRenderInput->setVertexCount(pRenderable->getVertexNum());
					mEffectRenderInput->setIndexCount(pRenderable->getIndexNum());
					pRender->render(mEffectRenderInput);
				}
#endif
			}
		}
//		beginRender();
	}

	void EffectSystemManager::renderDistortionedEffectLayer()
	{
		if( !mEnableRender )
		{
			return ;
		}

		if(mUsedRenderables.empty())
			return;

#ifdef MERGE_VERTEX
		updateVertexBuffer();
#endif

		Renderer* pRender = Renderer::Instance();
		SceneManager* pSceneManager = SceneManager::Instance();
		ShaderProgram* shaderProgram = ShaderProgramManager::Instance()->getShaderProgram("Effect");
	
		pRender->setDepthStencilState(mEffectDepthState);
		pRender->setRasterizerState(mEffectRasterizerState);

		//get the camera to fill the matrix.
		Matrix4 viewProjMat = SceneManager::Instance()->getMainCamera()->getViewProjMatrix();
		shaderProgram->setUniform(mSPMatIndex, &viewProjMat, SPT_MAT4, 1);
		shaderProgram->setUniform(mSPTextureIndex, (void*)&TEXTURE_UNIT0, SPT_TEXTURE, 1);
		shaderProgram->bindUniforms();

		//render all used renderables
		RenderableMap::iterator it;
		EffectRenderable* pRenderable;
		EFFECT_BLEND_MODE blendMode;
		for(it=mUsedRenderables.begin(); it!=mUsedRenderables.end(); ++it)
		{
			vector<EffectRenderable*>::type::iterator iter;
			for (iter = it->second.begin(); iter != it->second.end(); ++iter)
			{
				pRenderable = *iter;

				if (!pRenderable)
				{
					continue;
				}

				Matrix4 viewProjMatTemp;
				if (pRenderable->isViewSpace())
				{
					viewProjMatTemp = SceneManager::Instance()->getMainCamera()->getViewProjMatrix2();
				}
				else
				{
					viewProjMatTemp = SceneManager::Instance()->getMainCamera()->getViewProjMatrix();
				}
				shaderProgram->setUniform(mSPMatIndex, &viewProjMatTemp, SPT_MAT4, 1);
				shaderProgram->bindUniforms();

				blendMode = pRenderable->getBlendMode();

				if (!pRenderable->getDistortion()) continue;

				if (pRenderable->getOnyDistortionLayer())
				{
					LordAssert(pRenderable->getDistortion());

					continue;
				}

				pRender->setTexture(0, pRenderable->getTexture());

				if (blendMode == AlphaLightMode)
					pRender->setBlendState(mEffectAlphaLighting);
				else if (blendMode == HighLightMode)
					pRender->setBlendState(mEffectHighLighting);

#ifdef MERGE_VERTEX
				ui16 vertexNum = pRenderable->getVertexNum();
				if (vertexNum > 0)
				{
					pRender->render(mEffectRenderInput, pRenderable->getIndexSysMemory(), pRenderable->getIndexNum());
				}
#else
				mEffectRenderInput->unbindGPUBuffers();
				ui16 vertexNum = pRenderable->getVertexNum();
				if (vertexNum > 0)
				{
					mEffectRenderInput->bindGPUBuffers(pRenderable->getVertexBuffer(), sizeof(EffectVertexFmt), pRenderable->getIndexBuffer(), sizeof(Word));
					mEffectRenderInput->setVertexCount(pRenderable->getVertexNum());
					mEffectRenderInput->setIndexCount(pRenderable->getIndexNum());
					pRender->render(mEffectRenderInput);
				}
#endif
			}
		}

		beginRender();
	}

	void EffectSystemManager::updateVertexBuffer()
	{
		// update vertexdata
		mEffectRenderInput->unbindGPUBuffers();
		Buffer vertBuff(sRedneralbeNum*sRenderalbeQuadSize * 4*sizeof(EffectVertexFmt), m_VertexSysMemory);
		m_VertexBuffer->updateSubData(0, vertBuff);
		mEffectRenderInput->bindGPUBuffers(m_VertexBuffer, sizeof(EffectVertexFmt));
		mEffectRenderInput->setVertexCount(sRedneralbeNum*sRenderalbeQuadSize * 4);
	}
}
