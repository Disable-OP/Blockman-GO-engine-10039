#include "NavigatePoint.h"
#include "SceneDefine.h"
#include "Util/QueryObject.h"
#include "Geom/SimpleSpline.h"
#include "Object/Root.h"
#include "Scene.h"
#include "Actor/ActorManager.h"
#include "Actor/ActorObject.h"
#include "UI/GUIString.h"

namespace LORD
{
	String GetNewPointName()
	{
		static int pointid = 0;
		return StringUtil::ToString(pointid++);
	}

	const float perSecondSpeed = 2;
	const float editor_r = 0.04f;
	const float dragMaxdistance = 2.0f;
	const ui32 dist_precision = 30;
	static const int special_flag = 1;


	NavigatePoint::NavigatePoint(const String& name)
		: m_prepared(false)
		, m_bFinding(false)
		, m_bForward(true)
		, m_isDraging(false)
		, m_bVisibleText(true)
		, m_Name(name)
		, m_Spline(NULL)
		, m_editor_line(NULL)
		, m_pActorObject(NULL)
		, m_editor_lineColor(Color::YELLOW)
		, m_beginIndex(0)
		, m_endIndex(-1)
		, m_time(0)
		, m_findTime(1000)
		, m_totaltime(0)
		, m_MoveSpeed(1.0f)
		, m_ActorScale(1.0f)
		, m_distance(5.0f)
		, m_height(10.0f)
		, m_DragSpeed(0.5f)
		, m_CameraMoveSpeed(1.0f)
		, m_CameraMoveTime(0)
		, m_MinHeight(5.0f)
		, m_MaxHeight(20.0f)
		, m_ExtremeValue(0.0f, 256.0f, 0.0f, 256.0f)
		, m_DragStartPos(Vector2::INVALID)
		, m_DragPositon(Vector2::INVALID)
		, m_DragStartCameraPos(Vector3::INVALID)
		, m_LastDragTargetPos(Vector3::INVALID)
	{
		m_CameraSpline = LordNew SimpleSpline;
		m_Spline = LordNew SimpleSpline;
	}

	NavigatePoint::~NavigatePoint()
	{
		unprepare();
		LordSafeDelete(m_Spline);
		LordSafeDelete(m_CameraSpline);
	}

