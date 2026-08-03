/********************************************************************
filename: 	GameObject.h
file path:	dev\engine\Src\Core\Scene\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Core.h"
#include "Geom/Box.h"
#include "Resource/LordThread.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#include "rapidjson/document.h"

namespace LORD
{

	enum
	{
		// object
		TOKEN_OBJ_NAME,
		TOKEN_OBJ_POSITION,
		TOKEN_OBJ_ORITATION,
		TOKEN_OBJ_SCALE,
		TOKEN_OBJ_MIRROR,
		TOKEN_OBJ_PITCH,
		TOKEN_OBJ_YAW,
		TOKEN_OBJ_ROLL,
		TOKEN_OBJ_PIROR,
		TOKEN_OBJ_RESOURCE,
		TOKEN_OBJ_NOTE,

		// smesh
		TOKEN_SM_SUBMESHNUM,
		TOKEN_SM_RENDERGLOW,
		TOKEN_SM_ISCANWALK,
		TOKEN_SM_ISRENDER,
		TOKEN_SM_ISCOLLISION,
		TOKEN_SM_ISCACULATEFOG,
		TOKEN_SM_UVSCALE,
		TOKEN_SM_LMSIZE,
		TOKEN_SM_VISIBLE,
		TOKEN_SM_ALPHATEST,
		TOKEN_SM_ISGRAY,

		// dmesh
		TOKEN_DM_COLOR,
		TOKEN_DM_BRIGHTEN,
		TOKEN_DM_SKELETON,
		TOKEN_DM_ANIMATION,

		// Effect
		TOKEN_EF_COLOR,
		TOKEN_EF_TIME,

		// logic object
		TOKEN_LO_TYPE,
		TOKEN_LO_ID,
		TOKEN_LO_COUNT,
		TOKEN_LO_RADIUS,

		// Light
		TOKEN_LT_COLOR,
		TOKEN_LT_RADIUS,
		TOKEN_LT_BRIGHTNESS,
		TOKEN_LT_FALLOFF,
		TOKEN_LT_INDIRSCALE,
		TOKEN_LT_INDIRSATURATE,
		TOKEN_LT_SHADOWEXP,
		TOKEN_LT_CASTSHADOWS,
		TOKEN_LT_INNERANGLE,
		TOKEN_LT_GRADIENTANGLE,

		// Camera Animation
		TOKEN_CA_TIME,
		TOKEN_CA_STAYTIME,
		TOKEN_CA_TIMESCALE,
		TOKEN_CA_PAUSESKILL,
		TOKEN_CA_DARKNESS,
		TOKEN_CA_DARKTIME,
		TOKEN_CA_LIGHTNESS,
		TOKEN_CA_LIGHTTIME,
		TOKEN_CA_ISFOCUS,
		TOKEN_CA_PLAYFADETIME,
		TOKEN_CA_STOPFADETIME,
		TOKEN_CA_ISSTAY,
		TOKEN_CA_ISDARK,

		TOKEN_HZ_ALPHA,

		TOKEN_CA_MAX
	};

	const static String objToken[TOKEN_CA_MAX] =
	{
		// object
		"obj_name",
		"obj_position",
		"obj_oritation",
		"obj_scale",
		"obj_mirror",
		"obj_pitch",
		"obj_yaw",
		"obj_roll",
		"obj_piror",
		"obj_resource",
		"obj_note",

		// smesh
		"sm_submeshnum",
		"sm_renderglow",
		"sm_iscanwalk",
		"sm_isrender",
		"sm_iscollision",
		"sm_iscaculatefog",
		"sm_uvscale",
		"sm_lmsize",
		"sm_visible",
		"sm_alphatest",
		"sm_isgray",

		// dmesh
		"dm_color",
		"dm_brighten",
		"dm_skeleton",
		"dm_animation",

		// effect
		"ef_color",
		"ef_time",

		// logic object
		"lo_type",
		"lo_id",
		"lo_count",
		"lo_radius",

		// light
		"lt_color",
		"lt_radius",
		"lt_brightness",
		"lt_falloff",
		"lt_indirscale",
		"lt_indirsaturate",
		"lt_shadowexp",
		"lt_castshadow",
		"lt_innerangle",
		"lt_gradientangle",

		// Camera Animation
		"ca_time",
		"ca_staytime",
		"ca_timescale",
		"ca_pauseskill",
		"ca_darkness",
		"ca_darktime",
		"ca_lightness",
		"ca_lighttime",
		"ca_isfocus",
		"ca_playfadetime",
		"ca_stopfadetime",
		"ca_isstay",
		"ca_isdark",

		"hz_alpha"
	};


	class SceneNode;
	class Mesh;
	class Zoom;
	class QueryObject;

	class LORD_CORE_API GameObject : public ObjectAlloc
	{
		friend class Scene;
		friend class Zoom;
		friend class ObjectPrepareEvent;

	public:
		/// foreditor
		enum MirrorType
		{
			MIR_XY = 0,
			MIR_XZ,
			MIR_YZ,
			MIR_NONE,
		};

	public:
		typedef map<String, String>::type PropertyValueList;
		typedef vector<std::pair<String, bool> >::type PropertyList;

		struct PickInfo
		{
			std::pair<bool, Real> info;
			bool operator < (const PickInfo& rhs) const
			{
				return this->info.second < rhs.info.second;
			}
		};

	protected:
		GameObject();
		virtual ~GameObject();

	public:
		/// 多线程加载 模型，贴图资源等
		virtual void _prepare_io() = 0;
		/// 加载 当前只针对编辑器做单线程
		virtual void _prepare_sys();
		/// 发起io请求之前，创建资源空对象
		virtual void _prepare_res() { }
		/// io线程完成后的工作,创建d3d对象,挂接到场景节点的过程
		virtual void _prepare_obj() = 0;
		/// 编辑器需要创建额外的资源 
		virtual void _prepare_editor() = 0;
		/// 从场景节点中卸载并且销毁模型，贴图资源
		virtual void _unprepare() = 0;

		// 序列化的接口之后再做
		// todo 最好用xml，便于版本扩展
		///// 导出数据 用于游戏
		virtual bool exportData(rapidxml::xml_node<>* parentNode, rapidxml::xml_document<>& doc);
		///// 导入数据 用于游戏
		virtual bool importData(rapidxml::xml_node<>* parentNode);

		//add by maxicheng
		virtual bool exportData(rapidjson::Value& node, rapidjson::Document* doc);
		virtual bool importData(rapidjson::Value& node);

		// 获得包围盒 用于注册场景节点，及查询与剔除
		virtual const Box& getWorldAABB() const = 0;
		//virtual const Mat4& getFullTransform() const = 0;

		virtual std::pair<bool, Real> rayIntersects(const Ray& ray);
		bool	frustumIntersects() { return m_bFrustumCull ? false : true; }

		void	setOwnerZoom(Zoom* zoom) { m_pOwnerZoom = zoom; }
		Zoom*	getOwnerZoom() { return m_pOwnerZoom; }

		// 是否需要保存
		bool isNeedSave() const { return m_isNeedSave; }

	public:
		// 渲染到场景
		// 渲染这个对象的接口 之后实现
		void frameMove(i32 deltaTime);
		void _render() {}

		inline ui32 getType() const { return m_Type; }
		inline void setName(const String name) { m_Name = name; }
		inline const String& getName() const { return m_Name; }
		inline void setUserData(void* userdata) { m_UserData = userdata; }
		inline void* getUserData() const { return m_UserData; }
		const String& getResourceName() const;
		virtual void setResourceName(const String& resName);

		void setSelected(bool selected);
		void setHovered(bool hovered);
		bool getSelected() const;
		bool getHovered() const;
		inline bool getIOprepared() const { return m_IOprepared; }
		inline bool getRequestPrepare() const { return m_bRequestPrepare; }

		inline SceneNode* getParentNode() const { return m_ParentNode; }
		inline SceneNode* getSceneNode() const { return m_SceneNode; }
		inline const Vector3& getPosition() const { return m_Position; }
		inline const Vector3& GetPosition() const { return getPosition(); }
		inline const Quaternion& getOritation() const { return m_Oritation; }
		inline const Vector3& getScale() const { return m_Scale; }
		inline ui8 getPiror() const { return m_Piror; }
		inline void setPiror(ui8 piror) { m_Piror = piror; }
		inline const char* getCustomNote() const { return m_Note.c_str(); }

		virtual void objectMoved() {}
		///以下四个函数都需要更新obj的所注册的包围盒的状态
		void setPosition(const Vector3& pos);
		void setOritation(const Quaternion& ori);
		void setScale(const Vector3& scale);
		void setMirrorType(const String& ms);
		String getMirrorTypeString() const;

		virtual void updateWorldAABB() = 0;

		///編輯器重設Position用於特殊對象
		//virtual void editor_setPosition(const Vec3& pos) = 0;
		//virtual void editor_setOritation(const Quan& ori) = 0;
		//virtual void editor_setScale(const Vec3& scale) = 0;

		//void registerSceneNode(SceneNode* parent, bool updateBounds = true);
		//void unregisterSceneNode(bool updateBounds = true);

		/// 四元数转欧拉角
		Quaternion reCaculateOritation();
		void reCaculateAngle();

		virtual void setVisible(bool visible)
		{
			m_bVisible = visible;
		}

		virtual bool getVisible() const
		{
			return m_bVisible;
		}

		/// -------------------------------------for editor---------------------------------------------
		virtual void getPropertyList(PropertyList & lst);
		virtual bool getPropertyType(const String & name, PropertyType &type);
		virtual bool getPropertyValue(const String & name, String &value);
		virtual bool setPropertyValue(const String & name, const String & value);

		virtual bool editor_IsCanTranslate() const;
		virtual bool editor_IsCanRotate() const;
		virtual bool editor_IsCanScale() const;
		virtual bool editor_IsCanMirror() const;

		virtual void editor_SetVisualHelpFront(bool bFront);

		virtual void	setSceneCreate(bool bscene);
		virtual bool	IsSceneCreate() const;

		virtual void setFadeIn(bool bFade);
		bool		isFadeIn() const;
		bool		isNormalFade() const;

		inline void setInsideList(bool bInside) { m_bInsideList = bInside; }
		inline void setFrustumCull(bool bCull) { m_bFrustumCull = bCull; }

	protected:
		/// 用于查询的帮助函数
		static std::pair<bool, Real> rayIntersectsMesh(const Ray& localRay, Mesh* pMesh);
		static std::pair<bool, Real> rayIntersectsMesh(const Ray& localRay, Mesh* pMesh, bool bTopPoint);

		/// 记录上次更新时的Frame，保证一帧只更新一次（渲染动态阴影图和渲染主场景都会引起更新）
		unsigned long		mLastUpdateFrameNumber;

	protected:
		bool		m_isNeedSave;		// 是否需要保存
		bool		m_IOprepared;		//是否多线程IO
		bool		m_bRequestPrepare;
		bool		m_bRequestDestroy;
		bool		m_bInsideList;		//当前是否在查询列表
		bool		m_bFrustumCull;		//当前是否被视锥裁剪掉
		bool		m_bSceneCreate;		//场景载入标记
		bool		m_bVisible;
		bool		editor_canTranslate;//能否平移
		bool		editor_canRotate;	//能否旋转
		bool		editor_canScale;	//能否缩放
		bool		editor_canMirror;	//能否镜像
		bool		editor_selected;	//是否选中，编辑器模式下 选中将渲染外框1
		bool		editor_hovered;		//是否hover 编辑器模式下 hover将渲染外框2
		ui32		m_Type;				//类型
		ui32		m_Mirror;			//镜像
		ui32		m_Piror;			//加载优先级 1,最高 2,次之 3,最低
		i32			m_Reverse[6];		//预留空间,方便版本扩展

		float			m_UnPrepareTime;	/// 对象延迟释放累计时间
		Real		m_Pitch;			//水平角度.
		Real		m_Yaw;				//俯仰角度.
		Real		m_Roll;				//测倾角度.

		void*		m_UserData;			//用户数据

		SceneNode*	m_ParentNode;		//父节点.
		SceneNode*	m_SceneNode;		//挂载点
		Zoom*			m_pOwnerZoom;
		QueryObject*	m_pHoverBoxQueryObject;
		QueryObject*	m_pSelectBoxQueryObject;
		Material*		m_Material;
		RenderQueue*	m_RenderQueue;

		Vector3		m_Position;			//位置
		Vector3		m_Scale;			//缩放
		Quaternion		m_Oritation;	//朝向

		Box				m_worldAABB;

		String		m_Name;				//名字	
		String		m_Resource;			//资源名字
		String		m_Note;				//备注
		String		m_ShaderDefine;		//shader
	};

	class ObjectPrepareEvent : public ThreadEvent
	{
	public:
		ObjectPrepareEvent(GameObject* pObject);
		virtual ~ObjectPrepareEvent();

		virtual bool ProcessEvent();

		virtual bool RespondEvent();

		virtual bool GetRepeatRespondIfFail() { return true; }
		virtual bool IsAutoDestroy() { return true; }

	protected:
		GameObject*	m_pObject;
	};

	class ObjectSubset : public ObjectAlloc
	{
		friend class Scene;
	public:
		ObjectSubset() :m_Type(GOT_MAX)
		{}
		virtual ~ObjectSubset() {}

		virtual void frameMove(i32 deltaTime) = 0;
		virtual void addObject(GameObject* object) = 0;
		virtual void deleteObject(GameObject* object) = 0;
		virtual void clearObjects() = 0;
	public:
		ui32 m_Type;
	};
}

#endif