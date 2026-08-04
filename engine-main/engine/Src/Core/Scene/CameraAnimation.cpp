#include "CameraAnimation.h"
#include "Object/Root.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Util/QueryObject.h"
#include "Geom/SimpleSpline.h"
#include "Actor/ActorManager.h"
#include "Actor/ActorObject.h"
#include "SceneDefine.h"

namespace LORD
{
	const float editor_r = 0.1f;

	String autoQueryName()
	{
		static int count = 0;
		return StringUtil::ToString(count++);
	}

	CameraAnimation::CameraAnimation(const String& name)
		: mIsFocus(false)
		, mIsStopFade(false)
		, mIsStay(false)
		, mIsChangeDark(true)
		, mFadeState(0)
		, m_State(STOP)
		, mPointNum(0)
		, mAccIndex(0)
		, mCurrentTime(0.f)
		, mStayCurrentTime(0.f)
		, mStayTime(0.f)
		, m_Pitch(0.f)
		, m_Yaw(0.f)
		, m_Roll(0.f)
		, m_DarkTime(0.5f)
		, m_Darkness(0.6f)
		, m_LightTime(0.5f)
		, m_Lightness(0.6f)
		, m_CurDarkTime(0.f)
		, m_CurLightTime(0.f)
		, m_PassTime(0.f)
		, m_TotalTime(0.f)
		, m_PlayFadeTime(1.0f)
		, m_StopFadeTime(1.0f)
		, m_FadeSpeed(0.f)
		, m_OldSpeedUp(1.0f)
		, m_FadeDirection(Vector3::ZERO)
		, m_LastPosition(Vector3::ZERO)
		, m_LastDirection(Vector3::NEG_UNIT_Z)
		, m_FadePosition(Vector3::ZERO)
		, m_FadeOriternion(Quaternion::IDENTITY)
		, m_LastOriternion(Quaternion::IDENTITY)
		, m_ActorOriternion(Quaternion::IDENTITY)
		, m_pActor(NULL)
		, m_pPathRender(NULL)
		, m_pSpline(NULL)
		, m_Name(name)
		, m_ActorArray(StringUtil::BLANK)
	{
		m_pCamera = (CameraMain*)SceneManager::Instance()->getMainCamera();
	}

	CameraAnimation::~CameraAnimation(void)
	{
		_unprepare();
	}

	/*bool CameraAnimation::_prepare_io()
	{
		return true;
	}*/

	bool CameraAnimation::_prepare_obj()
	{
		m_pSpline = LordNew SimpleSpline;
		return true;
	}

	bool CameraAnimation::_prepare_editor()
	{
		if (Root::Instance()->isEditorMode())
		{
			m_pPathRender = LordNew CameraPathRender(m_Name);
		}
		return true;
	}

	void CameraAnimation::_unprepare()
	{
		if (m_State == PLAY)
		{
			stop();
		}
		if (Root::Instance()->isEditorMode())
		{
			QueryObjectManager* pQueryManager = QueryObjectManager::Instance();
			list<QueryObject*>::type::iterator it = m_PointObject.begin();
			for (; it!=m_PointObject.end(); ++it)
			{
				SceneNode* pNode = (*it)->getParent();
				pNode->getParent()->destroyChild(pNode);
				pQueryManager->destroyQueryObject(*it);
			}
			m_PointObject.clear();
		}
		LordSafeDelete(m_pPathRender);
		LordSafeDelete(m_pSpline);
	}

	void CameraAnimation::update( ui32 frameTime )
	{
		if (m_State == PLAY)
		{
			if (mPointNum < 2)
				return;
			
			float delTime = (float)frameTime * 0.001f;
			m_PassTime += delTime;

			updateFadeLight(delTime);

			if (m_PassTime < m_PlayFadeTime)
			{
				float passtime = Math::Min(m_PassTime, m_PlayFadeTime);
				m_pCamera->setPosition_forLord(m_LastPosition + m_FadeDirection*m_FadeSpeed*passtime);
				if (mIsFocus && m_pActor)
				{
					m_pCamera->setTarget_forLord(m_pActor->GetPosition());
				}
				else
				{
					Quaternion q, transQ = mCurrentIt->qua;
					Quaternion::Lerp(q, m_LastOriternion, transQ, passtime / m_PlayFadeTime, true);
					m_pCamera->setDirection_forLord(q*Vector3::NEG_UNIT_Z);
				}
			}
			else if (mIsStopFade)
			{
				float escapeTime = Math::Max(m_TotalTime - m_PassTime, 0.f);
				if (escapeTime > 0.f)
				{
					escapeTime = m_StopFadeTime - escapeTime;
					m_pCamera->setPosition_forLord(m_FadePosition + m_FadeDirection*m_FadeSpeed*escapeTime);
					if (mIsFocus && m_pActor)
					{
						m_pCamera->setTarget_forLord(m_pActor->GetPosition());
					}
					else
					{
						Quaternion q;
						Quaternion::Lerp(q, m_FadeOriternion, m_LastOriternion, escapeTime / m_StopFadeTime, true);
						m_pCamera->setDirection_forLord(q*Vector3::NEG_UNIT_Z);
					}
				}
				else
				{
					stop();
				}
			}
			else
			{
				updatePath(delTime);
			} // fade
		}//if (m_State == PLAY)
	}

	void CameraAnimation::updatePath(float frameTime)
	{
		list<PointInfo>::type::iterator nextIt = mCurrentIt;
		if (++nextIt == mMainPointList.end())
		{
			if (mStayCurrentTime >= mStayTime)
			{
				if (m_StopFadeTime > 0.f)
				{
					mIsStopFade = true;
					m_FadePosition = m_pCamera->getPosition();
					m_FadeOriternion = m_pCamera->getOritation();
					m_FadeDirection = m_LastPosition - m_FadePosition;
					float len = m_FadeDirection.normalizeLen();
					m_FadeSpeed = len / m_StopFadeTime;
					if (m_pActor)
						m_pActor->setOnlyCameraAnim(true);
				}
				else
				{
					stop();
				}
			}
			else
			{
				mStayCurrentTime += frameTime;
			}
		}
		else
		{
			if (m_pActor)
				m_pActor->setOnlyCameraAnim(mCurrentIt->pause);
			if (mStayCurrentTime >= mStayTime)
			{
				mCurrentTime += frameTime;
				float totalTime = nextIt->time - mCurrentIt->time;
				float t = mCurrentTime / totalTime;
				Vector3 pos = m_pSpline->interpolate(mAccIndex, t);
				m_pCamera->setPosition_forLord(pos);

				if (mIsFocus && m_pActor)
				{
					m_pCamera->setTarget_forLord(m_pActor->GetPosition());
				}
				else
				{
					Quaternion q, transQ = mCurrentIt->qua, destQ = nextIt->qua;
					if (m_pActor)
					{
						q = m_ActorOriternion;
						transQ = q * transQ;
						destQ = q * destQ;
					}
					Quaternion::Lerp(q, transQ, destQ, t, true);
					m_pCamera->setDirection_forLord(q*Vector3::NEG_UNIT_Z);
				}

				if (t >= 1.0f)
				{
					++mCurrentIt;
					++mAccIndex;
					//if (++mAccIndex == mPointNum - 2) mFadeState = 2;  forget this code what work for!
					mCurrentTime -= totalTime;
					mStayCurrentTime = 0;
					mStayTime = mCurrentIt->staytime;
					if (m_pActor)
					{
						ActorManager::Instance()->SetSpeedUp(mCurrentIt->timescale*m_OldSpeedUp);
					}
				}
			}
			else
			{
				mStayCurrentTime += frameTime;
			}
		}// list
	}