	void NavigatePoint::exportData(rapidxml::xml_document<>& doc)
	{
		rapidxml::xml_node<>* PointRoot = doc.allocate_node(rapidxml::node_element, "PointRoot", NULL);
		doc.append_node(PointRoot);
		for (PointObjectlist::iterator it = m_editor_PointList.begin(); it != m_editor_PointList.end(); ++it)
		{
			Vector3 pos = (*it)->getParent()->getLocalPosition();
			char* strPos = doc.allocate_string(StringUtil::ToString(pos).c_str());
			rapidxml::xml_node<>* navigatePoint = doc.allocate_node(rapidxml::node_element, "NavigatePoint", NULL);
			navigatePoint->append_attribute(doc.allocate_attribute("pos", strPos));
			PointRoot->append_node(navigatePoint);
		}
		
		rapidxml::xml_node<>* MarkRoot = doc.allocate_node(rapidxml::node_element, "MarkPointRoot", NULL);
		doc.append_node(MarkRoot);
		for (SpecialPointArray::iterator it = mSpecPointArray.begin(); it != mSpecPointArray.end(); ++it)
		{
			int index = *it;
			char* strIndex = doc.allocate_string(StringUtil::ToString(index).c_str());
			rapidxml::xml_node<>* markindex = doc.allocate_node(rapidxml::node_element, "SpecialPoint", NULL);
			markindex->append_attribute(doc.allocate_attribute("index", strIndex));
			MarkRoot->append_node(markindex);
		}

		rapidxml::xml_node<>* CommonRoot = doc.allocate_node(rapidxml::node_element, "CommonRoot", NULL);
		doc.append_node(CommonRoot);
		char* strSpeed = doc.allocate_string(StringUtil::ToString(m_MoveSpeed).c_str());
		char* strScale = doc.allocate_string(StringUtil::ToString(m_ActorScale).c_str());
		char* strDistance = doc.allocate_string(StringUtil::ToString(m_distance).c_str());
		char* strHeight = doc.allocate_string(StringUtil::ToString(m_height).c_str());
		char* strExtreme = doc.allocate_string(StringUtil::ToString(m_ExtremeValue).c_str());
		char* strDragSpeed = doc.allocate_string(StringUtil::ToString(m_DragSpeed).c_str());
		char* strCameraSpeed = doc.allocate_string(StringUtil::ToString(m_CameraMoveSpeed).c_str());
		char* strMaxHeight = doc.allocate_string(StringUtil::ToString(m_MaxHeight).c_str());
		char* strMinHeight = doc.allocate_string(StringUtil::ToString(m_MinHeight).c_str());
		CommonRoot->append_attribute(doc.allocate_attribute("MoveSpeed", strSpeed));
		CommonRoot->append_attribute(doc.allocate_attribute("ActorScale", strScale));
		CommonRoot->append_attribute(doc.allocate_attribute("Distance", strDistance));
		CommonRoot->append_attribute(doc.allocate_attribute("Height", strHeight));
		CommonRoot->append_attribute(doc.allocate_attribute("Extreme", strExtreme));
		CommonRoot->append_attribute(doc.allocate_attribute("DragSpeed", strDragSpeed));
		CommonRoot->append_attribute(doc.allocate_attribute("CameraSpeed", strCameraSpeed));
		CommonRoot->append_attribute(doc.allocate_attribute("MaxHeight", strMaxHeight));
		CommonRoot->append_attribute(doc.allocate_attribute("MinHeight", strMinHeight));
	}

	void NavigatePoint::importData(rapidxml::xml_node<>* parentNode)
	{
		if (!parentNode)
		{
			LordLogError("the Navigate Point file content is valid.It does not contain point root node!");
			return;
		}

		rapidxml::xml_node<> *pPointNode = parentNode->first_node();
		while (pPointNode)
		{
			rapidxml::xml_attribute<> *attribute = pPointNode->first_attribute();
			while (attribute)
			{
				String strnode = attribute->name();
				char* cvalue = attribute->value();
				if (strnode == "pos")
				{
					Vector3 pos = StringUtil::ParseVec3(cvalue);
					insertPoint(pos);
				}

				attribute = attribute->next_attribute();
			}
			pPointNode = pPointNode->next_sibling();
		}

		rapidxml::xml_node<> * MarkNode = parentNode->next_sibling();
		if (MarkNode)
		{
			rapidxml::xml_node<>* pNode = MarkNode->first_node();
			while (pNode)
			{
				rapidxml::xml_attribute<>* valueNode = pNode->first_attribute(); LordAssert(valueNode);
				while (valueNode)
				{
					String strnode = valueNode->name();
					char* cvalue = valueNode->value();
					if (strnode == "index")
					{
						int index = StringUtil::ParseI32(cvalue);
						mSpecPointArray.push_back(index);
					}
					valueNode = valueNode->next_attribute();
				}
				pNode = pNode->next_sibling();
			}
			if (!mSpecPointArray.empty())
			{
				mSpecPointArray.sort();
			}
		}

		rapidxml::xml_node<> *CommonNode = MarkNode->next_sibling();
		if (CommonNode)
		{
			rapidxml::xml_attribute<>* valueNode = CommonNode->first_attribute();
			while (valueNode)
			{
				String strName = valueNode->name();
				char* cvalue = valueNode->value();
				if (strName == "MoveSpeed")
				{
					m_MoveSpeed = StringUtil::ParseFloat(cvalue);
				}
				else if (strName == "ActorScale")
				{
					m_ActorScale = StringUtil::ParseFloat(cvalue);
				}
				else if (strName == "Distance")
				{
					m_distance = StringUtil::ParseFloat(cvalue);
				}
				else if (strName == "Height")
				{
					m_height = StringUtil::ParseFloat(cvalue);
				}
				else if (strName == "Extreme")
				{
					m_ExtremeValue = StringUtil::ParseVec4(cvalue);
				}
				else if (strName == "DragSpeed")
				{
					m_DragSpeed = StringUtil::ParseFloat(cvalue);
				}
				else if (strName == "CameraSpeed")
				{
					m_CameraMoveSpeed = StringUtil::ParseFloat(cvalue);
				}
				else if (strName == "MaxHeight")
				{
					m_MaxHeight = StringUtil::ParseFloat(cvalue);
				}
				else if (strName == "MinHeight")
				{
					m_MinHeight = StringUtil::ParseFloat(cvalue);
				}
				valueNode = valueNode->next_attribute();
			}
		}
	}

