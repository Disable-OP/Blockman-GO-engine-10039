/********************************************************************
filename: 	GUIFlyAnimateController.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_FLY_ANIMATE_CONTROLLER_H__
#define __GUI_FLY_ANIMATE_CONTROLLER_H__

#include "Core.h"
#include "GUIUDim.h"

namespace LORD
{
	class GUIWindow;
	class LORD_CORE_API GUIFlyAnimateController: public ObjectAlloc
	{
	public:
		enum FlyAnmateType
		{
			//飞行动画
			FLY_AT_STRAIGHTLINE,	//直线
			FLY_AT_PARABOLA,		//抛物线
			FLY_AT_MISSILE,		//导弹
		};

		struct FlyAnimInfo
		{
			FlyAnmateType f_type;
			float f_Acceleration;
			int  f_AccDir;
			ui32 f_time;
			bool f_NeedChange;

			FlyAnimInfo()
			{
				f_Acceleration = 100;
				f_AccDir	   = 1;
				f_time		   = 500;
				f_type		   = FLY_AT_STRAIGHTLINE;
				f_NeedChange   = false;
			}
		};

		GUIFlyAnimateController(GUIWindow* pWindow);
		~GUIFlyAnimateController();

		void SetAnimateType(FlyAnmateType nType) { m_nAnimateType = nType; }
		FlyAnmateType GetAnimateType() { return m_nAnimateType; }
		void SetFlyTime(ui32 nTime) { m_nWholeTime = nTime; }
		ui32 GetFlyTime();

		void Update(ui32 nTimeElapse);

		void Start();
		void onChangeAnimateInfo();
		void DoAnimate();

		// 目标角度

		bool GetNeedRT() {return m_NeedRT;}
		void SetNeedRT(bool rt){ m_NeedRT = rt;}

		void	SetBeginArea();
		void    SetEndArea();

		void    SetBeginArea(const URect& beginArea){m_BeginArea = beginArea;}
		URect&  GetBeginArea(){return m_BeginArea;}

		void    SetEndArea(const URect& EndArea){m_EndArea = EndArea;}
		URect&  GetEndArea(){return m_EndArea;}

		FlyAnimInfo& GetFlyAnimatInfo(){return m_FlyAnimInfo;}

	protected:
		void	_makeParabola(const UVector2& _beginPos, const UVector2& _endPos, float _time, float _AccVal, int _AccDir);
		void	_makeStraightline(const UVector2& _beginPos, const UVector2& _endPos, float _time, float _AccVal, int _type);
		void    _makeMissile(const UVector2& _beginPos, const UVector2& _endPos, float _time, float _AccVal, int _AccDir);
		void    _ClearFlyParam();
		void    _updateParabola();
		void    _updateStraightline(const UVector2& _beginPos);

	private:

		FlyAnmateType	m_nAnimateType;
		GUIWindow*		m_pWindow;

		bool			m_bStarted;
		ui32			m_nElapseTime;
		ui32			m_nWholeTime;
		// 用开始的位置作为最终位置
		FlyAnimInfo     m_FlyAnimInfo;
		URect			m_BeginArea;
		URect			m_EndArea;
		UVector2        m_tempPos;
		float		    m_FlyParam[6];
		bool			m_NeedRT;
	};
}

#endif