	void CameraAnimation::updateFadeLight(float frameTime)
	{
		if (mIsChangeDark)
		{
			if (m_DarkTime > 0 || m_LightTime > 0)
			{
				if (mFadeState == 0)
				{
					if (m_PassTime >= m_TotalTime - m_LightTime)
					{
						mFadeState = 2;
					}
				}
				else if (mFadeState == 1)
				{
					m_CurDarkTime = Math::Min(m_CurDarkTime + frameTime, m_DarkTime);
					if (Scene* pScene = SceneManager::Instance()->getCurrentScene())
					{
						pScene->ChangeDark(1.f - m_Darkness*m_CurDarkTime / m_DarkTime);
					}
					if (m_CurDarkTime >= m_DarkTime) mFadeState = 0;
				}
				else if (mFadeState == 2) //动画路径最后一个段
				{
					m_CurLightTime = Math::Min(m_CurLightTime + frameTime, m_LightTime);
					if (Scene* pScene = SceneManager::Instance()->getCurrentScene())
					{
						pScene->ChangeDark((1.f - m_Darkness) + m_Darkness*m_CurLightTime / m_LightTime);
					}
					if (m_CurLightTime >= m_LightTime) mFadeState = 0;
				}
			}
		}
	}
	
	void CameraAnimation::createEditorHelp( QueryObject* pObj )
	{
		int longitude = 10;
		int latitude = 10;

		pObj->clear();
		pObj->beginPolygon(QueryObject::QO_SOLIDQUAD);
		Real angleLongitude;
		Real angleLatitude = -Math::PI_DIV2;
		Real segmentLongitude = Math::PI_2/longitude;
		Real segmentLatitude = Math::PI/(latitude-1);
		Real radius = editor_r;
		float x, y, z, projectxz;
		Color color(Color::GREEN);

		for(i32 i=0; i<latitude; ++i)
		{
			angleLongitude = 0.0f;
			for(i32 j=0; j<longitude; ++j)
			{
				y = radius * Math::Sin(angleLatitude);
				projectxz = radius * Math::Cos(angleLatitude);
				x = projectxz * Math::Cos(angleLongitude);
				z = projectxz * Math::Sin(angleLongitude);

				pObj->setPosition(x, y, z);
				pObj->setColor(color);
				angleLongitude += segmentLongitude;
			}
			angleLatitude += segmentLatitude;
		}

		pObj->setPosition(radius,0,0);
		pObj->setColor(color);
		pObj->setPosition(-radius,0,0);
		pObj->setColor(color);
		pObj->setPosition(0,0,-radius*4.0f);
		pObj->setColor(color);

		pObj->setPosition(0,radius,0);
		pObj->setColor(color);
		pObj->setPosition(0,-radius,0);
		pObj->setColor(color);
		pObj->setPosition(0,0,-radius*4.0f);
		pObj->setColor(color);

		for(i32 i=0; i<latitude-1; ++i)
		{
			for(i32 j=0; j<longitude; ++j)
			{
				if(j == longitude-1)
				{
					pObj->setIndex(i*(longitude)+j);
					pObj->setIndex((i+1)*(longitude)+j);
					pObj->setIndex((i+1)*(longitude));
					pObj->setIndex(i*(longitude)+j);
					pObj->setIndex((i+1)*(longitude));
					pObj->setIndex(i*(longitude));
				}
				else
				{
					pObj->setIndex(i*(longitude)+j);
					pObj->setIndex((i+1)*(longitude)+j);
					pObj->setIndex((i+1)*(longitude)+j+1);
					pObj->setIndex(i*(longitude)+j);
					pObj->setIndex((i+1)*(longitude)+j+1);
					pObj->setIndex(i*(longitude)+j+1);
				}
			}
		}
		pObj->setIndex(pObj->getVerticesNum()-6);
		pObj->setIndex(pObj->getVerticesNum()-5);
		pObj->setIndex(pObj->getVerticesNum()-4);
		pObj->setIndex(pObj->getVerticesNum()-3);
		pObj->setIndex(pObj->getVerticesNum()-2);
		pObj->setIndex(pObj->getVerticesNum()-1);
		pObj->endPolygon();
	}

	void CameraAnimation::addMainPoint( const Vector3& point, const Quaternion& q, float time )
	{
		PointInfo info(point, q, time);
		Vector3 camPos = point;
		if (m_pActor)
		{
			Vector3 dir_a = (m_pActor->GetOrientation().zAxis()).normalizedCopy();
			Vector3 dir_b = (m_pActor->GetPosition() - camPos);
			info.len = dir_b.normalizeLen();
			info.relateQ = dir_a.getRotationTo(dir_b);
		}
		list<PointInfo>::type::reverse_iterator it = mMainPointList.rbegin();
		if (it == mMainPointList.rend())
		{
			info.time = 0;
		}
		else
		{
			info.time = it->time + time;
		}
		mMainPointList.push_back(info);
		mPointNum = (ui32)mMainPointList.size();

		if (Root::Instance()->isEditorMode())
		{
			SceneNode* pNode = SceneManager::Instance()->getRootNode()->createChild();
			QueryObject* pObj = QueryObjectManager::Instance()->createQueryObject(m_Name+"_"+autoQueryName()+"_"+"PointQuery");
			createEditorHelp(pObj);
			pObj->setTestDepth(true);
			pObj->setWriteDepth(true);
			pObj->setQueryFlag(CAMERA_POINT_QUERY_FLAG);
			pObj->attachTo(pNode);
			m_PointObject.push_back(pObj);
		}
	}

	void CameraAnimation::addMainPoint(const PointInfo& info)
	{
		mMainPointList.push_back(info);
		mPointNum = (ui32)mMainPointList.size();
		if (Root::Instance()->isEditorMode())
		{
			SceneNode* pNode = SceneManager::Instance()->getRootNode()->createChild();
			QueryObject* pObj = QueryObjectManager::Instance()->createQueryObject(m_Name + "_" + autoQueryName() + "_" + "PointQuery");
			createEditorHelp(pObj);
			pObj->setTestDepth(true);
			pObj->setWriteDepth(true);
			pObj->setQueryFlag(CAMERA_POINT_QUERY_FLAG);
			pObj->attachTo(pNode);
			m_PointObject.push_back(pObj);
		}
	}

	void CameraAnimation::insertPoint(QueryObject* pObject, const Vector3& point, const Quaternion& q)
	{
		assert(Root::Instance()->isEditorMode());
		if (m_PointObject.empty())
			return;

		list<QueryObject*>::type::iterator oit = std::find(m_PointObject.begin(), m_PointObject.end(), pObject);
		i32 index = (i32)std::distance(m_PointObject.begin(), oit);
		list<PointInfo>::type::iterator pit = mMainPointList.begin();
		std::advance(pit, index);

		PointInfo info(point, q, 0.f);
		Vector3 camPos = point;
		if (m_pActor)
		{
			Vector3 dir_a = (m_pActor->GetOrientation().zAxis()).normalizedCopy();
			Vector3 dir_b = (m_pActor->GetPosition() - camPos);
			info.len = dir_b.normalizeLen();
			info.relateQ = dir_a.getRotationTo(dir_b);
		}
		list<PointInfo>::type::iterator tempIt = pit++;
		if (pit == mMainPointList.end())
		{
			info.time = tempIt->time + 1.0f;
		}
		else
		{
			info.time = (tempIt->time + pit->time) * 0.5f;
		}
		mMainPointList.insert(pit, info);
		mPointNum = (ui32)mMainPointList.size();

		SceneNode* pNode = SceneManager::Instance()->getRootNode()->createChild();
		QueryObject* pObj = QueryObjectManager::Instance()->createQueryObject(m_Name + "_" + autoQueryName() + "_" + "PointQuery");
		createEditorHelp(pObj);
		pObj->setTestDepth(true);
		pObj->setWriteDepth(true);
		pObj->setQueryFlag(CAMERA_POINT_QUERY_FLAG);
		pObj->attachTo(pNode);
		m_PointObject.insert(++oit, pObj);
	}

