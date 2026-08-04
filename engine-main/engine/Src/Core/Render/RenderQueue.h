/********************************************************************
filename: 	RenderQueue.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RENDERQUEUE_H__
#define __LORD_RENDERQUEUE_H__

namespace LORD
{
	//enum RenderQueueType
	//{
	//	"None" = 0,					  // useless		
	//	"BackgroundForEditor" = 1,   // rgb       
	//	"StaticMeshForScene",   // alpha test
	//	"StaticMeshNormalForScene",   // alpha test
	//	"StaticMeshNormalForSceneWithoutAlpha",   // no alpha
	//	"StaticMeshForActorXray",
	//	"SkinedMeshForActorXray",
	//	"SkinedMeshForActorGPU",  // no alpha
	//	"SkinedMeshForActorCPU",  // no alpha
	//	"SkinedMeshForActorGPUHigh", // 1.diffus RGB, 2. mask R specular, 3. normal no alpha
	//	"SkinedMeshForActorFogGPU",  // no alpha , compute fog
	//	"SkinedMeshForActorFogCPU",  // no alpha , compute fog
	//	"StaticMeshForActor",  // no alpha
	//	"StaticMeshForActorHigh",// 1.diffus RGB, 2. mask R specular, 3. normal no alpha
	//	"StaticMeshForActorFog",// compute fog
	//	"DynamicPointLightSimpleInfluenceActor",
	//	"StaticMeshForActorTransparent",
	//	"SkinedMeshForActorCPUTransparent",
	//	"SkinedMeshForActorGPUTransparent",
	//	//RENDER_QUEUE_skined_mesh_for_scene,   // alpha test
	//	"Terrain",               // alpha test
	//	"TerrainMesh",              // no alpha
	//	"TerrainDecal",             // alpha test
	//	"Effect",                 // alpha blend
	//	"UI",                   // alpha blend
	//	"Font",
	//	"HelperForEditor",
	//	RENDER_QUEUE_MAX,
	//};

	class Renderable;
	class Material;

	class LORD_CORE_API ManualRenderListener
	{
	public:
		virtual void render(bool uiRender) = 0;
	};

	class LORD_CORE_API RenderQueue: public ObjectAlloc
	{
	public:
		RenderQueue(const String& strName);
		~RenderQueue();

		typedef vector<Renderable*>::type	RenderableList;

	public:
		void			beginRender();		
		void			addRenderable(Renderable* pRenderable);
		void			renderQueue(bool uiRender = false);

		void setManalRenderBegin(ManualRenderListener* mListener)
		{
			m_ManualBegin = mListener;
		}

		void setManalRenderEnd(ManualRenderListener* mListener)
		{
			m_ManualEnd = mListener;
		}
		
		void			enableRender()
		{
			m_bEnableRender = true;
		}

		void			disableRender()
		{
			m_bEnableRender = false;
		}

		const String&	getName() { return m_strName; }

	protected:
		bool				m_bEnableRender;
		ManualRenderListener* m_ManualBegin;
		ManualRenderListener* m_ManualEnd;
		String				m_strName;
		RenderableList		m_Renderables;		
	};

	// typedef RenderQueue** RenderQueueGroup;
	typedef vector<RenderQueue*>::type RenderQueueGroup;
}

#endif
