#include "GUIFlyAnimateController.h"
#include "GUIWindow.h"
#include "GUIRenderManager.h"

namespace LORD
{


	GUIFlyAnimateController::GUIFlyAnimateController(GUIWindow* pWindow)
		: m_nAnimateType(FLY_AT_STRAIGHTLINE)
		, m_pWindow(pWindow)
		, m_bStarted(false)
		, m_nElapseTime(0)
		, m_nWholeTime(300)
		, m_NeedRT(true)
	{
		if (m_pWindow)
		{
//			m_BeginArea = m_pWindow->GetSavedArea();
			m_EndArea	= m_BeginArea;
		}
	}

	GUIFlyAnimateController::~GUIFlyAnimateController()
	{

	}

	void GUIFlyAnimateController::Update( ui32 nTimeElapse )
	{
		if (m_bStarted)
		{
			m_nElapseTime += nTimeElapse;

			if (m_nElapseTime > m_nWholeTime)
			{
				// ¶¯»­Í£Ö¹
				m_bStarted = false;
				m_nElapseTime = 0;

				if (m_pWindow)
				{
					m_pWindow->SetArea(m_EndArea);
				}
			}
			else
			{
				DoAnimate();
			}
		}
	}

	void GUIFlyAnimateController::Start()
	{
		m_bStarted = true;
		m_nElapseTime = 0;
		m_nWholeTime = 800;
		if (m_pWindow)
		{

		}
		onChangeAnimateInfo();
	}

	void GUIFlyAnimateController::DoAnimate()
	{
		if (!m_pWindow)
		{
			return;
		}

		// float fTimePercent = m_nElapseTime * 1.0f / m_nWholeTime;

		switch (m_FlyAnimInfo.f_type)
		{
		case FLY_AT_STRAIGHTLINE:
			{
				_updateStraightline(m_BeginArea.d_min);
			}
			break;
		case FLY_AT_PARABOLA:
			{
				_updateParabola();
			}
			break;
		case FLY_AT_MISSILE:
			{
				if(m_FlyAnimInfo.f_NeedChange && m_nElapseTime < 280)
				{
					_updateParabola();
				}
				else
				{
					if(m_FlyAnimInfo.f_NeedChange)
					{
						m_tempPos = m_pWindow->GetPosition();
						_makeStraightline(m_tempPos,m_EndArea.d_min,float(m_FlyAnimInfo.f_time),m_FlyAnimInfo.f_Acceleration,1);
						m_FlyAnimInfo.f_NeedChange = false;
						m_nWholeTime = m_FlyAnimInfo.f_time;
						m_nElapseTime = 0;
					}
					_updateStraightline(m_tempPos);
				}
			}
		}
	}

	void GUIFlyAnimateController::SetBeginArea()
	{
		m_BeginArea = m_pWindow->GetArea();
	}

	void GUIFlyAnimateController::SetEndArea()
	{
		m_EndArea = m_pWindow->GetArea();
	}

	void GUIFlyAnimateController::onChangeAnimateInfo()
	{
		if(m_FlyAnimInfo.f_type == FLY_AT_STRAIGHTLINE)
		{
			m_nWholeTime = m_FlyAnimInfo.f_time;
			_makeStraightline(m_BeginArea.d_min, m_EndArea.d_min, float(m_FlyAnimInfo.f_time) , m_FlyAnimInfo.f_Acceleration, m_FlyAnimInfo.f_AccDir);
			m_pWindow->SetPosition(m_BeginArea.d_min);
		}
		else if(m_FlyAnimInfo.f_type == FLY_AT_PARABOLA)
		{
			m_nWholeTime = m_FlyAnimInfo.f_time;
			_makeParabola(m_BeginArea.d_min, m_EndArea.d_min, float(m_FlyAnimInfo.f_time), m_FlyAnimInfo.f_Acceleration, m_FlyAnimInfo.f_AccDir);
			m_pWindow->SetPosition(m_BeginArea.d_min);
		}
		else if(m_FlyAnimInfo.f_type == FLY_AT_MISSILE)
		{
			m_nWholeTime = m_FlyAnimInfo.f_time + 280;
			_makeMissile(m_BeginArea.d_min, m_EndArea.d_min, float(m_FlyAnimInfo.f_time), m_FlyAnimInfo.f_Acceleration, m_FlyAnimInfo.f_AccDir);
			m_pWindow->SetPosition(m_BeginArea.d_min);
		}
	}