	void CameraAnimation::deleteMainPoint( QueryObject* pObject )
	{
		assert(Root::Instance()->isEditorMode());

		list<QueryObject*>::type::iterator oit = std::find(m_PointObject.begin(), m_PointObject.end(), pObject);
		i32 index = (i32)std::distance(m_PointObject.begin(), oit);
		QueryObjectManager::Instance()->destroyQueryObject(pObject);
		m_PointObject.erase(oit);

		list<PointInfo>::type::iterator pit = mMainPointList.begin();
		std::advance(pit,index);
		mMainPointList.erase(pit);
		mPointNum = (ui32)mMainPointList.size();

		m_pPathRender->refleshGpuBuffer();
	}

	void CameraAnimation::refleshPath()
	{
		if (m_pSpline)
		{
			Vector3 transPos;
			Quaternion camQ;
			m_pSpline->clear();
			list<QueryObject*>::type::iterator oit = m_PointObject.begin();
			for (list<PointInfo>::type::iterator it = mMainPointList.begin(); it != mMainPointList.end(); ++it)
			{
				transPos = it->pos;
				camQ = it->qua;
				if (m_pActor)
				{
					Matrix4  matR1, matR2;
					Quaternion ori = m_ActorOriternion;
					camQ = ori * camQ;
					ori.toMat4(matR1);
					it->relateQ.toMat4(matR2);
					matR1 = matR2 * matR1;
					Quaternion newQ;
					newQ.fromMat4(matR1);
					Vector3 dir = newQ.zAxis().normalizedCopy();
					transPos = m_pActor->GetPosition() - dir * it->len;
				}
				if (oit != m_PointObject.end())
				{
					QueryObject* pObject = *oit++;
					if (pObject)
					{
						SceneNode* pNode = pObject->getParent();
						pNode->setLocalPosition(transPos);
						pNode->setLocalOrientation(camQ);
					}
				}
				m_pSpline->addPoint(transPos);
			}

			if (Root::Instance()->isEditorMode())
			{
				m_pPathRender->importPointVector(m_pSpline);
			}
		}
	}

	void CameraAnimation::setColor( const Color& color )
	{
		m_pPathRender->m_Color = color;
		m_pPathRender->refleshGpuBuffer();
	}

	std::pair<CameraAnimation*, i32> CameraAnimation::rayInstersect( const Ray& ray )
	{
		std::pair<CameraAnimation*, i32> result;
		result.first = NULL;
		result.second = -1;
		QueryObjectManager* pManager = QueryObjectManager::Instance();
		QueryObject* pObj = pManager->RayQueryObject(ray, CAMERA_POINT_QUERY_FLAG);
		if (pObj)
		{
			list<QueryObject*>::type::iterator it = std::find(m_PointObject.begin(), m_PointObject.end(), pObj);
			if (it != m_PointObject.end())
			{
				result.first = this;
				result.second = (i32)std::distance(m_PointObject.begin(), it);
			}
		}

		return result;
	}

	QueryObject* CameraAnimation::editor_getPointObject( i32 index )
	{
		list<QueryObject*>::type::iterator it = m_PointObject.begin();
		std::advance(it, index);
		return *it;
	}

	QueryObject* CameraAnimation::editor_getPointObject( const String& name )
	{
		list<QueryObject*>::type::iterator it = m_PointObject.begin();
		for (; it!=m_PointObject.end(); ++it)
		{
			QueryObject* pObject = *it;
			if (pObject->getName() == name)
			{
				return pObject;
			}
		}
		return NULL;
	}

	void CameraAnimation::editor_setPosition( QueryObject* pObject, const Vector3& pos )
	{
		list<QueryObject*>::type::iterator it = m_PointObject.begin();
		assert(it!=m_PointObject.end());
		i32 index = (i32)std::distance(it, std::find(it, m_PointObject.end(), pObject));
		pObject->getParent()->setLocalPosition(pos);
		list<PointInfo>::type::iterator pit = mMainPointList.begin();
		std::advance(pit, index);
		pit->pos = pos;
		if (m_pActor)
		{
			Vector3 dir_a = (m_pActor->GetOrientation().zAxis()).normalizedCopy();
			Vector3 dir_b = (m_pActor->GetPosition() - pos);
			pit->len = dir_b.normalizeLen();
			pit->relateQ = dir_a.getRotationTo(dir_b);
		}
		
		refleshPath();
	}

	void CameraAnimation::editor_setOritation( QueryObject* pObject, const Quaternion& q )
	{
		list<QueryObject*>::type::iterator it = m_PointObject.begin();
		assert(it!=m_PointObject.end());
		i32 index = (i32)std::distance(it, std::find(it, m_PointObject.end(), pObject));
		pObject->getParent()->setLocalOrientation(q);
		list<PointInfo>::type::iterator pit = mMainPointList.begin();
		std::advance(pit, index);
		pit->qua = q;
			
		refleshPath();
	}
	
	float CameraAnimation::editor_getPointTime(i32 index)
	{
		list<PointInfo>::type::iterator pit = mMainPointList.begin();
		std::advance(pit, index);
		return pit->time;
	}

	void CameraAnimation::editor_setPointTime(i32 index, float time)
	{
		list<PointInfo>::type::iterator pit = mMainPointList.begin();
		std::advance(pit, index);
		pit->time = time;
	}

	void CameraAnimation::play()
	{
		if (m_pCamera->getCameraState() == CameraMain::NORMAL &&  m_State != PLAY)
		{
			if (mPointNum < 2)
				return;

			list<QueryObject*>::type::iterator it = m_PointObject.begin();
			for (; it != m_PointObject.end(); ++it)
			{
				(*it)->setVisible(false);
			}

			m_LastPosition = m_pCamera->getPosition();
			m_LastDirection = m_pCamera->getDirection();
			m_LastOriternion = m_pCamera->getOritation();
			mCurrentIt = mMainPointList.begin();
			if (mCurrentIt != mMainPointList.end())
			{
				m_State = PLAY;
				mFadeState = 1;
				mAccIndex = 0;
				mCurrentTime = 0.f;
				mStayCurrentTime = 0.f;
				m_CurDarkTime = 0.f;
				m_CurLightTime = 0.f;
				mStayTime = mCurrentIt->staytime;
				m_PassTime = 0;
				m_TotalTime = getTotalTime();
				mIsStopFade = false;

				Vector3 camPos = mCurrentIt->pos;
				Quaternion camQ = mCurrentIt->qua;
				if (m_pActor)
				{
					ActorManager::Instance()->SetActorsHide(true, m_ActorArray);
					list<PointInfo>::type::iterator nextIt = mCurrentIt; ++nextIt;
					Vector3 pos = m_pSpline->interpolate(mAccIndex, 0);
					if (m_PlayFadeTime > 0.f)
					{
						m_FadeDirection = (pos - m_LastPosition);
						float len = m_FadeDirection.normalizeLen();
						m_FadeSpeed = len / m_PlayFadeTime;
						m_pActor->setOnlyCameraAnim(true);
						if (mIsFocus)
							m_pCamera->setTarget_forLord(m_pActor->GetPosition());
					}
					else
					{
						m_pCamera->setPosition_forLord(pos);
						if (mIsFocus)
						{
							m_pCamera->setTarget_forLord(m_pActor->GetPosition());
						}
						else
						{
							Quaternion q, transQ = mCurrentIt->qua, destQ = nextIt->qua;
							if (m_pActor)
							{
								q = m_ActorOriternion;
								transQ = q * transQ;
								destQ = q * destQ;
							}
							Quaternion::Lerp(q, transQ, destQ, 0, true);
							m_pCamera->setDirection_forLord(q*Vector3::NEG_UNIT_Z);
						}
						m_pActor->setOnlyCameraAnim(mCurrentIt->pause);
					}

					//set time scale
					m_OldSpeedUp = ActorManager::Instance()->GetSpeedUp();
					ActorManager::Instance()->SetSpeedUp(mCurrentIt->timescale*m_OldSpeedUp);
				}
				m_pCamera->setCameraState(CameraMain::ANIMA);

				if (mIsChangeDark)
				{
					if (Scene* pScene = SceneManager::Instance()->getCurrentScene())
					{
						pScene->ChangeDark(1.f - m_Darkness);
					}
				}
			}
		}
	}

