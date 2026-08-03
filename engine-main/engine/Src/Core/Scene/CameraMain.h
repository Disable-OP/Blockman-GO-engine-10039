/********************************************************************
filename: 	CameraMain.h
file path:	dev\engine\Src\Core\Scene\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_CAMERAMAIN_H__
#define __LORD_CAMERAMAIN_H__

#include "Camera.h"

namespace LORD
{
	// 相机振动结构定义
	struct Camera_Shake_Info
	{
		//震动次数
		int shakeTimes;
		// 开始时间
		float beginTime;
		// 振动幅度
		float scale;
		// 剩余振动时间
		float timelast;
		// 总振动时间
		float duration;
		//
		float shakeSpeed;

		Camera_Shake_Info()
		{
			beginTime = 0.f;
			scale = 0.f;
			timelast = 0.f;
			duration = -1.0f;
			shakeSpeed = 0.f;
			shakeTimes = 0;
		}
	};

	class LORD_CORE_API CameraMain: public Camera
	{
	public:
		enum CAMERA_STATE
		{
			NORMAL,
			SHAKE,
			ANIMA,
			LOCK,
			STATEMAX
		};
	public:	
		CameraMain(ProjMode mode = PM_PERSPECTIVE, bool isFixedYaw = true );
		virtual ~CameraMain();

	public:
		virtual void			setPosition(const Vector3& pos);
		virtual void			setTarget(const Vector3& target);
		virtual void			setDirection(const Vector3& dir);
		virtual void			setUp(const Vector3& vUp);

		inline void setPosition_forLord( const Vector3& pos )
		{
			m_position = pos;
			m_bNeedUpdateView = true;
		}

		inline void setTarget_forLord( const Vector3& target )
		{
			m_dir = target - m_position;
			m_dir.normalize();

			m_bNeedUpdateView = true;
		}

		inline void setDirection_forLord( const Vector3& dir )
		{
			m_dir = dir;
			m_bNeedUpdateView = true;
		}

		inline void setUp_forLord( const Vector3& vUp )
		{
			m_up = vUp;
			m_bNeedUpdateView = true;
		}

		virtual void			yaw(Real rad);
		virtual void			pitch(Real rad);
		virtual void			roll(Real rad);
		virtual void			rotate(const Vector3& vAxis, Real rad);
		virtual void			move(const Vector3& offset);

		inline void				setCameraState(ui32 state)
		{
			m_CameraState = state;
		}

		inline ui32				getCameraState() const { return (ui32)m_CameraState; }

		// 添加摄像机抖动
		void	AddCameraShake(float beginTime, float scale, float durationTime, int shakeTimes);
		void	ProcessMainCameraShake(float delta);
		void    StopCameraShake();
		void	EnableCameraShake(bool bEnable) { m_bEnableShake = bEnable; }
	protected:
		bool		m_bEnableShake;
		ui32		m_CameraState;
		//摄像机震动
		int			mCameraShakeStep1;
		int			mCameraShakeStep2;
		float		mCameraShakeDelta;
		Camera_Shake_Info mCameraShake;
	};

}

#endif