	void GUIFlyAnimateController::_ClearFlyParam()
	{
		for(int i = 0 ; i < 6; ++i)
		{
			m_FlyParam[i] = 0.0f;
		}
	}

	void GUIFlyAnimateController::_makeParabola( const UVector2& _beginPos, const UVector2& _endPos, float _time, float _AccVal, int _AccDir )
	{
		_ClearFlyParam();
		_time = _time * 0.01f;
		float offsetX = _endPos.d_x.d_offset - _beginPos.d_x.d_offset;
		float offsetY = _endPos.d_y.d_offset - _beginPos.d_y.d_offset;
		float v0 = _AccVal * _time * 0.5f;
		m_FlyParam[0] = offsetX / _time;
		m_FlyParam[1] = offsetY / _time;

		if(offsetY == 0.0f)
		{
			m_FlyParam[4] = -v0;
			m_FlyParam[5] = _AccVal;
		}
		else if(offsetX == 0.0f)
		{
			m_FlyParam[2] = v0;
			m_FlyParam[3] = -_AccVal;
		}
		else
		{
			float AccDirX = 1;
			float AccDirY = 1;

			if(offsetY > 0.0f)
			{
				AccDirX = -1;
			}

			if(offsetX < 0.0f)
			{
				AccDirY = -1;
			}

			float dx = Math::Sqr(offsetX / offsetY) + 1.0f; 
			float dy = Math::Sqr(offsetY / offsetX) + 1.0f;

			m_FlyParam[3] = AccDirX * Math::Sqrt(_AccVal * _AccVal / dx);
			m_FlyParam[5] =	AccDirY * Math::Sqrt(_AccVal * _AccVal / dy);

			m_FlyParam[2] = -AccDirX * Math::Sqrt(v0 * v0 / dx);
			m_FlyParam[4] =	-AccDirY * Math::Sqrt(v0 * v0 / dy);
		}

		bool isRight = false;
		if(_AccDir == 2)
		{
			isRight = true;
		}
		else if(_AccDir == 3 )
		{
			float rate = Math::IntervalRandom(0.0f,2.0f);
			if(rate > 1.0f)
				isRight = true;
		}

		if(isRight)
		{
			m_FlyParam[2] = -m_FlyParam[2];
			m_FlyParam[3] = -m_FlyParam[3];
			m_FlyParam[4] = -m_FlyParam[4];
			m_FlyParam[5] = -m_FlyParam[5];
		}
	}

	void GUIFlyAnimateController::_makeStraightline( const UVector2& _beginPos, const UVector2& _endPos, float _time, float _AccVal, int _type )
	{
		_ClearFlyParam();
		_time = _time * 0.02f;
		float offsetX = _endPos.d_x.d_offset - _beginPos.d_x.d_offset;
		float offsetY = _endPos.d_y.d_offset - _beginPos.d_y.d_offset;
		if(_type == 1)
		{
			m_FlyParam[0] = offsetX / _time;
			m_FlyParam[1] = offsetY / _time;
			return;
		}

		float AccDirX = 1;
		float AccDirY = 1;

		if(offsetX < 0.0f)
		{
			AccDirX = -1;
		}

		if(offsetY < 0.0f)
		{
			AccDirY = -1;
		}

		if(offsetX != 0.0f && offsetY != 0.0f)
		{
			float dx = Math::Sqr(offsetY / offsetX) + 1.0f; 
			float dy = Math::Sqr(offsetX / offsetY) + 1.0f;
			m_FlyParam[3] = AccDirX * Math::Sqrt(_AccVal * _AccVal / dx);
			m_FlyParam[5] =	AccDirY * Math::Sqrt(_AccVal * _AccVal / dy);
		}

		if(_type == 3)
		{
			m_FlyParam[3] = -m_FlyParam[3];
			m_FlyParam[5] = -m_FlyParam[5];
		}

		if(offsetX == 0.0f)
		{
			m_FlyParam[4] = offsetY / _time - AccDirY * _AccVal * _time * 0.5f;
			m_FlyParam[5] = AccDirY * _AccVal;
		}
		else if(offsetY == 0.0f)
		{
			m_FlyParam[2] = offsetX / _time - AccDirX * _AccVal * _time * 0.5f;
			m_FlyParam[3] = AccDirX * _AccVal;
		}
		else
		{ 
			m_FlyParam[2] = offsetX / _time - m_FlyParam[3] * _time * 0.5f;
			m_FlyParam[4] = offsetY / _time - m_FlyParam[5] * _time * 0.5f;
		}
	}