	void CameraAnimation::stop()
	{
		list<QueryObject*>::type::iterator it = m_PointObject.begin();
		for (; it != m_PointObject.end(); ++it)
		{
			(*it)->setVisible(true);
		}

		if (m_pActor)
		{
			m_pActor->setOnlyCameraAnim(false);
			ActorManager::Instance()->SetActorsHide(false, m_ActorArray);
			ActorManager::Instance()->SetSpeedUp(m_OldSpeedUp);
		}

		if (mIsChangeDark)
		{
			if (Scene* pScene = SceneManager::Instance()->getCurrentScene())
			{
				if (m_LightTime>0)
				{
					pScene->RevertLight();
				}
			}
		}
		mCurrentTime = 0.f;
		mStayCurrentTime = 0.f;
		mStayTime = 0.f;
		mCurrentIt = mMainPointList.begin();
		m_State = STOP;
		if (m_pCamera->getCameraState() == CameraMain::ANIMA)
		{
			m_pCamera->setCameraState(CameraMain::NORMAL);
			if (!mIsStay)
			{
				m_pCamera->setPosition_forLord(m_LastPosition);
				m_pCamera->setDirection_forLord(m_LastDirection);
			}
		}

		m_OldSpeedUp = 1.0f;
	}

	void CameraAnimation::pause()
	{
		m_State = PAUSE;
	}

