#include "CameraMain.h"

namespace LORD
{
	CameraMain::CameraMain(ProjMode mode, bool isFixedYaw)
		: Camera(mode, isFixedYaw)
		, m_bEnableShake(true)
		, m_CameraState(NORMAL)
		, mCameraShakeStep1(0)
		, mCameraShakeStep2(0)
		, mCameraShakeDelta(0.0f)
	{
	}

	CameraMain::~CameraMain()
	{
	}

	void CameraMain::setPosition(const Vector3& pos)
	{
		if (m_CameraState  == NORMAL)
		{
			m_position = pos;
			m_bNeedUpdateView = true;
		}
	}
	
	void CameraMain::setTarget(const Vector3& target)
	{
		if (m_CameraState  == NORMAL)
		{
			m_dir = target - m_position;
			m_dir.normalize();

			m_bNeedUpdateView = true;
		}
	}

	void CameraMain::setDirection(const Vector3& dir)
	{
		if (m_CameraState  == NORMAL)
		{
			m_dir = dir;
			m_bNeedUpdateView = true;
		}
	}

	void CameraMain::setUp(const Vector3& vUp)
	{
		if (m_CameraState  == NORMAL)
		{
			m_up = vUp;
			m_bNeedUpdateView = true;
		}
	}
	
	void CameraMain::yaw(Real rad)
	{
		if (m_CameraState  == NORMAL)
		{
			Matrix4 matRot;
			matRot.rotateAxisReplace(m_fixedYawAxis, rad);

			m_dir = matRot.transform(m_dir);
			m_up = matRot.transform(m_up);
			m_bNeedUpdateView = true;
		}
	}

	void CameraMain::pitch(Real rad)
	{
		if (m_CameraState  == NORMAL)
		{
			Matrix4 matRot;
			matRot.rotateAxisReplace(m_right, rad);

			m_dir = matRot.transform(m_dir);
			m_up = matRot.transform(m_up);
			m_bNeedUpdateView = true;
		}
	}

	void CameraMain::roll(Real rad)
	{
		if (m_CameraState  == NORMAL)
		{
			Matrix4 matRot;
			matRot.rotateAxisReplace(m_dir, rad);

			m_dir = matRot.transform(m_dir);
			m_up = matRot.transform(m_up);
			m_bNeedUpdateView = true;
		}
	}

	void CameraMain::rotate(const Vector3& vAxis, Real rad)
	{
		if (m_CameraState  == NORMAL)
		{
			Matrix4 matRot;
			matRot.rotateAxisReplace(vAxis, rad);

			m_dir = matRot.transform(m_dir);
			m_up = matRot.transform(m_up);
			m_bNeedUpdateView = true;
		}
	}

	void CameraMain::move(const Vector3& offset)
	{
		if (m_CameraState  == NORMAL)
		{
			m_position += offset;
			m_bNeedUpdateView = true;
		}
	}

	void CameraMain::AddCameraShake(float beginTime, float scale, float durationTime, int shakeTimes)
	{
		if (!m_bEnableShake || m_CameraState != NORMAL)
			return;

		m_CameraState = CameraMain::SHAKE;
		mCameraShake.beginTime = beginTime;
		mCameraShake.scale = scale;
		mCameraShake.duration = durationTime;
		mCameraShake.timelast = durationTime;
		mCameraShake.shakeSpeed = scale / durationTime / (SceneManager::Instance()->getMainCamera()->getFov() * 10);
		mCameraShake.shakeTimes = shakeTimes;
	}

	void CameraMain::ProcessMainCameraShake(float delta)
	{
		if (mCameraShake.duration < 0.f)
			return;

		mCameraShake.beginTime -= delta;
		if (mCameraShake.beginTime > 0.0f)
		{
			return;
		}

		mCameraShakeDelta += delta;
		if (mCameraShakeDelta < 0.033f)
		{
			return;
		}
		else
		{
			mCameraShakeDelta -= 0.033f;
		}

		// ³¬Ê±Çå³ý
		if (mCameraShake.timelast < 0 && mCameraShakeStep1 == 0 && mCameraShakeStep2 == 0)
		{
			mCameraShake.timelast = mCameraShake.duration;
			mCameraShake.shakeTimes -= 1;
			if (mCameraShake.shakeTimes <= 0)
			{
				StopCameraShake();
			}
		}
		else
		{
			// ¼ÆËãÆ«ÒÆ
			Vector3 deltapos;
			Vector3 oldcamerapos = m_position;
			if (mCameraShake.timelast > mCameraShake.duration * 0.6f)
			{
				deltapos = m_dir* mCameraShake.shakeSpeed * 0.3f;
				setPosition_forLord(oldcamerapos + deltapos);
				++mCameraShakeStep1;
			}
			else if (mCameraShake.timelast > mCameraShake.duration * 0.4f)
			{
				deltapos = m_dir* mCameraShake.shakeSpeed * 0.4f;
				setPosition_forLord(oldcamerapos - deltapos);
				++mCameraShakeStep2;
			}
			else if (mCameraShakeStep2)
			{
				deltapos = m_dir* mCameraShake.shakeSpeed * 0.4f;
				setPosition_forLord(oldcamerapos + deltapos);
				--mCameraShakeStep2;
			}
			else if (mCameraShakeStep1)
			{
				if (mCameraShakeStep1 > 1)
				{
					deltapos = m_dir* mCameraShake.shakeSpeed * 0.6f;
					setPosition_forLord(oldcamerapos - deltapos);
					mCameraShakeStep1 -= 2;
					if (!mCameraShakeStep1)
					{
						mCameraShake.timelast = 0.f;
					}

				}
				else if (mCameraShakeStep1 == 1)
				{
					deltapos = m_dir* mCameraShake.shakeSpeed*0.3f;
					setPosition_forLord(oldcamerapos - deltapos);
					--mCameraShakeStep1;
					mCameraShake.timelast = 0.f;
				}
			}
		}
		mCameraShake.timelast -= 0.033f;
	}

	void CameraMain::StopCameraShake()
	{
		m_CameraState = NORMAL;
		mCameraShake.duration = -1.0f;
		mCameraShakeDelta = 0.f;
		mCameraShakeStep1 = 0;
		mCameraShakeStep2 = 0;
	}

}
