#ifndef __CAMERA_ANIMTION_H_
#define __CAMERA_ANIMTION_H_

#include "Core.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include "rapidjson/document.h"

namespace LORD
{
	class CameraPathRender;

	class LORD_CORE_API CameraAnimation :	public ObjectAlloc
	{
		friend class Scene;
	public:
		enum ANIM_STATE
		{
			PLAY,
			STOP,
			PAUSE,
		};
		struct PointInfo 
		{
			PointInfo(const Vector3& p, const Quaternion& q, float t)
				: pos(p)
				, qua(q)
				, relateQ(Quaternion::IDENTITY)
				, time(t)
				, staytime(0)
				, timescale(1.0f)
				, pause(false)
			{
			}
			~PointInfo(){}
			Vector3 pos;
			Quaternion qua;
			Quaternion relateQ;
			float len;
			float time;
			float staytime;
			float timescale;
			bool pause;
		};
	public:
		CameraAnimation(const String& name);
		~CameraAnimation(void);

		void exportData(DataStream& dataStream);
		void importData(DataStream& dataStream);
		void exportData(rapidxml::xml_document<>& doc);
		void importData(rapidxml::xml_node<>* parentNode);
		void exportData(rapidjson::Document* doc);
		void importData(const rapidjson::Value& node);

		//bool _prepare_io();
		bool _prepare_obj();
		bool _prepare_editor();
		void _unprepare();

		std::pair<CameraAnimation*, i32> rayInstersect(const Ray& ray);

		void createEditorHelp(QueryObject* pObj);
		void addMainPoint(const Vector3& point, const Quaternion& q, float time);
		void addMainPoint(const PointInfo& info);
		void insertPoint(QueryObject* pObject, const Vector3& point, const Quaternion& q);
		void deleteMainPoint(QueryObject* pObject);
		void refleshPath();
		void setColor(const Color& color);
		void play();
		void stop();
		void pause();
		inline bool isStop() { return m_State == STOP; }
		inline const String& getName() const { return m_Name; }

		QueryObject* editor_getPointObject(i32 index);
		QueryObject* editor_getPointObject(const String& name);
		void editor_setPosition(QueryObject* pObject, const Vector3& pos);
		void editor_setOritation(QueryObject* pObject, const Quaternion& q);
		inline const list<QueryObject*>::type& editor_getPointObjects() const { return m_PointObject; }

		float editor_getPointTime(i32 index);
		void editor_setPointTime(i32 index, float time);

		void update(ui32 frameTime);
		void updatePath(float frameTime);
		void updateFadeLight(float frameTime);

		void setTargetActor(ActorObject* pTarget);
		/// -------------------------------------for editor---------------------------------------------
		typedef vector<std::pair<String, bool> >::type PropertyList;
		virtual void getPropertyList(PropertyList & lst);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value, QueryObject* pObj);
		virtual bool setPropertyValue(const String & name,const String & value, QueryObject* pObj);

		float getPauseLength();

		float getTotalTime();

		Quaternion CaculateOritation();
		void CaculateAngle(const Quaternion& q);

		void SetVisibleActor(const String& actorNameArray) { m_ActorArray = actorNameArray; }
	protected:
		bool  mIsFocus;
		bool  mIsStopFade;
		bool  mIsStay;
		bool  mIsChangeDark;
		ui32  mFadeState;
		ui32  m_State;
		ui32  mPointNum;
		int	  mAccIndex;
		float mCurrentTime;
		float mStayCurrentTime;
		float mStayTime;
		float m_Pitch;
		float m_Yaw;
		float m_Roll;
		float m_DarkTime;
		float m_Darkness;
		float m_LightTime;
		float m_Lightness;
		float m_CurDarkTime;
		float m_CurLightTime;
		float m_PassTime;
		float m_TotalTime;
		float m_PlayFadeTime;
		float m_StopFadeTime;
		float m_FadeSpeed;
		float m_OldSpeedUp;

		ActorObject* m_pActor;
		CameraPathRender* m_pPathRender;
		SimpleSpline* m_pSpline;
		CameraMain* m_pCamera;

		Vector3 m_FadeDirection;
		Vector3 m_LastPosition;
		Vector3 m_LastDirection;
		Vector3 m_FadePosition;
		Quaternion m_FadeOriternion;
		Quaternion m_LastOriternion;
		Quaternion m_ActorOriternion;

		String m_Name;
		String m_ActorArray;
		list<PointInfo>::type mMainPointList;
		list<PointInfo>::type::iterator mCurrentIt;
		list<QueryObject*>::type m_PointObject;
	};

	class CameraPathRender : public ObjectAlloc
	{
		friend class CameraAnimation;
	public:
		CameraPathRender(const String& name);
		~CameraPathRender();

		void refleshGpuBuffer();
		void importPointVector(SimpleSpline* spline);

	protected:
		String mName;
		QueryObjectManager* m_pQueryManager;
		QueryObject* m_pLineObject;
		Color m_Color;
		vector<Vector3>::type mPointVector;
	};
}

#endif