	void NavigatePoint::prepare()
	{
		if (m_Spline->getNumPoints() > 0)
		{
			m_DragStartCameraPos = m_Spline->getPoint(0);
			m_LastDragTargetPos = m_Spline->getPoint(0);
		}
	}

	void NavigatePoint::prepare_editor()
	{
		if (!m_prepared)
		{
			SceneNode* pNode = SceneManager::Instance()->getRootNode()->createChild();
			m_editor_line = QueryObjectManager::Instance()->createQueryObject(m_Name + "_navigateline");
			m_editor_line->attachTo(pNode);
			m_editor_line->setTestDepth(true);
			m_editor_line->setWriteDepth(false);
			m_editor_line->setQueryFlag(0);

			SpecialPointArray::iterator it = mSpecPointArray.begin();
			for (; it != mSpecPointArray.end(); ++it)
			{
				int index = *it;
				PointObjectlist::iterator itor = m_editor_PointList.begin();
				std::advance(itor, index);
				QueryObject* pObject = *itor;
				pObject->setUserData((void*)special_flag);
				pObject->createSphere(editor_r*2.0f, 10, 10, Color::YELLOW);
			}

			refleshNavigateLine();
			m_prepared = true;
		}
	}

	void NavigatePoint::unprepare()
	{
		QueryObjectManager* pManager = QueryObjectManager::Instance();
		PointObjectlist::iterator it = m_editor_PointList.begin();
		for (; it != m_editor_PointList.end(); ++it)
		{
			QueryObject* pObject = *it;
			SceneNode* node = pObject->getParent();
			node->getParent()->destroyChild(node);
			pManager->destroyQueryObject(pObject);
			pObject = NULL;
		}
		m_editor_PointList.clear();

		if (m_editor_line)
		{
			SceneNode* linenode = m_editor_line->getParent();
			linenode->getParent()->destroyChild(linenode);
			pManager->destroyQueryObject(m_editor_line);
			m_editor_line = NULL;
		}
		CameraMain* pCameraMain = (CameraMain*)SceneManager::Instance()->getMainCamera();
		pCameraMain->setFixedYawAxis(true);
		pCameraMain->setCameraState(CameraMain::NORMAL);
	}

	QueryObject* NavigatePoint::insertPoint(const Vector3& pos)
	{
		if (Root::Instance()->isEditorMode())
		{
			SceneNode* pNode = SceneManager::Instance()->getRootNode()->createChild();
			QueryObject* newpoint = QueryObjectManager::Instance()->createQueryObject(m_Name + "_navigatepoint_" + GetNewPointName());
			newpoint->attachTo(pNode);
			newpoint->setTestDepth(true);
			newpoint->setWriteDepth(false);
			newpoint->setQueryFlag(NAVIGATE_POINT_QUERY_FLAG);
			newpoint->setUserData(NULL);
			newpoint->createSphere(editor_r, 10, 10, Color::GREEN);
			pNode->setLocalPosition(pos);
			m_editor_PointList.push_back(newpoint);
			if (m_prepared)
				refleshNavigateLine();
			return newpoint;
		}
		else
		{
			m_Spline->addPoint(pos);
		}
		return NULL;
	}

