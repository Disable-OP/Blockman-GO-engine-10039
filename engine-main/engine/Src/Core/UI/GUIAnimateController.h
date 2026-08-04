/********************************************************************
filename: 	GUIAnimateController.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_ANIMATE_CONTROLLER_H__
#define __GUI_ANIMATE_CONTROLLER_H__

#include "Core.h"
#include "GUIUDim.h"

namespace LORD
{
	class GUIWindow;
	class LORD_CORE_API GUIAnimateController: public ObjectAlloc
	{
	public:
		enum AnimateType
		{
			AT_NONE,
			AT_FLY_FROM_LEFT,
			AT_FLY_FROM_TOP,
			AT_FLY_FROM_RIGHT,
			AT_FLY_FROM_BOTTOM,
			AT_POPUP,
			AT_ROTATE,
		};

		GUIAnimateController(GUIWindow* pWindow);
		~GUIAnimateController();

		void SetAnimateType(AnimateType nType) { m_nAnimateType = nType; }
		AnimateType GetAnimateType() { return m_nAnimateType; }
		void SetWholeTime(ui32 nTime) { m_nWholeTime = nTime; }
		ui32 GetWholeTime();

		void Update(ui32 nTimeElapse);

		void Start();

		void DoAnimate();

		bool IsStarted() { return m_bStarted; }

		// 目标角度
		void SetTargetRotateY(float fTRotateY) { m_fTargetRotateY = fTRotateY; }
		void SetWhloeTime(ui32 nTime) { m_nWholeTime = nTime; }
		void SetRotateAngle(float fAngle) { m_fRotateAngle = fAngle; }

		bool GetNeedRT() {return m_NeedRT;}
		void SetNeedRT(bool rt){ m_NeedRT = rt;}
	private:

		AnimateType		m_nAnimateType;
		GUIWindow*		m_pWindow;
		
		bool			m_bStarted;
		ui32			m_nElapseTime;
		ui32			m_nWholeTime;
		// 用开始的位置作为最终位置
		URect			m_TargetArea;
		float			m_fTargetRotateY;
		float			m_fRotateAngle;
		float			m_AnimParam[3];
		bool			m_NeedRT;
	};
}

#endif