	void CameraAnimation::getPropertyList( PropertyList & lst )
	{
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_NAME], false));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_POSITION], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_ORITATION], false));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_PITCH], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_YAW], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_ROLL], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_TIME], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_STAYTIME], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_TIMESCALE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_PAUSESKILL], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_DARKNESS], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_DARKTIME], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_LIGHTNESS], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_LIGHTTIME], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_ISFOCUS], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_PLAYFADETIME], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_STOPFADETIME], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_ISSTAY], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_CA_ISDARK], true));
	}

	bool CameraAnimation::getPropertyType( const String & name, PropertyType &type )
	{
		if (name == objToken[TOKEN_OBJ_NAME])
		{
			type = PT_STRING; return true;
		}
		else if (name == objToken[TOKEN_OBJ_POSITION])
		{
			type = PT_VECTOR3; return true;
		}
		else if (name == objToken[TOKEN_OBJ_ORITATION])
		{
			type = PT_QUATERNION; return true;
		}
		else if (name == objToken[TOKEN_OBJ_PITCH])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_OBJ_YAW])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_OBJ_ROLL])
		{
			type = PT_REAL; return true;
		}
		else if(name == objToken[TOKEN_CA_TIME])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_CA_STAYTIME])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_CA_TIMESCALE])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_CA_PAUSESKILL])
		{
			type = PT_BOOL; return true;
		}
		else if (name == objToken[TOKEN_CA_DARKNESS])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_CA_DARKTIME])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_CA_LIGHTNESS])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_CA_LIGHTTIME])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_CA_ISFOCUS])
		{
			type = PT_BOOL; return true;
		}
		else if (name == objToken[TOKEN_CA_PLAYFADETIME])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_CA_STOPFADETIME])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_CA_ISSTAY])
		{
			type = PT_BOOL; return true;
		}
		else if (name == objToken[TOKEN_CA_ISDARK])
		{
			type = PT_BOOL; return true;
		}
		return false;
	}

	bool CameraAnimation::getPropertyValue( const String & name, String &value, QueryObject* pObj )
	{
		if (name == objToken[TOKEN_OBJ_NAME])
		{
			value = m_Name; return true;
		}
		else if (name == objToken[TOKEN_OBJ_POSITION])
		{
			if (pObj)
			{
				value = StringUtil::ToString(pObj->getParent()->getLocalPosition());
				return true;
			}
			return false;
		}
		else if (name == objToken[TOKEN_OBJ_ORITATION])
		{
			if (pObj)
			{
				const Quaternion& q = pObj->getParent()->getLocalOrientation();
				String w = StringUtil::ToString(q.w);
				String x = StringUtil::ToString(q.x);
				String y = StringUtil::ToString(q.y);
				String z = StringUtil::ToString(q.z);
				// value = StringUtil::ToString(m_Oritation); return true;
				value = w + " " + x + " " + y + " " + z;
				return true;
			}
			return false;
		}
		else if (name == objToken[TOKEN_OBJ_PITCH])
		{
			if (pObj)
			{				
				CaculateAngle(pObj->getParent()->getLocalOrientation());
				value = StringUtil::ToString(m_Pitch);
				return true;
			}
			return false;
		}
		else if (name == objToken[TOKEN_OBJ_YAW])
		{
			if (pObj)
			{
				CaculateAngle(pObj->getParent()->getLocalOrientation());
				value = StringUtil::ToString(m_Yaw);
				return true;
			}
			return false;
		}
		else if (name == objToken[TOKEN_OBJ_ROLL])
		{
			if (pObj)
			{
				CaculateAngle(pObj->getParent()->getLocalOrientation());
				value = StringUtil::ToString(m_Roll);
				return true;
			}

			return false;
		}
		else if (name == objToken[TOKEN_CA_TIME])
		{
			list<QueryObject*>::type::iterator it = std::find(m_PointObject.begin(), m_PointObject.end(), pObj);
			ui32 index = (ui32)std::distance(m_PointObject.begin(), it);

			list<PointInfo>::type::iterator itor = mMainPointList.begin();
			std::advance(itor, index);
			value = StringUtil::ToString(itor->time);
			return true;
		}
		else if (name == objToken[TOKEN_CA_STAYTIME])
		{
			list<QueryObject*>::type::iterator it = std::find(m_PointObject.begin(), m_PointObject.end(), pObj);
			ui32 index = (ui32)std::distance(m_PointObject.begin(), it);

			list<PointInfo>::type::iterator itor = mMainPointList.begin();
			std::advance(itor, index);
			value = StringUtil::ToString(itor->staytime);
			return true;
		}
		else if (name == objToken[TOKEN_CA_TIMESCALE])
		{
			list<QueryObject*>::type::iterator it = std::find(m_PointObject.begin(), m_PointObject.end(), pObj);
			ui32 index = (ui32)std::distance(m_PointObject.begin(), it);

			list<PointInfo>::type::iterator itor = mMainPointList.begin();
			std::advance(itor, index);
			value = StringUtil::ToString(itor->timescale);
			return true;
		}
		else if (name == objToken[TOKEN_CA_PAUSESKILL])
		{
			list<QueryObject*>::type::iterator it = std::find(m_PointObject.begin(), m_PointObject.end(), pObj);
			ui32 index = (ui32)std::distance(m_PointObject.begin(), it);

			list<PointInfo>::type::iterator itor = mMainPointList.begin();
			std::advance(itor, index);
			value = StringUtil::ToString(itor->pause);
			return true;
		}
		else if (name == objToken[TOKEN_CA_DARKNESS])
		{
			value = StringUtil::ToString(m_Darkness);
			return true;
		}
		else if (name == objToken[TOKEN_CA_DARKTIME])
		{
			value = StringUtil::ToString(m_DarkTime);
			return true;
		}
		else if (name == objToken[TOKEN_CA_LIGHTNESS])
		{
			value = StringUtil::ToString(m_Lightness);
			return true;
		}
		else if (name == objToken[TOKEN_CA_LIGHTTIME])
		{
			value = StringUtil::ToString(m_LightTime);
			return true;
		}
		else if (name == objToken[TOKEN_CA_ISFOCUS])
		{
			value = StringUtil::ToString(mIsFocus);
			return true;
		}
		else if (name == objToken[TOKEN_CA_PLAYFADETIME])
		{
			value = StringUtil::ToString(m_PlayFadeTime);
			return true;
		}
		else if (name == objToken[TOKEN_CA_STOPFADETIME])
		{
			value = StringUtil::ToString(m_StopFadeTime);
			return true;
		}
		else if (name == objToken[TOKEN_CA_ISSTAY])
		{
			value = StringUtil::ToString(mIsStay);
			return true;
		}
		else if (name == objToken[TOKEN_CA_ISDARK])
		{
			value = StringUtil::ToString(mIsChangeDark);
			return true;
		}
		
		return false;
	}

	bool CameraAnimation::setPropertyValue( const String & name,const String & value, QueryObject* pObj )
	{
		if (name == objToken[TOKEN_OBJ_NAME])
		{
			m_Name = value; 
			return true;
		}
		else if (name == objToken[TOKEN_OBJ_POSITION])
		{
			if (pObj)
			{
				editor_setPosition(pObj, StringUtil::ParseVec3(value));
				return true;
			}

			return false;
		}
		else if (name == objToken[TOKEN_OBJ_PITCH])
		{
			if (pObj)
			{
				m_Pitch = StringUtil::ParseReal(value);
				editor_setOritation(pObj, CaculateOritation());
				return true;
			}

			return false;
		}
		else if (name == objToken[TOKEN_OBJ_YAW])
		{
			if (pObj)
			{
				m_Yaw = StringUtil::ParseReal(value);
				editor_setOritation(pObj, CaculateOritation());
				return true;
			}

			return false;
		}
		else if (name == objToken[TOKEN_OBJ_ROLL])
		{
			if (pObj)
			{
				m_Roll = StringUtil::ParseReal(value);
				editor_setOritation(pObj, CaculateOritation());
				return true;
			}

			return false;
		}
		else if (name == objToken[TOKEN_CA_TIME])
		{
			list<QueryObject*>::type::iterator it = std::find(m_PointObject.begin(), m_PointObject.end(), pObj);
			ui32 index = (ui32)std::distance(m_PointObject.begin(), it);

			list<PointInfo>::type::iterator itor = mMainPointList.begin();
			std::advance(itor, index);
			itor->time = StringUtil::ParseFloat(value); 
			return true;
		}
		else if (name == objToken[TOKEN_CA_STAYTIME])
		{
			list<QueryObject*>::type::iterator it = std::find(m_PointObject.begin(), m_PointObject.end(), pObj);
			ui32 index = (ui32)std::distance(m_PointObject.begin(), it);

			list<PointInfo>::type::iterator itor = mMainPointList.begin();
			std::advance(itor, index);
			itor->staytime = StringUtil::ParseFloat(value);
			return true;
		}
		else if (name == objToken[TOKEN_CA_TIMESCALE])
		{
			list<QueryObject*>::type::iterator it = std::find(m_PointObject.begin(), m_PointObject.end(), pObj);
			ui32 index = (ui32)std::distance(m_PointObject.begin(), it);

			list<PointInfo>::type::iterator itor = mMainPointList.begin();
			std::advance(itor, index);
			itor->timescale = StringUtil::ParseFloat(value);
			return true;
		}
		else if (name == objToken[TOKEN_CA_PAUSESKILL])
		{
			list<QueryObject*>::type::iterator it = std::find(m_PointObject.begin(), m_PointObject.end(), pObj);
			ui32 index = (ui32)std::distance(m_PointObject.begin(), it);

			list<PointInfo>::type::iterator itor = mMainPointList.begin();
			std::advance(itor, index);
			itor->pause = StringUtil::ParseBool(value);
			return true;
		}
		else if (name == objToken[TOKEN_CA_DARKNESS])
		{
			m_Darkness = StringUtil::ParseReal(value);
			return true;
		}
		else if (name == objToken[TOKEN_CA_DARKTIME])
		{
			m_DarkTime = StringUtil::ParseReal(value);
			return true;
		}
		else if (name == objToken[TOKEN_CA_LIGHTNESS])
		{
			m_Lightness = StringUtil::ParseReal(value);
			return true;
		}
		else if (name == objToken[TOKEN_CA_LIGHTTIME])
		{
			m_LightTime = StringUtil::ParseReal(value);
			return true;
		}
		else if (name == objToken[TOKEN_CA_ISFOCUS])
		{
			mIsFocus = StringUtil::ParseBool(value);
			return true;
		}
		else if (name == objToken[TOKEN_CA_PLAYFADETIME])
		{
			m_PlayFadeTime = StringUtil::ParseReal(value);
			return true;
		}
		else if (name == objToken[TOKEN_CA_STOPFADETIME])
		{
			m_StopFadeTime = StringUtil::ParseReal(value);
			return true;
		}
		else if (name == objToken[TOKEN_CA_ISSTAY])
		{
			mIsStay = StringUtil::ParseBool(value);
			return true;
		}
		else if (name == objToken[TOKEN_CA_ISDARK])
		{
			mIsChangeDark = StringUtil::ParseBool(value);
			return true;
		}

		return false;
	}

	void CameraAnimation::exportData( DataStream& dataStream )
	{
		ui32 pointnum = (ui32)mMainPointList.size();
		dataStream.write(&pointnum, sizeof(ui32));
		
		for (list<PointInfo>::type::iterator it=mMainPointList.begin(); it!=mMainPointList.end(); ++it)
		{
			dataStream.write(&it->pos, sizeof(Vector3));
			dataStream.write(&it->qua, sizeof(Quaternion));
			dataStream.write(&it->relateQ, sizeof(Quaternion));
			dataStream.write(&it->len, sizeof(float));
			dataStream.write(&it->time, sizeof(float));
			dataStream.write(&it->staytime, sizeof(float));
			//dataStream.write(&it->timescale, sizeof(float));
			dataStream.write(&it->pause, sizeof(bool));
		}
		dataStream.write(&mIsFocus, sizeof(bool));
		dataStream.write(&m_DarkTime, sizeof(float));
		dataStream.write(&m_Darkness, sizeof(float));
		dataStream.write(&m_PlayFadeTime, sizeof(float));
		dataStream.write(&m_StopFadeTime, sizeof(float));
		dataStream.write(&mIsStay, sizeof(bool));
		dataStream.write(&m_LightTime, sizeof(float));
		dataStream.write(&m_Lightness, sizeof(float));
	}

	void CameraAnimation::importData( DataStream& dataStream )
	{
		ui32 pointnum = 0;
		dataStream.read(&pointnum, sizeof(ui32));
		for (ui32 i=0; i<pointnum; ++i)
		{
			Vector3 pos;
			Quaternion q, relateQ;
			float time, len, staytime/*, timescale*/;
			bool pause;
			dataStream.read(&pos, sizeof(Vector3));
			dataStream.read(&q, sizeof(Quaternion));
			dataStream.read(&relateQ, sizeof(Quaternion));
			dataStream.read(&len, sizeof(float));
			dataStream.read(&time, sizeof(float));
			dataStream.read(&staytime, sizeof(float));
			//dataStream.read(&timescale, sizeof(float));
			dataStream.read(&pause, sizeof(bool));
			PointInfo info(pos, q, time);
			info.relateQ = relateQ;
			info.len = len;
			info.staytime = staytime;
			//info.timescale = timescale;
			info.pause = pause;
			addMainPoint(info);
		}
		dataStream.read(&mIsFocus, sizeof(bool));
		dataStream.read(&m_DarkTime, sizeof(float));
		dataStream.read(&m_Darkness, sizeof(float));
		dataStream.read(&m_PlayFadeTime, sizeof(float));
		dataStream.read(&m_StopFadeTime, sizeof(float));
		dataStream.read(&mIsStay, sizeof(bool));
		dataStream.read(&m_LightTime, sizeof(float));
		dataStream.read(&m_Lightness, sizeof(float));
	}

	void CameraAnimation::exportData(rapidxml::xml_document<>& doc)
	{
		rapidxml::xml_node<>* PointRoot = doc.allocate_node(rapidxml::node_element, "PointRoot", NULL);
		doc.append_node(PointRoot);
		for (list<PointInfo>::type::iterator it = mMainPointList.begin(); it != mMainPointList.end(); ++it)
		{
			char* strPos = doc.allocate_string(StringUtil::ToString(it->pos).c_str());
			char* strQua = doc.allocate_string(StringUtil::ToString(it->qua).c_str());
			char* strRelateQ = doc.allocate_string(StringUtil::ToString(it->relateQ).c_str());
			char* strLen = doc.allocate_string(StringUtil::ToString(it->len).c_str());
			char* strTime = doc.allocate_string(StringUtil::ToString(it->time).c_str());
			char* strStayTime = doc.allocate_string(StringUtil::ToString(it->staytime).c_str());
			char* strTimeScale = doc.allocate_string(StringUtil::ToString(it->timescale).c_str());
			char* strPause = doc.allocate_string(StringUtil::ToString(it->pause).c_str());

			rapidxml::xml_node<>* animPoint = doc.allocate_node(rapidxml::node_element, "AnimtionPoint", NULL);
			animPoint->append_attribute(doc.allocate_attribute("pos", strPos));
			animPoint->append_attribute(doc.allocate_attribute("que", strQua));
			animPoint->append_attribute(doc.allocate_attribute("relateq", strRelateQ));
			animPoint->append_attribute(doc.allocate_attribute("len", strLen));
			animPoint->append_attribute(doc.allocate_attribute("time", strTime));
			animPoint->append_attribute(doc.allocate_attribute("staytime", strStayTime));
			animPoint->append_attribute(doc.allocate_attribute("timescale", strTimeScale));
			animPoint->append_attribute(doc.allocate_attribute("pause", strPause));
			PointRoot->append_node(animPoint);
		}

		rapidxml::xml_node<>* CommonRoot = doc.allocate_node(rapidxml::node_element, "CommonRoot", NULL);
		doc.append_node(CommonRoot);
		rapidxml::xml_node<>* FocusNode = doc.allocate_node(rapidxml::node_element, "focus", NULL);
		rapidxml::xml_node<>* DarkTimeNode = doc.allocate_node(rapidxml::node_element, "darktime", NULL);
		rapidxml::xml_node<>* DarkNessNode = doc.allocate_node(rapidxml::node_element, "darkness", NULL);
		rapidxml::xml_node<>* LightTimeNode = doc.allocate_node(rapidxml::node_element, "lighttime", NULL);
		rapidxml::xml_node<>* LightNessNode = doc.allocate_node(rapidxml::node_element, "lightness", NULL);
		rapidxml::xml_node<>* PlayFadeNode = doc.allocate_node(rapidxml::node_element, "playfadetime", NULL);
		rapidxml::xml_node<>* StopFadeNode = doc.allocate_node(rapidxml::node_element, "stopfadetime", NULL);
		rapidxml::xml_node<>* IsStayNode = doc.allocate_node(rapidxml::node_element, "isstay", NULL);
		rapidxml::xml_node<>* IsDarkNode = doc.allocate_node(rapidxml::node_element, "isdark", NULL);

		CommonRoot->append_node(FocusNode);
		CommonRoot->append_node(DarkTimeNode);
		CommonRoot->append_node(DarkNessNode);
		CommonRoot->append_node(LightTimeNode);
		CommonRoot->append_node(LightNessNode);
		CommonRoot->append_node(PlayFadeNode);
		CommonRoot->append_node(StopFadeNode);
		CommonRoot->append_node(IsStayNode);
		CommonRoot->append_node(IsDarkNode);

		char* strFocus = doc.allocate_string(StringUtil::ToString(mIsFocus).c_str());
		char* strDarkTime = doc.allocate_string(StringUtil::ToString(m_DarkTime).c_str());
		char* strDarkNess = doc.allocate_string(StringUtil::ToString(m_Darkness).c_str());
		char* strLightTime = doc.allocate_string(StringUtil::ToString(m_LightTime).c_str());
		char* strLightNess = doc.allocate_string(StringUtil::ToString(m_Lightness).c_str());
		char* strPlayFadeTime = doc.allocate_string(StringUtil::ToString(m_PlayFadeTime).c_str());
		char* strStopFadeTime = doc.allocate_string(StringUtil::ToString(m_StopFadeTime).c_str());
		char* strIsStay = doc.allocate_string(StringUtil::ToString(mIsStay).c_str());
		char* strIsDark = doc.allocate_string(StringUtil::ToString(mIsChangeDark).c_str());

		FocusNode->append_attribute(doc.allocate_attribute("value", strFocus));
		DarkTimeNode->append_attribute(doc.allocate_attribute("value", strDarkTime));
		DarkNessNode->append_attribute(doc.allocate_attribute("value", strDarkNess));
		LightTimeNode->append_attribute(doc.allocate_attribute("value", strLightTime));
		LightNessNode->append_attribute(doc.allocate_attribute("value", strLightNess));
		PlayFadeNode->append_attribute(doc.allocate_attribute("value", strPlayFadeTime));
		StopFadeNode->append_attribute(doc.allocate_attribute("value", strStopFadeTime));
		IsStayNode->append_attribute(doc.allocate_attribute("value", strIsStay));
		IsDarkNode->append_attribute(doc.allocate_attribute("value", strIsDark));
	}

	void CameraAnimation::importData(rapidxml::xml_node<>* parentNode)
	{
		if (!parentNode)
		{
			LordLogError("the camAnim file content is valid.It does not contain point root node!");
			return;
		}

		rapidxml::xml_node<> *pPointNode = parentNode->first_node();
		while (pPointNode)
		{
			Vector3 pos;
			Quaternion q, relateQ;
			float time, len, staytime, timescale;
			bool pause;

			rapidxml::xml_attribute<> *attribute = pPointNode->first_attribute();
			while (attribute)
			{
				String strnode = attribute->name();
				char* cvalue = attribute->value();
				if (strnode == "pos")
				{
					pos = StringUtil::ParseVec3(cvalue);
				}
				else if (strnode == "que")
				{
					q = StringUtil::ParseQuaternion(cvalue);
				}
				else if (strnode == "relateq")
				{
					relateQ = StringUtil::ParseQuaternion(cvalue);
				}
				else if (strnode == "len")
				{
					len = StringUtil::ParseFloat(cvalue);
				}
				else if (strnode == "time")
				{
					time = StringUtil::ParseFloat(cvalue);
				}
				else if (strnode == "staytime")
				{
					staytime = StringUtil::ParseFloat(cvalue);
				}
				else if (strnode == "timescale")
				{
					timescale = StringUtil::ParseFloat(cvalue);
				}
				else if (strnode == "pause")
				{
					pause = StringUtil::ParseBool(cvalue);
				}
				attribute = attribute->next_attribute();
			}

			pPointNode = pPointNode->next_sibling();
			PointInfo info(pos, q, time);
			info.relateQ = relateQ;
			info.len = len;
			info.staytime = staytime;
			info.timescale = timescale;
			info.pause = pause;
			addMainPoint(info);
		}

		rapidxml::xml_node<> * commonNode = parentNode->next_sibling();
		if (commonNode)
		{
			rapidxml::xml_node<>* pNode = commonNode->first_node();
			while (pNode)
			{
				rapidxml::xml_attribute<>* valueNode = pNode->first_attribute(); LordAssert(valueNode);
				String strnode = pNode->name();
				char* cvalue = valueNode->value();
				if (strnode == "focus")
				{
					mIsFocus = StringUtil::ParseBool(cvalue);
				}
				else if (strnode == "darktime")
				{
					m_DarkTime = StringUtil::ParseFloat(cvalue);
				}
				else if (strnode == "darkness")
				{
					m_Darkness = StringUtil::ParseFloat(cvalue);
				}
				else if (strnode == "lighttime")
				{
					m_LightTime = StringUtil::ParseFloat(cvalue);
				}
				else if (strnode == "lightness")
				{
					m_Lightness = StringUtil::ParseFloat(cvalue);
				}
				else if (strnode == "playfadetime")
				{
					m_PlayFadeTime = StringUtil::ParseFloat(cvalue);
				}
				else if (strnode == "stopfadetime")
				{
					m_StopFadeTime = StringUtil::ParseFloat(cvalue);
				}
				else if (strnode == "isstay")
				{
					mIsStay = StringUtil::ParseBool(cvalue);
				}
				else if (strnode == "isdark")
				{
					mIsChangeDark = StringUtil::ParseBool(cvalue);
				}
				
				pNode = pNode->next_sibling();
			}
		}//if (commonNode)
	}

	void CameraAnimation::exportData(rapidjson::Document* doc)
	{
		doc->SetObject();



		rapidjson::Value pointRoot(rapidjson::kArrayType);
		doc->AddMember("PointRoot", pointRoot, doc->GetAllocator());

		rapidjson::Value& refPointRoot = (*doc)["PointRoot"];
		for (list<PointInfo>::type::iterator it = mMainPointList.begin(); it != mMainPointList.end(); ++it)
		{
			String strPos = StringUtil::ToString(it->pos);
			String strQua = StringUtil::ToString(it->qua);
			String strRelateQ = StringUtil::ToString(it->relateQ);
			String strLen = StringUtil::ToString(it->len);
			String strTime = StringUtil::ToString(it->time);
			String strStayTime = StringUtil::ToString(it->staytime);
			String strTimeScale = StringUtil::ToString(it->timescale);
			String strPause = StringUtil::ToString(it->pause);


			rapidjson::Value valPos(strPos.c_str(), doc->GetAllocator());
			rapidjson::Value valQua(strQua.c_str(), doc->GetAllocator());
			rapidjson::Value valRelateQ(strRelateQ.c_str(), doc->GetAllocator());
			rapidjson::Value valLen(strLen.c_str(), doc->GetAllocator());
			rapidjson::Value valTime(strTime.c_str(), doc->GetAllocator());
			rapidjson::Value valStayTime(strStayTime.c_str(), doc->GetAllocator());
			rapidjson::Value valTimeScale(strTimeScale.c_str(), doc->GetAllocator());
			rapidjson::Value valPause(strPause.c_str(), doc->GetAllocator());

			rapidjson::Value animPoint(rapidjson::kObjectType);
			animPoint.AddMember("pos", valPos, doc->GetAllocator());
			animPoint.AddMember("qua", valQua, doc->GetAllocator());
			animPoint.AddMember("relateq", valRelateQ, doc->GetAllocator());
			animPoint.AddMember("len", valLen, doc->GetAllocator());
			animPoint.AddMember("time", valTime, doc->GetAllocator());
			animPoint.AddMember("staytime", valStayTime, doc->GetAllocator());
			animPoint.AddMember("timescale", valTimeScale, doc->GetAllocator());
			animPoint.AddMember("pause", valPause, doc->GetAllocator());

			refPointRoot.PushBack(animPoint, doc->GetAllocator());
		}

		rapidjson::Value commonRoot(rapidjson::kObjectType);
		doc->AddMember("CommonRoot", commonRoot, doc->GetAllocator());

		rapidjson::Value& refCommonRoot = (*doc)["CommonRoot"];

		String strFocus = StringUtil::ToString(mIsFocus);
		String strDarkTime = StringUtil::ToString(m_DarkTime);
		String strDarkNess = StringUtil::ToString(m_Darkness);
		String strLightTime = StringUtil::ToString(m_LightTime);
		String strLightNess = StringUtil::ToString(m_Lightness);
		String strPlayFadeTime = StringUtil::ToString(m_PlayFadeTime);
		String strStopFadeTime = StringUtil::ToString(m_StopFadeTime);
		String strIsStay = StringUtil::ToString(mIsStay);
		String strIsDark = StringUtil::ToString(mIsChangeDark);

		rapidjson::Value valFocus(strFocus.c_str(), doc->GetAllocator());
		rapidjson::Value valDarkTime(strDarkTime.c_str(), doc->GetAllocator());
		rapidjson::Value valDarkNess(strDarkNess.c_str(), doc->GetAllocator());
		rapidjson::Value valLightTime(strLightTime.c_str(), doc->GetAllocator());
		rapidjson::Value valLightNess(strLightNess.c_str(), doc->GetAllocator());
		rapidjson::Value valPlayFadeTime(strPlayFadeTime.c_str(), doc->GetAllocator());
		rapidjson::Value valStopFadeTime(strStopFadeTime.c_str(), doc->GetAllocator());
		rapidjson::Value valIsStay(strIsStay.c_str(), doc->GetAllocator());
		rapidjson::Value valIsDark(strIsDark.c_str(), doc->GetAllocator());

		refCommonRoot.AddMember("focus", valFocus, doc->GetAllocator());
		refCommonRoot.AddMember("darktime", valDarkTime, doc->GetAllocator());
		refCommonRoot.AddMember("darkness", valDarkNess, doc->GetAllocator());
		refCommonRoot.AddMember("lighttime", valLightTime, doc->GetAllocator());
		refCommonRoot.AddMember("lightness", valLightNess, doc->GetAllocator());
		refCommonRoot.AddMember("playfadetime", valPlayFadeTime, doc->GetAllocator());
		refCommonRoot.AddMember("stopfadetime", valStopFadeTime, doc->GetAllocator());
		refCommonRoot.AddMember("isstay", valIsStay, doc->GetAllocator());
		refCommonRoot.AddMember("isdark", valIsDark, doc->GetAllocator());

	}

	void CameraAnimation::importData(const rapidjson::Value& node)
	{
		if (!node.IsObject())
		{
			LordLogError("the camAnim file content is valid.");
			return;
		}

		//PointNode
		rapidjson::Value::ConstMemberIterator itNode = node.MemberBegin();
		if (itNode != node.MemberEnd())
		{
			rapidjson::Value::ConstValueIterator itPointNode = itNode->value.Begin();
			for (; itPointNode != itNode->value.End(); ++itPointNode)
			{
				Vector3 pos;
				Quaternion q, relateQ;
				float time, len, staytime, timescale;
				bool pause;

				rapidjson::Value::ConstMemberIterator itAttribute = (*itPointNode).MemberBegin();
				for (; itAttribute != (*itPointNode).MemberEnd(); ++itAttribute)
				{
					String strName = itAttribute->name.GetString();
					String strValue = itAttribute->value.GetString();

					if (strName == "pos")
					{
						pos = StringUtil::ParseVec3(strValue);
					}
					else if (strName == "qua")
					{
						q = StringUtil::ParseQuaternion(strValue);
					}
					else if (strName == "relateq")
					{
						relateQ = StringUtil::ParseQuaternion(strValue);
					}
					else if (strName == "len")
					{
						len = StringUtil::ParseFloat(strValue);
					}
					else if (strName == "time")
					{
						time = StringUtil::ParseFloat(strValue);
					}
					else if (strName == "staytime")
					{
						staytime = StringUtil::ParseFloat(strValue);
					}
					else if (strName == "timescale")
					{
						timescale = StringUtil::ParseFloat(strValue);
					}
					else if (strName == "pause")
					{
						pause = StringUtil::ParseBool(strValue);
					}

				}//end for itAttribute

				PointInfo info(pos, q, time);
				info.relateQ = relateQ;
				info.len = len;
				info.staytime = staytime;
				info.timescale = timescale;
				info.pause = pause;
				addMainPoint(info);

			}//end for itPointNode
		}


		//CommonNode
		++itNode;
		if (itNode != node.MemberEnd())
		{
			rapidjson::Value::ConstMemberIterator itCommonNode = itNode->value.MemberBegin();

			if (itCommonNode != itNode->value.MemberEnd())
			{
				rapidjson::Value::ConstMemberIterator itAttribute = itCommonNode->value.MemberBegin();
				for (; itAttribute != itCommonNode->value.MemberEnd(); ++itAttribute)
				{
					String strName = itAttribute->name.GetString();
					String strValue = itAttribute->value.GetString();

					if (strName == "focus")
					{
						mIsFocus = StringUtil::ParseBool(strValue);
					}
					else if (strName == "darktime")
					{
						m_DarkTime = StringUtil::ParseFloat(strValue);
					}
					else if (strName == "darkness")
					{
						m_Darkness = StringUtil::ParseFloat(strValue);
					}
					else if (strName == "lighttime")
					{
						m_LightTime = StringUtil::ParseFloat(strValue);
					}
					else if (strName == "lightness")
					{
						m_Lightness = StringUtil::ParseFloat(strValue);
					}
					else if (strName == "playfadetime")
					{
						m_PlayFadeTime = StringUtil::ParseFloat(strValue);
					}
					else if (strName == "stopfadetime")
					{
						m_StopFadeTime = StringUtil::ParseFloat(strValue);
					}
					else if (strName == "isstay")
					{
						mIsStay = StringUtil::ParseBool(strValue);
					}
					else if (strName == "isdark")
					{
						mIsChangeDark = StringUtil::ParseBool(strValue);
					}
				}
			}

		}

	}

	void CameraAnimation::setTargetActor(ActorObject* pTarget)
	{
		m_pActor = pTarget;
		if (m_pActor) m_ActorOriternion = m_pActor->GetOrientation();
		refleshPath();
	}

	float CameraAnimation::getPauseLength()
	{
		float length = 0;
		list<PointInfo>::type::const_iterator tempIt;
		list<PointInfo>::type::const_iterator it = mMainPointList.begin();
		for (; it != mMainPointList.end();)
		{
			tempIt = it++;
			if (tempIt->pause)
			{
				if(it != mMainPointList.end())
					length += it->time - tempIt->time + tempIt->staytime;
			}
		}

		return length;
	}

	float CameraAnimation::getTotalTime()
	{
		float length = 0.0f;
		list<PointInfo>::type::const_reverse_iterator it = mMainPointList.rbegin();
		for (; it != mMainPointList.rend();)
		{
			list<PointInfo>::type::const_reverse_iterator temp = it++;
			if (it != mMainPointList.rend())
			{
				length += temp->time - it->time + temp->staytime;
			}
			else
			{
				length += temp->staytime;
			}
		}

		return  length + m_StopFadeTime + m_PlayFadeTime;
	}

	LORD::Quaternion CameraAnimation::CaculateOritation()
	{
		float fCosHRoll = Math::Cos(m_Roll*Math::DEG2RAD * 0.5f);
		float fSinHRoll = Math::Sin(m_Roll*Math::DEG2RAD * 0.5f);
		float fCosHPitch = Math::Cos(m_Pitch*Math::DEG2RAD * 0.5f);
		float fSinHPitch = Math::Sin(m_Pitch*Math::DEG2RAD * 0.5f);
		float fCosHYaw = Math::Cos(m_Yaw*Math::DEG2RAD * 0.5f);
		float fSinHYaw = Math::Sin(m_Yaw*Math::DEG2RAD * 0.5f);

		float w, x, y, z;
		w = fCosHRoll * fCosHPitch * fCosHYaw + fSinHRoll * fSinHPitch * fSinHYaw;
		x = fCosHRoll * fSinHPitch * fCosHYaw + fSinHRoll * fCosHPitch * fSinHYaw;
		y = fCosHRoll * fCosHPitch * fSinHYaw - fSinHRoll * fSinHPitch * fCosHYaw;
		z = fSinHRoll * fCosHPitch * fCosHYaw - fCosHRoll * fSinHPitch * fSinHYaw;

		return Quaternion(w, x, y, z);
	}

	void CameraAnimation::CaculateAngle(const Quaternion& q)
	{
		float w, x, y, z;
		w = q.w;
		x = q.x;
		y = q.y;
		z = q.z;

		float roll = Math::ATan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (z * z + x * x));
		float pitch = Math::ASin(2.0f * (w * x - y * z));
		float yaw = Math::ATan2(2.0f * (w * y + z * x), 1.0f - 2.0f * (x * x + y * y));

		m_Pitch = pitch * Math::RAD2DEG;
		m_Yaw = yaw * Math::RAD2DEG;
		m_Roll = roll * Math::RAD2DEG;
	}
