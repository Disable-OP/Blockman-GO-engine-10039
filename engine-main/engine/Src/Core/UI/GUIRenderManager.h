/********************************************************************
filename: 	GUIRenderManager.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_RENDER_MANAGER_H__
#define __GUI_RENDER_MANAGER_H__

#include "Core.h"
#include "Object/Singleton.h"
#include "GUICommonDef.h"

namespace LORD
{
	class GUICoolDownRender;
	class LORD_CORE_API GUIRenderManager : public ObjectAlloc, public Singleton<GUIRenderManager>
	{
	public:
		struct RenderEvent
		{
			GUIMaterialType matType;
			Texture* pTexture[2];
			Word*	pIndices;
			ui32    indicesNum;
			ERect scissorRect;
			bool hasScissor;
		};
	public:
		GUIRenderManager(uint nWidth, uint nHeight);
		~GUIRenderManager();

		static GUIRenderManager* Instance();

		void	Initialize();
		void	Destroy();

		void	OnSized(uint nWidth, uint nHeight);

		void	CreateRenderable();
		void	DestroyRenderable();

		void	AddQuadToBuffer(const Vector3& lt, const Vector3& rb, const Vector3& lb, const Vector3& rt, const ERect& rtTexAddress, const ColorRect& color,
							Texture* pTexture1, Texture* pTexture2, GUIMaterialType material);

		void	DrawBuffer();

		// 设置UI需要的渲染状态
		void	BeginRender();

		// 恢复之前的渲染状态
		void	EndRender();

		void	RenderEvents();

		// set ui view port
		void	BeginGUIViewport();

		void	EndGUIViewport();

		void	BeginScissorRect(const ERect& rtClip);
		void	EndScissorRect();

		// 这个接口用来在外面获取samplerstate， 用来激活texture
		Material*		GetMaterial(GUIMaterialType material = GUIMaterialType_CULL_BACK) { return m_materialMap[material]; }

		int		GetWidth() { return m_nDisplayWidth; }
		int		GetHeight() { return m_nDisplayHeight; }

		Vector2 GetDisplaySize();

		bool	IsInScissor() { return m_bIsInScissor; }

		const ERect&	GetCurrentScissor() { return m_rtCurrentClipRect; }
		const Texture* GetCurrentTexture1() { return m_currentTexture1; }
		const Texture* GetCurrentTexture2() { return m_currentTexture2; }

		void	SetDepth(float depth) { m_depth = depth; }
		float	GetDepth() { return m_depth;  }

		void	clearQuadNumber() { m_quadNumber = 0; }

		unsigned long getQuadNumber() { return m_quadNumber; }

	protected:
		void	_handleMaterial();
		void	_handleMaterial(GUIMaterialType _type);
		void    _handleProgram(GUIMaterialType _type);

		void	_reset();

	private:
		void		UpdateProjMatrix();

		int						m_nDisplayWidth;
		int						m_nDisplayHeight;
		ERect					m_rtCurrentClipRect;
		bool					m_bIsInScissor;

		Camera*					m_pGUI2DCamera;
		Camera*					m_pActorCamera;
		Matrix4					m_matViewProject;

		// render input
		GUIVertexDeclaration*	m_pVertexSystemMemory;
		Word*					m_pIndexSystemMemory;
		int						m_nVertexCount;
		int						m_nIndexCount;
		int						m_nQuadCount;
		GPUBuffer*				m_pVertexBuffer;
		GPUBuffer*				m_pIndexBuffer;
		Texture*				m_pActiveTexture;
		RenderInput*			m_pRenderInput;

		GUIVertexDeclaration*	m_pBufferOffset;

		float					m_depth;

		ShaderProgram*			m_CurrentProgram;
		Texture*				m_CurrentTexture;

		bool					m_is3DCamera;

		// 材质管理器
		map<GUIMaterialType, Material*>::type	m_materialMap;
		GUIMaterialType			m_currentMaterial;
		Texture*				m_currentTexture1;
		Texture*				m_currentTexture2;

		// 渲染队列
		vector<RenderEvent>::type		m_RenderEvent;

		// ui quad number
		unsigned long			m_quadNumber;
	};
}


#endif
