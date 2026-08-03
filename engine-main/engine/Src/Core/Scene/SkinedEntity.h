#ifndef __LORD_SKINEDENTITY_H__
#define __LORD_SKINEDENTITY_H__

#include "Core.h"
#include "ModelEntity.h"
#include "Resource/LordThread.h"

namespace LORD
{
	/************************************************************************/
	/* this class is only used to render.
	   you need create a skeleton and a animBlender and a SkinnedEntity
	/************************************************************************/

	class SkinedEvent;

	class LORD_CORE_API SkinedEntity: public ModelEntity
	{
		friend class SkinedEvent;

		struct VF_SKIN_SCENE
		{
			Vector3 position;
			Dword	color;
			Vector2 texcoord;
		};

		struct VF_SKIN_ACTOR_CPU
		{
			Vector3 position;
			Vector3 normal;
			Vector2 texcoord;
		};

	public:
		SkinedEntity();
		virtual ~SkinedEntity();

	public:
		virtual void	setMesh(Mesh* pMesh);

		void			setSkeleton(Skeleton* pSkeleton);
		Skeleton*		getSkeleton() const{return m_pSkeleton;}
		
		void			setAnimBlender(AnimBlender* pAnimation);
	
		virtual void	createRenderable(bool isUIActor = false);
		virtual void    update(const Matrix4* viewproj=NULL);
		virtual void	updateForCalcThread(bool isUIActor = false);
		virtual void	excludeCalcThread();

		virtual void updateRenderQueue(RenderQueue* pRenderqueue);

		Box getLocalAABB() const;

		// 设置是否使用XRay
		virtual void    setUseXRay( bool isUseXRay, const Vector4* color);

		virtual void	setEffectTexture(Texture* pTexture, int effect);
		virtual void	revertEffectTexture();
		
		void			changeDefaultTexture(Texture* pTexture);
		void			revertDefaultTexture();

		virtual void replaceTexture(Texture* pTexture, size_t subid, ui32 uint);
	protected:
		void			generateVBForScene();
		void			generateVBForActor();

	private:
		// 更新子模型用骨骼动画数据
		void			refreshSubMeshBoneMatRows();

	protected:
		static int		s_maxBoneCount;
		//static Color	s_edgeColor;
		Skeleton*		m_pSkeleton;
		AnimBlender*	m_pAnimBlender;

		uint					m_boneCount;
		Vector4*				m_boneMatRows;
		vector<Vector4*>::type	m_subMeshBoneMatRows;

		bool			m_bSuppertGPU;

		// used for cpu skinned mesh.
		vector<Byte*>::type		 m_vertexCPUs;
		vector<GPUBuffer*>::type m_vertexBuffers;
		SkinedEvent*			 m_pSkinedEvent;

		Texture*		m_pDefaultTexture;
	};

	class SkinedEvent : public ThreadEvent
	{
	public:
		SkinedEvent(SkinedEntity* pSkinedEntity);
		~SkinedEvent();
		
		virtual bool ProcessEvent();

		virtual bool RespondEvent();

		virtual void SetInCalcThread(bool inThread);

		virtual bool GetInCalcThread() const;

		virtual void RestoreEvent();
	protected:
		SkinedEntity* m_pSkinedEntity;
		bool		  m_InCalcThread;
	};
}

#endif
