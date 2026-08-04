/********************************************************************
filename: 	GUICoolDownRender.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/
//
// #ifndef __GUI_COOL_DOWN_RENDER_H__
// #define __GUI_COOL_DOWN_RENDER_H__
// 
// #include "Core.h"
// 
// namespace LORD
// {
// 	struct GUIVertexDeclaration;
// 	class LORD_CORE_API GUICoolDownRender: public ObjectAlloc
// 	{
// 	public:
// 
// 		GUICoolDownRender();
// 		~GUICoolDownRender();
// 
// 		void	CreateRenderable();
// 		void	DestroyRenderable();
// 
// 		// 百分比表示cooldown当前的cd状态
// 		void	AddCoolDown(const Vector2& centerPoint, float fRadius, float fPercent);
// 
// 		void	DrawBuffer();
// 
// 	private:
// 		void	AddPoint(const Vector2& position, const Vector2& uv, unsigned int nColor = 0xFFFFFFFF);
// 		
// 	private:
// 
// 		// render data
// 		GUIVertexDeclaration*	m_pVertexSystemMemory;
// 		Word*					m_pIndexSystemMemory;
// 		int						m_nVertexCount;
// 		int						m_nIndexCount;
// 		int						m_nTriangleCount;
// 		GPUBuffer*				m_pVertexBuffer;
// 		GPUBuffer*				m_pIndexBuffer;
// 		Texture*				m_pTexture;
// 		RenderInput*			m_pRenderInput;
// 		GUIVertexDeclaration*	m_pBufferOffset;
// 	};
// }
// 
// #endif