	void GUIFlyAnimateController::_updateParabola()
	{
		UVector2 distanceOffset;
		float _time = m_nElapseTime * 0.01f;
		distanceOffset.d_x.d_offset = (m_FlyParam[0] + m_FlyParam[2]) * _time;
		distanceOffset.d_x.d_offset += m_FlyParam[3] * _time * _time * 0.5f;
		distanceOffset.d_y.d_offset = (m_FlyParam[1] + m_FlyParam[4]) * _time;
		distanceOffset.d_y.d_offset += m_FlyParam[5] * _time * _time * 0.5f;
		distanceOffset.d_x.d_scale = 0.0f;
		distanceOffset.d_y.d_scale = 0.0f;
		m_pWindow->SetPosition(m_BeginArea.d_min + distanceOffset);
	}

	void GUIFlyAnimateController::_updateStraightline(const UVector2& _beginPos)
	{
		UVector2 distanceOffset;
		float _time = m_nElapseTime * 0.02f;
		distanceOffset.d_x.d_offset = m_FlyParam[0] * _time;
		distanceOffset.d_y.d_offset = m_FlyParam[1] * _time;
		distanceOffset.d_x.d_offset += m_FlyParam[2] * _time + m_FlyParam[3] * _time * _time * 0.5f;
		distanceOffset.d_y.d_offset += m_FlyParam[4] * _time + m_FlyParam[5] * _time * _time * 0.5f;
		distanceOffset.d_x.d_scale = 0.0f;
		distanceOffset.d_y.d_scale = 0.0f;
		m_pWindow->SetPosition(_beginPos + distanceOffset);
	}

	void GUIFlyAnimateController::_makeMissile( const UVector2& _beginPos, const UVector2& _endPos, float _time, float _AccVal, int _AccDir )
	{
		float offsetX = _endPos.d_x.d_offset - _beginPos.d_x.d_offset;
		float offsetY = _endPos.d_y.d_offset - _beginPos.d_y.d_offset;
		float dirX = 1;
		float dirY = 1;

		if(offsetX < 0.0f)
		{
			dirX = -1;
			offsetX = -offsetX;
		}
		else if(offsetX == 0.0f)
		{
			dirX = 0.0f;
		}

		if(offsetY < 0.0f)
		{
			dirY = -1;
			offsetY = -offsetY;
		}
		else if(offsetY == 0.0f)
		{
			dirY = 0.0f;
		}



		bool isRight = false;
		if(_AccDir == 2)
		{
			isRight = true;
		}
		else if(_AccDir == 3 )
		{
			float rate = Math::IntervalRandom(0.0f,2.0f);
			if(rate > 1.0f)
				isRight = true;
		}

		UVector2 _tempPoint;
		if(!isRight)
		{
			_tempPoint.d_x.d_offset = _beginPos.d_x.d_offset +  dirY * offsetY / (offsetY + offsetX)*150.f;
			_tempPoint.d_y.d_offset = _beginPos.d_y.d_offset -  dirX * offsetX / (offsetY + offsetX)*150.f;
			_makeParabola(_beginPos,_tempPoint,280.f,80.f,1);
		}
		else
		{
			_tempPoint.d_x.d_offset = _beginPos.d_x.d_offset -  dirY * offsetY / (offsetY + offsetX)*150.f;
			_tempPoint.d_y.d_offset = _beginPos.d_y.d_offset +  dirX * offsetX / (offsetY + offsetX)*150.f;
			_makeParabola(_beginPos,_tempPoint,280.f,80.f,2);
		}

		m_FlyAnimInfo.f_NeedChange = true;
	}
}