	void NavigatePoint::deletePoint(QueryObject* pointobject)
	{
		PointObjectlist::iterator it = std::find(m_editor_PointList.begin(), m_editor_PointList.end(), pointobject);
		if (it != m_editor_PointList.end())
		{
			QueryObject* pObject = *it;
			SceneNode* node = pObject->getParent();
			node->getParent()->destroyChild(node);
			QueryObjectManager::Instance()->destroyQueryObject(pObject);
			pObject = NULL;
			m_editor_PointList.erase(it);
			refleshNavigateLine();
			refleshSpecialPoint();
		}
	}

	void NavigatePoint::refleshNavigateLine()
	{
		m_Spline->clear();
		PointObjectlist::iterator it = m_editor_PointList.begin();
		for (; it != m_editor_PointList.end(); ++it)
		{
			m_Spline->addPoint((*it)->getParent()->getLocalPosition());
		}
		int pointnum = m_editor_PointList.size();
		if (pointnum > 1)
		{
			Scene* pScene = SceneManager::Instance()->getCurrentScene();
			ui16 xcount, ycount;
			pScene->getBlockCount(xcount, ycount);

			Vector3 pos;
			float radius = pScene->getSphereRadius();
			float scenewidht = xcount * pScene->getWidthPerBlock();
			float sceneheight = ycount * pScene->getHeightPerBlock();
			Vector3 center(scenewidht*0.5f, -radius, sceneheight*0.5f);
			m_editor_line->clear();
			m_editor_line->beginPolygon(QueryObject::QO_WIREFRAME);
			int num = dist_precision * pointnum;
			for (int i = 0; i < num; ++i)
			{
				//法线方向偏移editor_r高度，避免z-fighting
				Vector3 pos = m_Spline->interpolate((float)i / (float)num);
				if (pScene->isSphereTerrain())
				{
					Vector3 dir = (pos - center).normalizedCopy();
					pos += dir * editor_r;
				}
				else
				{
					pos += Vector3::UNIT_Y * 0.1f;
				}
				m_editor_line->setPosition(pos);
				m_editor_line->setColor(m_editor_lineColor);
			}

			for (int i = 0; i < num - 1; ++i)
			{
				m_editor_line->setIndex(i);
				m_editor_line->setIndex(i + 1);
			}
			m_editor_line->endPolygon();
		}
	}

	void NavigatePoint::refleshSpecialPoint()
	{
		int index = 0;
		mSpecPointArray.clear();
		PointObjectlist::iterator it = m_editor_PointList.begin();
		for (; it != m_editor_PointList.end(); ++it)
		{
			QueryObject* pObject = *it;
			if (pObject->getUserData())
			{
				mSpecPointArray.push_back(index);
			}
			index++;
		}
	}

	void NavigatePoint::setSpecialPoint(QueryObject* pointobject)
	{
		PointObjectlist::iterator it = std::find(m_editor_PointList.begin(), m_editor_PointList.end(), pointobject);
		if (it != m_editor_PointList.end())
		{
			QueryObject* pObject = *it;
			if (pObject->getUserData())
			{
				pObject->setUserData(NULL);
				pObject->createSphere(editor_r, 10, 10, Color::GREEN);
			}
			else
			{
				pObject->setUserData((void*)&special_flag);
				pObject->createSphere(editor_r*2.0f, 10, 10, Color::YELLOW);
			}

			refleshSpecialPoint();
		}
	}

	void NavigatePoint::startFindPath(int dst, bool fly)
	{
		if (m_pActorObject && !m_bFinding)
		{
			if (fly)
			{
				setBeginPoint(dst);
			}
			else
			{
				SpecialPointArray::iterator it = std::find(mSpecPointArray.begin(), mSpecPointArray.end(), dst);
				if (it != mSpecPointArray.end())
				{
					m_endIndex = *it;
					if (m_beginIndex != m_endIndex)
					{
						m_bFinding = true;
						m_bForward = m_beginIndex < m_endIndex;
						m_totaltime = calcmoveTime();
						m_time = m_bForward ? 0 : m_totaltime;
						m_CameraMoveTime = 0;
						m_pActorObject->PlaySkill("run");
					}
				}
			}
		}
	}

