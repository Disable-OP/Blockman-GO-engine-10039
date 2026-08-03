#ifndef _LORD_NAVIGATE_POINT_H__
#define _LORD_NAVIGATE_POINT_H__

#include "Core.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

class GizomManager;

namespace LORD
{
	class LORD_CORE_API NavigatePoint : public ObjectAlloc
	{
	public:
		typedef list<QueryObject*>::type PointObjectlist;
		typedef list<ActorObject*>::type PointActorlist;
		typedef list<int>::type SpecialPointArray;
		typedef vector<Vector3>::type MovePointArray;
	public:
		NavigatePoint(const String& name);
		~NavigatePoint();
		const String& getName() const { return m_Name; }
		void exportData(rapidxml::xml_document<>& doc);
		void importData(rapidxml::xml_node<>* parentNode);
		void prepare();
		void prepare_editor();
		void unprepare();
		QueryObject* insertPoint(const Vector3& pos);
		void deletePoint(QueryObject* pointobject);
		void refleshNavigateLine();
		void refleshSpecialPoint();
		void setSpecialPoint(QueryObject* pointobject);
		int getSpecialPointNum();
		void setTime(ui32 time) { m_findTime = time; }
		void startFindPath(int dst, bool fly=false);
		void editor_startFindPath(QueryObject* pointobject);
		void endFindPath();
		ui32 calcmoveTime();
		void calcCameralogic(ui32 frameTime=0, bool delaymove = false);
		void setMainActor(ActorObject* pObject, int beginIndex=0);
		void setPointActor(ActorObject* pObject, int pointindex);
		void clearPointActor(int pointindex);
		void clearAllPointActor();
		bool update(ui32 frametime);
		void setMoveSpeed(float fspeed);
		float getMoveSpeed() const { return m_MoveSpeed; }
		void renderSpecialPointText();
		void setMainActorScale(float scale);
		float getMainActorScale() const { return m_ActorScale; }
		void setBeginPoint(int beginIndex);
		void setActorQrientation(ActorObject* pObject, const Vector3& dir, const Vector3& up);
		void setActorPosition(const Vector3& pos);
		void touchdown(const Vector2& point);
		void touchmove(const Vector2& point);
		void touchup();
		void dragCamera(ui32 frametime);
		void setCameraDistance(float distance) { m_distance = distance; }
		float getCameraDistance() const { return m_distance; }
		void setCameraHeight(float height) { m_height = height; }
		float getCameraHeight() const { return m_height; }

		void setLeftExtreme(float val) { m_ExtremeValue.x = val; }
		void setRightExtreme(float val) { m_ExtremeValue.y = val; }
		void setTopExtreme(float val) { m_ExtremeValue.z = val; }
		void setButtomExtreme(float val) { m_ExtremeValue.w = val; }
		float getLeftExtreme() const { return m_ExtremeValue.x; }
		float getRightExtreme() const { return m_ExtremeValue.y; }
		float getTopExtreme() const { return m_ExtremeValue.z; }
		float getButtomExtreme() const { return m_ExtremeValue.w; }

		void setDragSpeed(float speed) { m_DragSpeed = speed; }
		float getDragSpeed() const { return m_DragSpeed; }
		void setCameraSpeed(float speed) { m_CameraMoveSpeed = speed; }
		float getCameraSpeed() const { return m_CameraMoveSpeed; }

		int rayIntersectsPoint(const Ray& ray);
		int rayIntersectsPoint(float screenPosX, float screenPosY);

		void pullHeight(float deltaHeight);
		void setMaxHeight(float height) { m_MaxHeight = height; }
		void setMinHeight(float height) { m_MinHeight = height; }
		float getMaxHeight() const { return m_MaxHeight; }
		float getMinHeight() const { return m_MinHeight; }

		void setVisibleSpecialText(bool bVisible) { m_bVisibleText = bVisible; }
		bool isVisibleSpecialText() const { return m_bVisibleText; }
		
		void extremePosition(Vector3& pos)
		{
			if (pos.x < m_ExtremeValue.x)
			{
				pos.x = m_ExtremeValue.x;
			}
			if (pos.x > m_ExtremeValue.y)
			{
				pos.x = m_ExtremeValue.y;
			}
			if (pos.z < m_ExtremeValue.z)
			{
				pos.z = m_ExtremeValue.z;
			}
			if (pos.z > m_ExtremeValue.w)
			{
				pos.z = m_ExtremeValue.w;
			}
		}
	protected:
		bool m_prepared;
		bool m_bFinding;
		bool m_bForward;
		bool m_isDraging;
		bool m_bVisibleText;

		ui32 m_findTime;
		ui32 m_totaltime;
		ui32 m_CameraMoveTime;
		int m_beginIndex;
		int m_endIndex;
		int m_time;

		float m_MoveSpeed;
		float m_ActorScale;
		float m_distance;
		float m_height;
		float m_DragSpeed;
		float m_CameraMoveSpeed;
		float m_MinHeight;
		float m_MaxHeight;

		SimpleSpline* m_CameraSpline;
		SimpleSpline* m_Spline;
		QueryObject* m_editor_line;
		ActorObject* m_pActorObject;

		Vector2 m_DragStartPos;
		Vector2 m_DragPositon;
		Vector3 m_DragStartCameraPos;
		Vector3 m_LastDragTargetPos;
		Vector4 m_ExtremeValue;
		Color m_editor_lineColor;
	
		String m_Name;
		PointObjectlist m_editor_PointList;
		PointActorlist m_PointActorlist;
		SpecialPointArray mSpecPointArray;
	};
}
#endif
