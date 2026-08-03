/********************************************************************
filename: 	GUIRenderTargetManager.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_RENDER_TARGET_MANAGER_H__
#define __GUI_RENDER_TARGET_MANAGER_H__

#include "Core.h"
#include "Object/Singleton.h"

namespace LORD
{
	class LORD_CORE_API GUIRenderTargetManager : public ObjectAlloc, public Singleton<GUIRenderTargetManager>
	{
	public:

		struct GUIRenderTarget 
		{
			// 是否在使用
			bool			m_bFlag;
			int				m_nOldID;
			uint			m_nOldWidth;
			uint			m_nOldHeight;
		};

		typedef vector<GUIRenderTarget>::type		RenderTargetFlagList;

		GUIRenderTargetManager();
		~GUIRenderTargetManager();

		static GUIRenderTargetManager* Instance();

		int		CreateRenderTarget(float fWidth, float fHeight);
		void	DestroyRenderTarget(int nID);

		// 开始切到一个新的rt
		void	BeginRenderTarget(int nID, uint nWidth, uint nHeight);
		// 结束的时候恢复到切之前的那个
		void	EndRenderTarget();

	private:
		int		GetUnusedID();

	private:
		RenderTargetFlagList		m_RenderTargetFlagList;
	};
}


#endif