/********************************************************************
filename: 	EffectSystemManager.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_SYSTEM_MANAGER_H__
#define __LORD_EFFECT_SYSTEM_MANAGER_H__

#include "Core.h"
#include "EffectHeader.h"
#include "EffectController.h"
#include "EffectLayer.h"
#include "Object/Singleton.h"
#include "Util/StringUtil.h"
#include "Object/Singleton.h"
#include "Render/RenderState.h"
#include "Render/RenderQueue.h"

namespace LORD
{
	class EffectSystemManager;

	class  LORD_CORE_API EffectManualRender : public ManualRenderListener
	{
	public:
		virtual void render(bool uiRender);
		EffectSystemManager* m_pEffectSystemManager;
	};

	class  LORD_CORE_API EffectManualRender_Special : public ManualRenderListener
	{
	public:
		virtual void render(bool uiRender);
		EffectSystemManager* m_pEffectSystemManager;
	};

	class  LORD_CORE_API EffectSystemManager : public Singleton<EffectSystemManager> , public ObjectAlloc
	{
		friend class EffectDestroyEvent;
	public:
		static inline EffectSystemManager* Instance()
		{
			return ms_pSingleton;
		}

	protected:
		EffectSystem* _createSystemImpl(const String& name);
		void _destroySystemImpl(EffectSystem* particleSystem);

	protected:
		static const size_t sRedneralbeNum;
		static const size_t sRenderalbeQuadSize;

		bool				mRenderalbeSetCreated;
		bool				mEnableRender;
		bool				mEnableDistortionRender;
		
		int					mSPMatIndex;
		int					mSPTextureIndex;

		void*				m_VertexSysMemory;
		GPUBuffer*			m_VertexBuffer;

		// common things.
		RenderInput*		mEffectRenderInput;
		SamplerState*		mEffectSamplerState;
		BlendState*			mEffectAlphaLighting;
		BlendState*			mEffectHighLighting;
		DepthStencilState*	mEffectDepthState;
		RasterizerState*	mEffectRasterizerState;

		Material*			mMtlDistortion;
		Texture*			mTextureNoise;

		EffectManualRender  mEffectManualRender;
		EffectManualRender_Special mEffectManualRender_Special;

		RenderInput::VertexElementList m_VertexElement;
		typedef list<EffectRenderable*>::type RenderableList;
		RenderableList mUnUsedRenderables;

		typedef map<String, EffectSystem*>::type EffectSystemMap;
		EffectSystemMap mEffectSystems;
		EffectSystemMap mEffectSystemsChilds;
		EffectSystemMap mEffectSystemTemplates;

		typedef map<ui32, vector<EffectRenderable*>::type>::type RenderableMap;
		RenderableMap mUsedRenderables;

	public:
		EffectSystemManager (void);
		~EffectSystemManager (void);


		EffectLayer* createLayer(EFFECT_LAYER_TYPE layerType);
		EffectLayer* cloneLayer(EffectLayer* layer);
		void destroyLayer(EffectLayer* layer);
		// for editor
		EFFECT_LAYER_TYPE getLayerType(const String& typeName);
		String getLayerTypeString(EFFECT_LAYER_TYPE type);

		EffectKeyFrame* createKeyFrame(void);
		EffectKeyFrame* cloneKeyFrame(EffectKeyFrame* keyFrame);
		void destroyKeyFrame(EffectKeyFrame* keyFrame);

		EffectParticleControllerSet* createParticleControllerSet(void);
		EffectParticleControllerSet* cloneParticleControllerSet(EffectParticleControllerSet* particleControllerSet);
		void destroyParticleControllerSet(EffectParticleControllerSet* particleControllerSet);

		EffectMaterial* createMaterial();
		EffectMaterial* cloneMaterial(EffectMaterial* material);
		void destroyMaterial(EffectMaterial* material);

		EffectController* createController(EFFECT_CONTROL_TYPE controlType);
		EffectController* cloneController(EffectController* controller);
		void destroyController(EffectController* controller);
		void getControllerTypes(StringArray& stringArry);
		// for editor
		EFFECT_CONTROL_TYPE getControllerType(const String& typeName);
		String getControllerTypeString(EFFECT_CONTROL_TYPE type);

		// for editor
		void replaceEffectSystemTemplate(const String& name, EffectSystem* system);
		EffectSystem* createEffectSystemTemplate(const String& name);
		EffectSystem* getEffectSystemTemplate(const String& templateName);
		EffectSystem* parseEffectTemplate(const String& templateName);
		void destroyEffectSystemTemplate(const String& templateName);
		void destroyAllEffectSystemTemplates(void);

		EffectSystem* createEffectSystem(const String& name, const String& templateName, bool isUIeffect = false);
		EffectSystem* createEffectSystem(const String& name);
		EffectSystem* getEffectSystem(const String& name);
		void destroyEffectSystem(EffectSystem* particleSystem);
		void destroyEffectSystemSingleThread(EffectSystem* particleSystem);

		void destroyAllEffectSystems();

		void createRenderableSet();
		void destroyRenderableSet();
		EffectRenderable* getRenderable(Texture* texture, EFFECT_RENDER_GROUP rg, EFFECT_BLEND_MODE bm, EffectMaterial* pEm, int vertexNum = 0, int indexNum = 0, bool isViewSpace = false);
		void beginRender();
		void endRender(bool isUIEffect = false, const Matrix4* viewproj = NULL);
		void beginRender_special();
		void endRender_special(bool uiRender);
		void renderDistortionLayer();
		void renderDistortionedEffectLayer();

		void updateVertexBuffer();
		
		void setEnableRender( bool _val )
		{
			mEnableRender = _val;

			if( !mEnableRender )
			{
				mEnableDistortionRender = false;
			}
		}

		bool getEnableRender() const { return mEnableRender; }

		void setEnableDistortionRender( bool _val )
		{
			if( _val && !mEnableRender )
			{
				return ;
			}

			mEnableDistortionRender = _val;
		}

		inline bool getEnableDistortionRender() const { return mEnableDistortionRender;	}

		SamplerState* getSamplerState() const { return mEffectSamplerState; }
	};
}
#endif