	void NavigatePoint::editor_startFindPath(QueryObject* pointobject)
	{
		PointObjectlist::iterator it = std::find(m_editor_PointList.begin(), m_editor_PointList.end(), pointobject);
		if (it != m_editor_PointList.end())
		{
			int dst = std::distance(m_editor_PointList.begin(), it);
			startFindPath(dst);
		}
	}

	LORD::ui32 NavigatePoint::calcmoveTime()
	{
		LORD::ui32 result;
		float dist = 0;
		if (m_bForward)
		{
			Vector3 beginpos = m_Spline->getPoint(m_beginIndex);
			for (int i = 1; i < dist_precision; ++i)
			{
				Vector3 pos = m_Spline->interpolate(m_beginIndex, (float)i / (float)dist_precision);
				dist += (beginpos - pos).len();
				beginpos = pos;
			}
		}
		else
		{
			Vector3 beginpos = m_Spline->getPoint(m_beginIndex-1);
			for (int i = 1; i < dist_precision; ++i)
			{
				Vector3 pos = m_Spline->interpolate(m_beginIndex - 1, (float)i / (float)dist_precision);
				dist += (beginpos - pos).len();
				beginpos = pos;
			}
		}

		result = (ui32)((dist / (perSecondSpeed)) * (1000.0f / m_MoveSpeed));
		return result;
	}

	void NavigatePoint::setMainActor(ActorObject* pObject, int beginIndex)
	{
		m_pActorObject = pObject;

		CameraMain* pCameraMain = (CameraMain*)SceneManager::Instance()->getMainCamera();
		if (m_pActorObject)
		{
			setBeginPoint(beginIndex);
			m_pActorObject->SetShadowVisible(false);
			m_pActorObject->SetScale(Vector3(m_ActorScale, m_ActorScale, m_ActorScale));
			pCameraMain->setFixedYawAxis(false);
			pCameraMain->setCameraState(CameraMain::LOCK);
		}
		else
		{
			pCameraMain->setFixedYawAxis(true);
			m_isDraging = false;
		}
	}

	bool NavigatePoint::update(ui32 frametime)
	{
		if (m_pActorObject)
		{
			if (m_bFinding)
			{
				if (m_bForward)
				{
					m_time += frametime;
					Vector3 targetPos = m_Spline->interpolate(m_beginIndex, Math::Min((float)m_time / (float)m_totaltime, 1.0f));
					Vector3 lastPos = m_pActorObject->GetPosition();
					Vector3 movedir = (targetPos - lastPos).normalizedCopy();
					if (!movedir.isZeroLength())
					{
						Vector3 center = SceneManager::Instance()->getCurrentScene()->getSphereTerrainCenter();
						Vector3 up = (targetPos - center).normalizedCopy();
						setActorQrientation(m_pActorObject, movedir, up);
					}
					setActorPosition(targetPos);
					if (m_time >= (int)m_totaltime)
					{
						if (++m_beginIndex == m_endIndex)
						{
							endFindPath();
						}
						else
						{
							m_time -= m_totaltime;
							m_totaltime = calcmoveTime();
						}
					}
				}
				else
				{
					m_time -= frametime;
					Vector3 targetPos = m_Spline->interpolate(m_beginIndex - 1, Math::Max((float)m_time / (float)m_totaltime, 0.0f));
					Vector3 lastPos = m_pActorObject->GetPosition();
					Vector3 movedir = (targetPos - lastPos).normalizedCopy();
					if (!movedir.isZeroLength())
					{
						Vector3 center = SceneManager::Instance()->getCurrentScene()->getSphereTerrainCenter();
						Vector3 up = (targetPos - center).normalizedCopy();
						setActorQrientation(m_pActorObject, movedir, up);
					}
					setActorPosition(targetPos);
					if (m_time <= 0)
					{
						if (--m_beginIndex == m_endIndex)
						{
							endFindPath();
						}
						else
						{
							m_totaltime = calcmoveTime();
							m_time += m_totaltime;
						}
					}
				}
				calcCameralogic(frametime, true);
			}
			else
			{
				dragCamera(frametime);
			}
		}
		return !m_bFinding;
	}