//////////////////////////////////////////////////////////////////////////
	// CameraPathRender
	CameraPathRender::CameraPathRender(const String& name)
		: mName(name)
		, m_Color(Color::WHITE)
		, m_pLineObject(NULL)
	{
		m_pQueryManager = QueryObjectManager::Instance();
	}

	CameraPathRender::~CameraPathRender()
	{
		if (m_pLineObject)
		{
			SceneNode* pNode = m_pLineObject->getParent();
			pNode->getParent()->destroyChild(pNode);
			m_pQueryManager->destroyQueryObject(m_pLineObject);
			m_pLineObject = NULL;
		}
	}

	void CameraPathRender::refleshGpuBuffer()
	{
		if (mPointVector.empty())
			return;

		m_pLineObject->clear();
		m_pLineObject->beginPolygon(QueryObject::QO_WIREFRAME);
		{
			ui32 count = (ui32)mPointVector.size();
			for (ui32 i=0; i<count; ++i)
			{
				m_pLineObject->setPosition(mPointVector[i]);
				m_pLineObject->setColor(m_Color);
			}

			for (ui32 i=0; i<count-1; ++i)
			{
				m_pLineObject->setIndex(i);
				m_pLineObject->setIndex(i+1);
			}
		}
		m_pLineObject->endPolygon();
	}

	void CameraPathRender::importPointVector( SimpleSpline* spline )
	{
		if (m_pLineObject)
		{
			SceneNode* pNode = m_pLineObject->getParent();
			pNode->getParent()->destroyChild(pNode);
			m_pQueryManager->destroyQueryObject(m_pLineObject);
			m_pLineObject = NULL;
		}

		SceneNode* pNode = SceneManager::Instance()->getRootNode()->createChild();
		m_pLineObject = m_pQueryManager->createQueryObject(mName + "query");
		m_pLineObject->attachTo(pNode);

		mPointVector.clear();
		int pointNum = spline->getNumPoints();
		if (pointNum>0)
		{
			int totalNum = -1;
			//如果只有一个点
			if (pointNum==1)
			{
				mPointVector.push_back(spline->getPoint(0));
			}
			else //根据两点间距离决定从spline取得点数，保证取出的点距为1个单位长度
			{				
				totalNum = 0;
				for (int i=0; i<pointNum-1; ++i)
					totalNum += int( (spline->getPoint(i)-spline->getPoint(i+1)).len());
			}

			//取出点往辅助线塞
			totalNum*=2;
			for (int i=0; i<=totalNum; ++i)
			{
				Vector3 pos = spline->interpolate((float)i/(float)totalNum);

				mPointVector.push_back(pos);
			}
		}
		refleshGpuBuffer();
	}

}