	void NavigatePoint::renderSpecialPointText()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		if (m_bVisibleText)
		{
			Renderer* pRenderer = Renderer::Instance();
			for (SpecialPointArray::iterator it = mSpecPointArray.begin(); it != mSpecPointArray.end(); ++it)
			{
				int id = *it;
				Vector3 pos = m_Spline->getPoint(id) + Vector3::UNIT_Y*1.0f;
				pos = pRenderer->project(pos);
				String text = StringUtil::ToString(id);
				char* buffer = (char*)LordMalloc(MAX_PATH);
				uint len;
				StringUtil::GBKToUTF8(buffer, len, text.c_str(), text.length());
				GUIString strText = buffer;
				if (pos.x > 0 && pos.x < Root::Instance()->getRealWidth() && pos.y>0 && pos.y < Root::Instance()->getRealHeight())
				{
					// need specify the font.
					// pRenderer->renderText(strText, Vector2(pos.x, pos.y), "");
				}
				LordFree(buffer);
			}
		}
#endif
	}

	void NavigatePoint::endFindPath()
	{
		if (m_bFinding)
		{
			m_bFinding = false;
			m_beginIndex = m_endIndex;
			Vector3 targetPos = m_Spline->getPoint(m_endIndex);
			Vector3 center = SceneManager::Instance()->getCurrentScene()->getSphereTerrainCenter();
			Vector3 up = (targetPos - center).normalizedCopy();
			Vector3 dir = up.cross(Vector3::UNIT_X);
			setActorQrientation(m_pActorObject, -dir, up);
			setActorPosition(targetPos);
			m_pActorObject->PlaySkill("idle", true);
			m_DragStartCameraPos = targetPos;
		}
	}

	void NavigatePoint::setMainActorScale(float scale)
	{
		m_ActorScale = scale;
		if (m_pActorObject)
		{
			m_pActorObject->SetScale(Vector3(scale, scale, scale));
		}
	}

	void NavigatePoint::setMoveSpeed(float fspeed)
	{
		m_MoveSpeed = fspeed;
		if (m_pActorObject)
		{
			m_pActorObject->SetTimeScale(fspeed);
		}
	}

	void NavigatePoint::setBeginPoint(int beginIndex)
	{
		if (m_pActorObject)
		{
			SpecialPointArray::iterator it = std::find(mSpecPointArray.begin(), mSpecPointArray.end(), beginIndex);
			if (it != mSpecPointArray.end())
			{
				m_beginIndex = *it;
				Vector3 targetPos = m_Spline->getPoint(m_beginIndex);
				Vector3 center = SceneManager::Instance()->getCurrentScene()->getSphereTerrainCenter();
				Vector3 up = (targetPos - center).normalizedCopy();
				Vector3 dir = up.cross(Vector3::UNIT_X);
				setActorQrientation(m_pActorObject, -dir, up);
				setActorPosition(targetPos);
				m_pActorObject->PlaySkill("idle");
				m_DragStartCameraPos = targetPos;
			}
			calcCameralogic();
		}
	}

	void NavigatePoint::setActorQrientation(ActorObject* pObject, const Vector3& dir, const Vector3& up)
	{
		Vector3 xVec = up.cross(dir);
		xVec.normalize();
		Vector3 yVec = dir.cross(xVec);
		yVec.normalize();
		Quaternion q;
		q.fromAxes(xVec, yVec, dir);
		pObject->SetOrientation(q);
	}

	void NavigatePoint::setActorPosition(const Vector3& pos)
	{
		m_pActorObject->SetPosition(pos);
		SceneManager* pManager = SceneManager::Instance();
		pManager->setDeltaPosition(pos - pManager->getMainCamera()->getPosition());
	}

	void NavigatePoint::calcCameralogic(ui32 frameTime, bool delaymove)
	{
		LordAssert(m_pActorObject);
		SceneManager* pSceneManager = SceneManager::Instance();
		CameraMain* pCamera = (CameraMain*)pSceneManager->getMainCamera();
		float radius = pSceneManager->getCurrentScene()->getSphereRadius();
		Vector3 center = pSceneManager->getCurrentScene()->getSphereTerrainCenter();
		Vector3 actorpos = m_pActorObject->GetPosition();

		extremePosition(actorpos);
		actorpos = center + (actorpos - center).normalizedCopy()*radius;

		Vector3 camerapos = actorpos + Vector3::UNIT_Z*m_distance;
		Vector3 dir = (camerapos - center).normalizedCopy();
		camerapos = center + dir*(radius + m_height);

		if (delaymove)
		{
			Vector3 curpos = pCamera->getPosition();
			m_CameraSpline->clear();
			m_CameraSpline->addPoint(curpos);
			m_CameraSpline->addPoint(camerapos);

			m_CameraMoveTime += frameTime;
			float radio = Math::Min((float)m_CameraMoveTime * m_CameraMoveSpeed / 1000.0f, 1.0f);
			pCamera->setPosition_forLord(m_CameraSpline->interpolate(radio));
			pCamera->setDirection_forLord((actorpos - camerapos).normalizedCopy());
			pCamera->setUp_forLord(dir);
			m_DragStartCameraPos = actorpos;
		}
		else
		{
			pCamera->setPosition_forLord(camerapos);
			pCamera->setDirection_forLord((actorpos - camerapos).normalizedCopy());
			pCamera->setUp_forLord(dir);
			m_DragStartCameraPos = actorpos;
		}
	}

	void NavigatePoint::dragCamera(ui32 frameTime)
	{
		if (m_isDraging)
		{
			SceneManager* pSceneManager = SceneManager::Instance();
			CameraMain* pCamera = (CameraMain*)pSceneManager->getMainCamera();
			float radius = pSceneManager->getCurrentScene()->getSphereRadius();
			Vector3 center = pSceneManager->getCurrentScene()->getSphereTerrainCenter();
			Vector3 right = Vector3::UNIT_X;
			Vector3 up = Vector3::NEG_UNIT_Z;

			Vector2 dragDelta = m_DragPositon - m_DragStartPos;
			if (dragDelta.lenSqr() > 4.f)
			{
				m_LastDragTargetPos = m_DragStartCameraPos;			
				m_LastDragTargetPos = m_LastDragTargetPos - right*dragDelta.x*0.03f*m_DragSpeed + up*dragDelta.y*0.03f*m_DragSpeed;

				extremePosition(m_LastDragTargetPos);

				Vector3 dir = (m_LastDragTargetPos - center).normalizedCopy();
				m_LastDragTargetPos = center + dir*radius;

				Vector3 camerapos = m_LastDragTargetPos + Vector3::UNIT_Z*m_distance;
				dir = (camerapos - center).normalizedCopy();
				camerapos = center + dir*(radius + m_height);
				pCamera->setPosition_forLord(camerapos);
				pCamera->setDirection_forLord((m_LastDragTargetPos - camerapos).normalizedCopy());
				pCamera->setUp_forLord(dir);
				SceneManager::Instance()->setDeltaPosition(m_LastDragTargetPos - camerapos);
			}
		}
	}

	void NavigatePoint::touchdown(const Vector2& point)
	{
		if (!m_bFinding && m_pActorObject)
		{
			m_isDraging = true;
			m_DragStartPos = point;
			m_DragPositon = point;
			m_LastDragTargetPos = m_DragStartCameraPos;
		}
	}

	void NavigatePoint::touchmove(const Vector2& point)
	{
		if (!m_bFinding)
		{
			if (m_isDraging)
			{
				m_DragPositon = point;
			}
		}
	}

	void NavigatePoint::touchup()
	{
		if (m_isDraging)
		{
			m_isDraging = false;
			m_DragStartCameraPos = m_LastDragTargetPos;
		}
	}

	int NavigatePoint::getSpecialPointNum()
	{
		return mSpecPointArray.size();
	}

	void NavigatePoint::setPointActor(ActorObject* pObject, int pointindex)
	{
		if (pObject)
		{
			SpecialPointArray::iterator itpoint = std::find(mSpecPointArray.begin(), mSpecPointArray.end(), pointindex);
			if (itpoint != mSpecPointArray.end())
			{
				PointActorlist::iterator it = std::find(m_PointActorlist.begin(), m_PointActorlist.end(), pObject);
				if (it == m_PointActorlist.end())
				{
					m_PointActorlist.push_back(pObject);
				}
				pObject->SetUserData(pointindex);

				Vector3 targetPos = m_Spline->getPoint(pointindex);
				Vector3 center = SceneManager::Instance()->getCurrentScene()->getSphereTerrainCenter();
				Vector3 up = (targetPos - center).normalizedCopy();
				Vector3 dir = -(up.cross(Vector3::UNIT_X));
				setActorQrientation(pObject, dir, up);
				pObject->SetPosition(targetPos);
			}
			
		}
	}

	void NavigatePoint::clearPointActor(int pointindex)
	{
		for (PointActorlist::iterator it = m_PointActorlist.begin(); it != m_PointActorlist.end(); ++it)
		{
			ActorObject* pObject = *it;
			if (pObject->getUserData() == pointindex)
			{
				m_PointActorlist.erase(it);
				break;
			}
		}
	}

	void NavigatePoint::clearAllPointActor()
	{
		m_PointActorlist.clear();
	}

	int NavigatePoint::rayIntersectsPoint(const Ray& ray)
	{
		vector<ActorManager::ActorPickInfo>::type actorPickResults;
		for (PointActorlist::iterator it = m_PointActorlist.begin(); it != m_PointActorlist.end(); ++it)
		{
			ActorObject* pObject = *it;
			if (!pObject->getActorIsHide() && !pObject->isActorBackup())
			{
				LORD::HitInfo hitInfo;
				std::pair<bool, LORD::Real> result;
				result.first = false;
				result.second = 0.0f;

				result.first = ray.hitBox((*it)->getActorAABB(), result.second, hitInfo);
				if (result.first)
				{
					ActorManager::ActorPickInfo pickInfo;
					pickInfo.pActor = *it;
					pickInfo.distance = result.second;
					actorPickResults.push_back(pickInfo);
				}
			}
		}

		if (!actorPickResults.empty())
		{
			// 排序
			std::sort(actorPickResults.begin(), actorPickResults.end());
			return actorPickResults[0].pActor->getUserData();
		}

		return -1;
	}

	int NavigatePoint::rayIntersectsPoint(float screenPosX, float screenPosY)
	{
		Ray ray;
		SceneManager::Instance()->getMainCamera()->getCameraRay(ray, Vector2(screenPosX, screenPosY));
		return rayIntersectsPoint(ray);
	}

	void NavigatePoint::pullHeight(float deltaHeight)
	{
		float oldHeight = m_height;
		m_height = Math::Clamp(m_height + deltaHeight, m_MinHeight, m_MaxHeight);
		float delta = m_height - oldHeight;

		CameraMain* pCamera = (CameraMain*)SceneManager::Instance()->getMainCamera();
		Vector3 oldPosition = pCamera->getPosition();
		Vector3 dir = pCamera->getDirection();
		Vector3 newPosition = oldPosition - dir*delta;
		pCamera->setPosition_forLord(newPosition);
	}
}