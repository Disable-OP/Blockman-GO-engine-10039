/********************************************************************
filename: 	ActorObject.h
file path:	dev\engine\Src\Core\Actor\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef _ACTOROBJECT_H_
#define _ACTOROBJECT_H_

#include "Core.h"
#include "ActorTemplateData.h"
#include "SkillSystem.h"
#include "Resource/LordThread.h"
#include "Scene/CameraMain.h"

namespace LORD
{
	class ActorManager;
	class SkillAttack;
	class ActorDelayedProcess;
	class ActorMeshTexModify;

	//#define RENDERACTORAABB
	//#define RENDERDUMMYNODE

	class LORD_CORE_API ActorObject : public ObjectAlloc
	{
		friend class ActorManager;
		friend class SkillSystem;
		friend class ActorPrepareEvent;
		friend class ActorDestoryEvent;
		friend class ActorAttachMeshEvent;
		friend class MergeTexturesEvent;
		friend class MergeMeshesEvent;

		//--------------------------------------------- 数据类型定义 -------------------------------------
	public:
		// 子模型
		struct Child_Mesh
		{
			// 挂接点名，现在存储骨骼名称
			String SocketName;
			// 挂接SceneNode
			SceneNode *pParentNode = nullptr;
			// 自身的SceneNode
			SceneNode *pModelNode = nullptr;

			LORD::Mesh* pMesh = nullptr;
			ModelEntity* pModelEntity = nullptr;

			String Name;
		};

		struct MeshTextureModify
		{
			TextureModify modifier;
			ui32 backGround;
			ui32 processing;
			Color textColor;
			String showString;
			float ratio;
		};

		struct Attack_Event_Info
		{
			bool bMiss = false;
			//float FlyingTime;
			ActorObject* pTarget = nullptr;

			Attack_Event_Info()
			{
				bMiss = 0;
				pTarget = NULL;
				//FlyingTime = 0;
			}
		};

		struct SkillState
		{
			ui32  skillAnimTime = 0;
			int   AttCount = 0;
		};

	protected:

		// 颜色改变状态
		struct Color_Change_State
		{
			// 过渡时间
			int smoothTime;
			// 持续时间
			int durationTime;
			// 恢复时间
			int resumeTime;

			// 要改变成的颜色
			Color color;

			Color_Change_State()
			{
				smoothTime = -1000;
				durationTime = -1000;
				resumeTime = -1000;
				color = Color::BLACK;
			}
		};

		//顿帧
		struct SlowDownEvent
		{
			float beginTime;
			float EqualizeTimeScale;
			float LastTime;
			float EqualizeTime;
			float scale;

			SlowDownEvent()
			{
				beginTime = -1.0f;
				EqualizeTimeScale = 0.f;
				LastTime = 0.f;
				EqualizeTime = 0.f;
				scale = 0.0f;
			}
		};

		//腾空击飞
		struct HitFlyEvent
		{
			float duration;
			float curentTime;
			float high;
			float speed;

			HitFlyEvent()
			{
				curentTime = 0.f;
				duration = -1.f;
				high = 0.f;
				speed = 0.f;
			}
		};

		//摄像机拉近拉远
		struct CameraPushEvent
		{
			float beginTime;
			float closeTime;
			float durationTime;
			float farawayTime;
			float scale;
			float stopTime;
			float closeSpeed;
			float farawaySpeed;
			float curentScale;

			CameraPushEvent()
			{
				beginTime = 0.0f;
				scale = 0.0f;
				durationTime = 0.0f;
				closeTime = 0.0f;
				farawayTime = 0.0f;
				closeSpeed = 0.0f;
				farawaySpeed = 0.0f;
				curentScale = 0.0f;
				stopTime = 0.0f;
			}
		};

		// 角色缩放
		struct Actor_Scale_Info
		{
			bool scalling;

			float curentTime;

			float durationTime;

			float beginScale;

			float endScale;

			float scaleSpeed;

			Actor_Scale_Info()
			{
				curentTime = 0.0f;
				durationTime = 0.2f;
				beginScale = 1.0f;
				endScale = 1.0f;
				scaleSpeed = 0.0f;
				scalling = false;
			}
		};

		// 蒙皮网格
		// 注意:每个网格只能有一个子网格, 也就是说导出的时候必须散件导出然后在编辑器中进行组装
		struct Skin_Mesh
		{
			// 模板数据指针
			const ActorTemplateData::Skin* pTemplateSkin = nullptr;
			// 网格
			SkinnedMesh* pMesh = nullptr;
			SkinedEntity* pEntity_WriteZ = nullptr;
			SkinedEntity* pEntity = nullptr;
		};

		struct Static_Mesh
		{
			// 模板数据指针
			const ActorTemplateData::StaticMesh* pTemplateMesh = nullptr;
			SceneNode *pParentNode = nullptr;
			SceneNode *pMeshNode = nullptr;
			LORD::Mesh* pMesh = nullptr;
			ModelEntity *pEntity = nullptr;
		};

		// 装备特效
		struct Equip_Effect
		{
			// 播放速度
			float TimeScale;
			// 最大透明度
			float Alpha;
			// 模板数据指针
			const ActorTemplateData::ActorEffect* pTemplateEffect = nullptr;
			// 特效的挂接SceneNode
			SceneNode *pParentNode = nullptr;
			// 特效自身的SceneNode
			SceneNode *pEffectNode = nullptr;
			// 特效指针
			EffectSystem *pEffect = nullptr;
		};

		// 特效
		struct Actor_Effect
		{
			// 是否跟随父节点转动
			bool isOrientFollowParentNode = false;
			// 渐出标志
			bool bFadeOut = false;
			// 播放次数(小于等于0为无限次)
			int times = 0;
			// 权重
			float Weight = 0.0f;
			// 最大透明度
			float Alpha = 1.0f;
			// 播放速度
			float TimeScale;
			// 特效的挂接SceneNode
			SceneNode *pParentNode = nullptr;
			// 特效自身的SceneNode
			SceneNode *pEffectNode = nullptr;
			// 特效指针
			EffectSystem *pEffect = nullptr;
		};

		// 装备部件信息
		struct Body_Part_Info
		{
			// Mesh自身的Alpha
			float mAlpha;
			// 模板数据指针
			const ActorTemplateData::BodyPart *pTemplateData;
			// 蒙皮
			vector<Skin_Mesh>::type SkinMeshes;
			//模型
			vector<Static_Mesh>::type StaticMeshes;
			// 特效
			vector<Equip_Effect>::type EquipEffects;

			Body_Part_Info()
			{
				mAlpha = 1.0f;
			}
		};

		// Batch Info
		struct Actor_Batch_Info
		{
			unsigned int boneCount;			// 骨骼数量
			unsigned int dummyCount;		// 挂点数量
			unsigned int skillAttackCount;	// 飞行击中数量

			Actor_Batch_Info(void)
			{
				boneCount = 0;
				dummyCount = 0;
				skillAttackCount = 0;
			}
		};

		//------------------------------------------------- 成员函数定义 -------------------------------------
	protected:
		// 只能由ActorManager创建或释放
		ActorObject(bool isUIActor = false, bool bSingleThread = true, int AnUserData = 0, bool isHide = false);
		virtual ~ActorObject(void);
	public:
		// 根据模板数据进行初始化
		virtual void Init(ActorTemplateData* pTemplateData, bool isActorMaster = false);

		// 释放
		virtual void Free(void);

		// 获得数据模板的指针
		const ActorTemplateData* GetTemplateData() { return m_pTemplateData; }

		void setDefaultIdle(String defaultIdle) { m_defaultIdle = defaultIdle; }
		String getDefaultIdle() { return m_defaultIdle; }

		// 静态成员函数
	public:
		static String GetNextActorName(void);
		static String GetNextMeshName(void);
		static String GetNextSocketName(void);
		static String GetNextChildActorAlias(void);
		static String GetNextEffectName(void);
		static String GetNextEntityName(void);
		static String GetNextSkillAttackName(void);

		String bodyPartString;

	public:
		// 挂接到指定节点，参数为空时挂接到SceneNode根节点
		void AttachToParentNode(SceneNode* pParentSceneNode = NULL);

		// 从父节点脱离
		void DetachFromParentNode();

	public:
		// 更新状态
		virtual void Update(ui32 delta);
		virtual void UpdateForUI(ui32 delta, const Matrix4& viewproj);
		virtual void UpdateFromMatrix(ui32 delta, const Matrix4& viewproj);

		// 更新逻辑设置的位置姿态缩放，冻结的角色也更新
		void UpdateLogicState(void);

		// 设置上半身骨骼
		void setUpper(const String& upperRootName);

		bool prepared();
		bool mergeFinished() { return mbMergeFinished; }

		// 获得SceneNode
		SceneNode* GetSceneNodeMaster(void) const;

		// 获得Skeleton
		Skeleton* GetSkeletonMaster(void) const;

		// 获得Entity
		//SkinedEntity* GetEntityMaster(void) const;
		const set<SkinedEntity*>::type GetSkinEntitySet(void);

		// 设置缩放
		void SetScale(const Vector3 &scale);

		// 设置位置
		virtual void SetPosition(const Vector3 &position);

		// 设置朝向
		void SetOrientation(const Quaternion &orientation);

		// 获得缩放
		const Vector3& GetScale(void) const;

		// 获得原始缩放
		const Vector3& GetOriginalScale(void) const;

		// 获得飞行击中的爆炸比例
		float GetAttackScale(void) const;

		// 获得飞行击列表
		list<SkillAttack*>::type GetAttackList(void) const;

		// 获得选中光圈尺寸
		float GetSelectedRingSize(void) const;

		// 获得位置
		const Vector3& GetPosition(void) const;

		// 获得朝向
		const Quaternion& GetOrientation(void) const;

		// 设置颜色
		void SetNaturalColor(const ActorTemplateData::ColorChange* pData);
		void SetNaturalColor(const Color &color, bool bSmooth, ui32 FadeTime, bool bCascade);
		void SetNaturalColor(const Color &color, bool bCascade);

		// 设置自定义肤色
		void SetCustomColor(const Color& color);

		Texture* GetDummyTexturePtr() { return mDummyTexture; }

		// 设置手动更新
		void setUpdateManually(bool manually) { mbUpdateManually = manually; }
		bool isUpdateManually() const { return mbUpdateManually; }

		// 设置缺省颜色
		void SetDefaultColor(bool bSmooth, bool bCascade);
		// 设置相机抖动启用
		void SetCameraShakeEnable(bool enable, bool bCascade = false);

		void StartActorFadeOut(float TimeScale = 1.0f);
		void SetActorTranslateAlpha(float alpha);
		inline float GetActorTranslateAlpha() const { return m_pTemplateData->mfAlpha; }
		void CreateActorFadeEntity();
		void ReserveActorEntity();

		// 添加摄像机抖动
		void AddCameraShake(float beginTime, float scale, float durationTime, int shakeTimes);

		// 删除摄像机抖动
		void DeleteCameraShake(unsigned int id);

		// 清空摄像机抖动
		void ClearAllCameraShake(void);

		// 设置动画时间缩放
		void SetTimeScale(float timeScale, bool bCascade = false);
		void SetCurrentTimeScale(float timeScale, bool bCascade = false);

		inline float GetTimeScale() { return mTimeScale; }

		void AddSlowDown(const ActorTemplateData::SlowDown* pData);

		void AddSlowDown(float beginTime, float scale, float durationTime, float equalizeTime);

		void AddSlowDownToTargetActors(const ActorTemplateData::SlowDown* pData);

		void AddHitFly(const ActorTemplateData::HitFly* pData);

		void AddCameraPush(const ActorTemplateData::CameraPush* pData);

		void AddSkillActorScale(const ActorTemplateData::ActorScale* pData);

		void ChangeDefaultTexture(const String& name); // only support change defaultTexture for skinnedEntity
		void RevertDefaultTexture();

		void SetEffectTexture(int effect);
		Texture* GetEffectTexture(int effect);

		void RevertEffectTexture();

		void SetAmbientColor(const Color& color);
		inline const Color& GetAmbientColor() const { return mCurentAmbientColor; }

		void SetMirror(bool bMirror);
		inline bool IsMirror() const { return m_isMirror; }

		void ChangeDark(float radio);
		void RevertLight();
	protected:
		// 各种操作
	public:
		// 返回值为技能播放的时间，以毫秒为单位
		int PlaySkill(const String& name, bool bInsert = false, bool bCascade = false, float TimeScale = 1.0f, int userdata = -1, int userdata2 = -1, const String& actorNameArray = StringUtil::BLANK, bool playsound = true);
		void PlayUpperAnim(const String& name, bool bLooped = false, float timeScale = 1.f);
		void StopSkill(bool bCascade = true);

		// 播放受击动画
		void PlayWoundAction(SkillAttack* skillAtt);

		// 清空逻辑保存的变量;
		void	SetUserData(int AnUserData = 0) { m_nUserData = AnUserData; }

		const map<String, ActorTemplateData::Skill*>::type& GatSkillMap() const;

		bool IsSkillExist(const String& name);

		void GetSkillAnimTimeLength(const String& name, SkillState& __state);

		// 计算一个技能的播放时间
		int GetSkillTimeLength(const String& name);

	protected:
		// 播放受击硬直音效
		void PlayWoundSound();

		// 飞行击中相关
	public:

		// 挂接攻击技能实体
		void AddSkillAttack(SkillAttack* pSkillAttack);

		void AddSkillAttack(const String& name, ActorObject* pOriginActor = NULL, bool isCallBlack = false, int userdata = 0, int userdata2 = 0);
		// 删除攻击技能实体
		void RemoveSkillAttack(const String& name);

		// 释放所有的攻击技能
		void FreeAllSkillAttack();

		void LogBodyPartMeshs(const String& prompt);

		virtual void SetAttackTargetActors(Attack_Event_Info* pAttackEventInfo, unsigned int NumActors);
		virtual void AddAttackTargetActors(ActorObject* pTarget, bool bMiss);
		virtual void ClearAttackTargetActors();
		virtual void AddSkillAttackToTargetActors(const ActorTemplateData::AttackEvent* pData, const String& skillName, int userData, int userData2, int callnum, int callindex);
		virtual void GetAttackTargetActors(vector<ActorObject*>::type& list);

		// 装备切换相关
	public:
		// 设置主骨骼
		void SetMasterSkeleton(Skeleton* pSkeleton);
		void AttachSlave(const String& masterName, const String &slaveName);
		// 加载、卸载装备的实现函数，编辑器直接调用，客户端通过EnableSlave间接调用，pMeshes为用到的mesh资源指针
		bool UseBodyPart(const String& masterName, const String &slaveName);
		// 启用所有默认加载的组
		void EnableAllBodyParts(bool bEnabled);
		void _unloadAllBodyParts();
	protected:
		// 根据BodyPart创建Mesh对象
		void CreateBodyPartMeshes(const Body_Part_Info* pBodyPart, vector<LORD::SkinnedMesh*>::type& pMeshes);
		// 装载一个指定的组（调用前已确认该组没有加载）
		bool LoadBodyPart(Body_Part_Info* BodyPart);
		// 根据输入的Mesh对象装载指定的组，必须保证pMeshes里的Mesh和BodyPart一致，
		// 上面的函数也是通过调本函数实现功能
		bool LoadBodyPart(Body_Part_Info* BodyPart, const vector<LORD::SkinnedMesh*>::type& pMeshes);
		// 卸载一个组
		void UnloadBodyPart(Body_Part_Info* BodyPart);
		// 装载一个Skin，失败返回NULL
		SkinnedMesh* LoadSkin(Skin_Mesh &mesh, ActorTemplateData::Skin* pSkin, LORD::SkinnedMesh *pMesh);
		void LoadSkin(Skin_Mesh &mesh, LORD::SkinnedMesh *pMesh);
		void LoadMergedSkin(Skin_Mesh &mesh, LORD::SkinnedMesh *pMesh);
		// 卸载一个Skin
		void UnloadSkin(Skin_Mesh &mesh);

		// 刷新一个Skin的换色（仅编辑器用）
		void UpdateSkinChangeColor(Skin_Mesh &mesh);

		// 装在一个Mesh，失败返回NULL
		ModelEntity* LoadStaticMesh(Static_Mesh &mesh, ActorTemplateData::StaticMesh* pStaticMesh);
		// 卸载一个mesh
		void UnloadStaticMesh(Static_Mesh &mesh);
		// 装载一个装备特效
		EffectSystem* LoadEquipEffect(Equip_Effect& effect, ActorTemplateData::ActorEffect* pEffect);
		// 卸载一个装备特效
		void UnloadEquipEffect(Equip_Effect& effect);
	public:
		// 说明：子角色的挂接和解挂都分为两个步骤，先设置数据的连接关系，再更改表现效果。
		//		挂接子角色时，若骨骼已加载完毕，AttachChildActor中先设置数据连接关系，再
		//		再调用CompleteAttachChildActor完成SceneNode挂接；否则SceneNode挂接将通过
		//		延迟处理机制在骨骼加载后再调用。解除挂接的步骤与之类似。
		// 挂接子角色
		bool AttachChildActor(const String &BoneName, ActorObject *pActor);
		// 完成子角色挂接
		void CompleteAttachChildActor(const String &BoneName, ActorObject *pActor);
		// 解除子角色挂接
		void DetachChildActor(ActorObject *pActor);
		// 完成子角色挂接解除
		void CompleteDetachChildActor(ActorObject *pActor);
		// 获得父角色，若为根角色，返回空
		ActorObject* GetParentActor() const;
		// 获得根父亲，若为根角色，返回自己
		ActorObject* GetRootParent(void) const;

		// 卸载一个组
		void UnloadBodyPart(const String& master);
		void UnloadBodyPart(const String& master, const String& slave);

		//骑马
		bool UpHorse(ActorObject *pActor);
		void DownHorse(void);
		// 挂入特效
	public:
		// 添加挂入的特效，SocketName为"buff_effect"，则挂接到BuffEffect上
		float AddPluginEffect(const String &Name,
			const String &SocketName,
			const String &EffectName,
			float TimeScale = 1.0f,
			float Alpha = 1.0f,
			int times = -1,
			const Vector3 &Position = Vector3::ZERO,
			const Quaternion &Orientation = Quaternion::IDENTITY,
			const Vector3 &Scale = Vector3(1.f, 1.f, 1.f),
			bool isOrientFollowParentNode = true);

		// 删除挂接特效
		void DelPluginEffect(const String &Name, bool Smooth = true);

		//旋转挂接特效
		void SetPluginEffectQuaternion(const String &Name, const Quaternion &Orientation);

		//挂接选中特效,暂时不用传入挂点。因为挂在脚下。
		void AttachSelectEffect(const String& effectName/*,const String &SocketName*/);
		//解挂选中特效
		void DetachSelectEffect();

		void SetShadowVisible(bool visible);
		bool GetShadowVisible();

		// 子模型
	public:
		Child_Mesh* AddChildMesh(const String &Name,
			const String &SocketName,
			const String &MeshName,
			const Vector3 &Position = Vector3::ZERO,
			const Quaternion &Orientation = Quaternion::IDENTITY,
			const Vector3 &Scale = Vector3(1.f, 1.f, 1.f));

		void DeleteChildMesh(const String &Name);

	public:
		bool AddMeshTexModify(const String& master, const String& slave, const Vector2i& pos, const Vector2i& size);
		bool DelMeshTexModify();
		ActorMeshTexModify* getMeshTexModify() { return m_meshTexModify; }
		
	// 各种更新
	protected:
		bool _UpdateObjectState(ui32 delta);
		bool _UpdateObjectStateFromMat(ui32 delta, const Matrix4& viewproj);
		void UpdateChildActors(ui32 delta);
		void UpdateChildMeshs(ui32 delta, const Matrix4* viewproj = NULL);
		void UpdateStaticMeshs(ui32 delta, const Matrix4* viewproj = NULL);
		void UpdatePluginEffects(ui32 delta);
		void UpdateSkillAttacks(ui32 delta);
		void UpdateColor(ui32 delta);
		void UpdateEffects(ui32 delta);
		void UpdateEquipEffects(ui32 delta);
		void UpdateBuffEffectSceneNode(void);
		void UpdateSkillAbout(ui32 delta);
		void UpdateWeaponActor(ui32 delta);
		void UpdateSlowDown(ui32 delta);
		void UpdateHitFly(ui32 delta);
		void UpdateCameraPush(float delta);
		void UpdateSkillActorScale(float delta);
		void UpdateActorFadeOut(ui32 delta);

		bool GetCameraShakeState() { return isCameraShakeEnd; }
		// 渲染悬挂特效
		void RenderPluginEffect();
		// 渲染挂点的特效
		void RenderSocketEffect();
		// 渲染技能特效
		void RenderSkillEffect();
		// 多线程									  
	public:
		// 在多线程IO完成后调用的初始化，CurStep输入当前进行的步骤
		// 返回true表明初始化完毕；返回false时，CurStep返回下次要进行的步骤。
		bool prepare_io();
		bool InitAfterIOprepared(int& CurStep);
		bool mbMergeFinished = true;
	public:

		// 获得指定挂点的SceneNode指针，找不到返回SceneNodeMaster
		SceneNode* getSocketSceneNode(const String &SocketName);

		// 获得Buff特效挂点SceneNode指针，不存在时返回SceneNodeMaster
		SceneNode* getBuffEffectSceneNode(void);

		// 获得AOE Buff特效挂点SceneNode指针，不存在时返回SceneNodeMaster
		SceneNode* getAOEEffectSceneNode(void);

		// 获得包围盒顶端的SceneNode指针
		SceneNode* getTopBoxSceneNode(void);

		// 获得包围盒中心的SceneNode指针
		SceneNode* getCenterBoxSceneNode(void);

		// 获得指定挂点SceneNode的世界坐标系下的位置
		Vector3 GetSocketPosition(const String &socketName);

		// 获得头顶文字的世界坐标系下的位置
		Vector3 GetTextWorldPosition(void);

		Vector2 GetTextScreenPosition();

		// 获得文字相对世界坐标下头顶点位置的高度偏移
		float GetTextHeightOffset() const;

		const char* getActorNameID() { return mActorNameID.c_str(); }

		// 获得挂在某个挂点上的所有子角色
		void getChildActorBySocket(const String &SocketName, vector<ActorObject*>::type& ChildActors) const;

		// 获得角色的文件名称
		String GetActorFileName() const;

		// 刷新镜像和原始缩放的显示，仅给编辑器用
		void UpdateScaleShow(void);

		void SetBrightness(const Color& color);

		void EnableMergeRenderable(bool bEnable) { mMergeRenderable = bEnable; }//当关闭时，已经合并的就合并了，不删除 

		//骨骼SceneNode相关
		SceneNode* getBoneNode(Bone* pBone);
		virtual void initBoneNodes();
		void       UpdateBoneNodes();

		void		setUIActor(bool setting) { m_isUIActor = setting; }
		bool		isUIActor() const { return m_isUIActor; }

		bool        isMainActor()const { return m_isMainActor; }

		Box			getActorAABB();
		Box			getLocalAABB();
		bool        getHasInited() { return mHasInited; }
		int			getUserData() const { return m_nUserData; }
		bool        isActorLoadSkin() { return (mEntitySet.empty() ? false : true); }
		void        setActorFrozen(bool frozen) { m_isFrozen = frozen; }
		bool        getActorIsFrozen() { return m_isFrozen; }
		void        setActorHide(bool hide) { m_isHide = hide; }
		bool        getActorIsHide() { return m_isHide; }
		void        setActorBackup(bool backup) { m_isBackup = backup; }
		bool        isActorBackup() { return m_isBackup; }
		void		setActorPause(bool bSetting) { m_isPause = bSetting; }
		bool		getActorPause() const { return m_isPause; }

		void		setBlockMan() { m_isBlockMan = true; }
		void		setBlockManVisible() { m_isBlockManVisible = true; }

		AnimBlender*	getAnimationBlender() const;

		void		setOnlyCameraAnim(bool onlycamera) { mSkillSystem.SetOnlyCameraAnim(onlycamera); }

		void		refleshActorAlpha();

		void		setCamAnimHide(bool bHide) { m_CameraAnimHide = (m_isHide ? m_isHide : bHide); }
		bool		getCamAnimHide() const { return m_CameraAnimHide; }

		bool        frustumIntersects() const { return m_isFrustumVisible; }

		void		stopCameraAnim();

		void		requestMergeMeshesEvent();
		void		mergeMeshes();
		void		releaseMergeResource();
		// 其他属性
	protected:
		bool mMergeRenderable;
		bool mIsActorSizeValid; // mDefaultActorSize是否有效
		bool mHasInited;	// 多线程 是否已经完成初始化	
		bool mbSingleThread; // 强制单线程执行的标志，只能在创建Actor时设置
		bool mbUpdateManually; // 手动更新Actor
		
		TextureSet* mMergeTexture = nullptr;
		SkinnedMesh* mMergeSkin = nullptr;

		Texture* mDummyTexture = nullptr;

		// 更新标志
		bool mPositionDirty;
		bool mOrientationDirty;
		bool mScaleDirty;

		bool		m_isUIActor;
		bool        m_isPause;
		bool        m_isFrozen;
		bool        m_isHide;
		bool		m_isBackup;// 为了换模型，老的资源不释放，加一个标志位
		bool        m_isSkillHideActor;
		bool		isCameraShakeEnd;
		bool        m_isMainActor;
		bool        m_isActorFadeOut;
		bool		m_isMirror;
		bool		m_bTranslate;
		bool		m_CameraAnimHide;	// 用于摄像机动画中，隐藏角色
		bool		m_MarkDestroy;
		bool		m_isFrustumVisible;

		/** 如果是BlockMan的逻辑Actor，MC自身有一套视锥剔除逻辑和渲染，为了兼容。*/
		bool		m_isBlockMan = false;
		bool		m_isBlockManVisible = false;

		ActorMeshTexModify* m_meshTexModify = nullptr;

		// 时间缩放
		float mBaseScale;
		float mTimeScale;
		//摄像机相关
		float		mCameraPushDelta;
		float		mCameraShakeDelta;

		float		m_FadeOutScale;		// 加速半透明时间
		float		m_fAlpha;

		float		m_DarkRadio;

		int			m_nUserData;
		ui32        m_curDelta;

		// 模板数据指针（优化时可能需要变为非const的）
		const ActorTemplateData* m_pTemplateData;

		//阴影
		EffectSystem* m_pShadowEffect;
		SceneNode*    m_pShadowSceneNode;
		Texture*	m_pDefaultTexture;	// 用于运行时切换默认贴图

										// 父角色
		ActorObject *mParentActor;
		// 主节点
		SceneNode *mSceneNodeMaster;
		//主骨骼
		Skeleton* mSkeletonMaster;
		// Buff特效挂点
		SceneNode* mBuffEffectSceneNode;
		// AOE特效挂点
		SceneNode* mAOEEffectSceneNode;
		// Top_Box挂点
		SceneNode* mTopBoxSceneNode;
		// Center_Box挂点
		SceneNode* mCenterBoxSceneNode;
		//坐骑
		ActorObject* mHorse;

		AnimBlender* m_pAnimBlender;

		SceneNode**	mBoneNodeVector;

		EffectSystem* mpSelectEffect;

		// 缩放：用于设置放大缩小效果等
		Vector3 mScale;
		// 位置
		Vector3 mPosition;
		// 应用新颜色
		Color mNaturalColor;
		Color mCustomColor;
		Color mAlphaColor;
		//边缘光颜色
		Color mCurentRimColor;
		//自定义环境色
		Color mCurentAmbientColor;
		// 光照参数x 高光系数, y边缘光系数
		Vector4 mCurentColorParams;
		// 朝向
		Quaternion mOrientation;

		bool mUnloadAllBodyParts;

		//顿帧
		SlowDownEvent m_slowDown;
		//击飞腾空
		HitFlyEvent m_HitFly;
		//摄像机拉近拉远
		CameraPushEvent m_CameraPush;
		//角色缩放
		Actor_Scale_Info m_SkillActorScale;
		// 颜色状态
		Color_Change_State mColorChangeState;

		String m_UpperRootBoneName;

		// 技能管理系统
		SkillSystem mSkillSystem;

		String mActorNameID;
		String mParentSocketName;// 角色挂接的父角色的挂点名称
		set<SkinedEntity*>::type mEntitySet; // Entity
		multimap<String, Body_Part_Info*>::type mBodyParts; // 装备集合（作为multimap的key的String存储用于换装的MasterName）
		set<ActorObject*>::type mChildActors; // 子角色集合（Key为子角色别名，由ActorObject内部分配，保证一个Actor内不重复）
		map<String, Actor_Effect>::type mPluginEffects; // 挂入的特效集合（客户端动态添加的）
		map<String, Child_Mesh*>::type mChildMeshs; // 挂入的模型集合（客户端动态添加的）
		set<Animation*>::type mAnimtions;
		vector<ActorDelayedProcess*>::type mDelayProcessVector;
		list<SkillAttack*>::type mSkillAttackList;
		vector<Attack_Event_Info>::type mTargetActors;
		map<unsigned int, Camera_Shake_Info>::type mCameraShakeMap; //相机振动信息

		bool m_needUpdate = false;
		String m_defaultIdle = "";

#ifdef RENDERDUMMYNODE
		vector<QueryObject*>::type m_DummyObjects;
#endif
#ifdef RENDERACTORAABB
		QueryObject*	m_BoxObject;
#endif
	};

	class ActorPrepareEvent : public ThreadEvent
	{
	public:
		ActorPrepareEvent(ActorObject* pActor);
		virtual ~ActorPrepareEvent();

		virtual bool GetRepeatRespondIfFail() { return false; }

		virtual bool IsAutoDestroy() { return true; }

		virtual bool ProcessEvent();

		virtual bool RespondEvent();

	protected:
		int m_RespondStep;
		ActorObject* m_pActor;
	};

	class ActorPreDestoryEvent : public ThreadEvent
	{
	public:
		ActorPreDestoryEvent(ActorObject* pActor);
		virtual ~ActorPreDestoryEvent();

		virtual bool GetRepeatRespondIfFail() { return true; }

		virtual bool IsAutoDestroy() { return true; }

		virtual bool ProcessEvent();

		virtual bool RespondEvent();

	protected:
		ActorObject * m_pActor;
	};

	class ActorDestoryEvent : public ThreadEvent
	{
	public:
		ActorDestoryEvent(ActorObject* pActor);
		virtual ~ActorDestoryEvent();

		virtual bool GetRepeatRespondIfFail() { return true; }

		virtual bool IsAutoDestroy() { return true; }

		virtual bool ProcessEvent();

		virtual bool RespondEvent();

	protected:
		ActorObject* m_pActor;
	};

	class ActorDeAttachAllEvent : public ThreadEvent
	{
	public:
		ActorDeAttachAllEvent(ActorObject* pActor);
		virtual ~ActorDeAttachAllEvent() {}

		virtual bool GetRepeatRespondIfFail() { return false; }

		virtual bool IsAutoDestroy() { return true; }

		virtual bool ProcessEvent() { return true; }

		virtual bool RespondEvent();

	protected:
		ActorObject* m_pActor;
	};

	class ActorAttachMeshEvent : public ThreadEvent
	{
	public:
		ActorAttachMeshEvent(ActorObject* pActor, vector<SkinnedMesh*>::type& pMeshes, const String& masterName, const String& slaveName);
		virtual ~ActorAttachMeshEvent();

		virtual bool GetRepeatRespondIfFail() { return false; }

		virtual bool IsAutoDestroy() { return true; }

		virtual bool ProcessEvent();

		virtual bool RespondEvent();

	protected:
		ui32 mCurrentIndex;
		ActorObject* m_pActor;
		String m_MasterName;
		String m_SlaveName;
		vector<SkinnedMesh*>::type m_MeshesVec;
	};

	class MergeTexPrepareEvent : public ThreadEvent
	{
	public:
		MergeTexPrepareEvent(ActorObject* pActor);
		virtual ~MergeTexPrepareEvent();

		virtual bool GetRepeatRespondIfFail() { return false; }
		virtual bool IsAutoDestroy() { return true; }

		virtual bool ProcessEvent();

		virtual bool RespondEvent();

	protected:
		ActorObject * m_pActor;
	};

	class MergeTexturesEvent : public ThreadEvent
	{
	public:
		MergeTexturesEvent(ActorObject* pActor);
		virtual ~MergeTexturesEvent();

		virtual bool GetRepeatRespondIfFail() { return false; }

		virtual bool IsAutoDestroy() { return true; }

		virtual bool ProcessEvent();

		virtual bool RespondEvent();
	protected:
		ActorObject * m_pActor;
		multimap<int, String>::type m_Textures;
		TextureSet* m_TextureSet;
		map<String, int>::type m_TextureIds;
		bool m_NeedMerge;
	};

	class MergeMeshesEvent : public ThreadEvent
	{
	public:
		MergeMeshesEvent(ActorObject* pActor, TextureSet* pTexSet, map<String, int>::type& textureIds);
		virtual ~MergeMeshesEvent();

		virtual bool GetRepeatRespondIfFail() { return false; }

		virtual bool IsAutoDestroy() { return true; }

		virtual bool ProcessEvent();

		virtual bool RespondEvent();
	protected:
		ActorObject* m_pActor;
		TextureSet* m_pTexSet;
		SkinnedMesh* m_pSkinnedMesh;
		map<String, int>::type m_TextureIds;
		vector<Mesh*>::type m_Meshes;
		vector<std::pair<Mesh*, int>>::type m_staticMeshes;
		bool m_NeedMerge;
	};

	class ActorDelayedProcess : public ObjectAlloc
	{
	public:
		ActorDelayedProcess() {}
		virtual ~ActorDelayedProcess() {}

		virtual void Execute(ActorObject* pActor) = 0;
	};

	class SkillDelayedPlayProcess : public ActorDelayedProcess
	{
	public:
		SkillDelayedPlayProcess(const String& name, bool bInsert, bool casecade, float timescale, int userdata, int userdata2, const String& actorNameArray);
		virtual ~SkillDelayedPlayProcess();

		virtual void Execute(ActorObject* pActor);
	protected:
		bool m_bInsert;
		bool m_bCasecade;
		int m_Userdata;
		int m_Userdata2;
		float m_Timescale;
		String m_Name;
		String m_ActorNameArray;
	};

	class SkillDelayedStopProcess : public ActorDelayedProcess
	{
	public:
		SkillDelayedStopProcess(bool casecade);
		virtual ~SkillDelayedStopProcess();

		virtual void Execute(ActorObject* pActor);

	protected:
		bool m_bCasecade;
	};

	class EffectDelayedAddProcess : public ActorDelayedProcess
	{
	public:
		EffectDelayedAddProcess();
		virtual ~EffectDelayedAddProcess();

		virtual void Execute(ActorObject* pActor);
	public:
		bool        m_isOrientFollowParentNode;
		int			m_Times;
		float		m_TimeScale;
		float		m_Alpha;
		Vector3		m_Position;
		Vector3		m_Scale;
		Quaternion	m_Orientation;
		String		m_Name;
		String		m_SocketName;
		String		m_EffectName;
	};

	class EffectDelayedDelProcess : public ActorDelayedProcess
	{
	public:
		EffectDelayedDelProcess(const String& name, bool smooth);
		virtual ~EffectDelayedDelProcess();

		virtual void Execute(ActorObject* pActor);
	public:
		bool m_Smooth;
		String m_Name;
	};

	class SlaveAttachProcess : public ActorDelayedProcess
	{
	public:
		SlaveAttachProcess(const String& masterName, const String& slaveName);
		virtual ~SlaveAttachProcess();

		virtual void Execute(ActorObject* pActor);
	public:
		String m_MasterName;
		String m_SlaveName;
	};

	class ChildAttachProcess : public ActorDelayedProcess
	{
	public:
		ChildAttachProcess(ActorObject* pChild, const String& SocketName);
		virtual ~ChildAttachProcess();

		virtual void Execute(ActorObject* pActor);
	public:
		ActorObject* m_ChildActor;
		String m_SocketName;
	};

	class ChildDetachProcess : public ActorDelayedProcess
	{
	public:
		ChildDetachProcess(ActorObject* child);
		virtual ~ChildDetachProcess() {}

		virtual void Execute(ActorObject* pActor);
	public:
		ActorObject* m_ChildActor;
	};

	class ChildAddDelayedProcess : public ActorDelayedProcess
	{
	public:
		ChildAddDelayedProcess(const String& boneName);
		virtual ~ChildAddDelayedProcess() {}

		virtual void Execute(ActorObject* pActor);
	public:
		String m_BoneName;
	};

	class ChildDelDelayedProcess : public ActorDelayedProcess
	{
	public:
		ChildDelDelayedProcess() {}
		virtual ~ChildDelDelayedProcess() {}

		virtual void Execute(ActorObject* pActor);
	public:
	};

	class DefaultTexDelayProcess : public ActorDelayedProcess
	{
	public:
		DefaultTexDelayProcess(const String& TextureName)
			:m_TextureName(TextureName)
		{}
		~DefaultTexDelayProcess() {}

		virtual void Execute(ActorObject* pActor);
	public:
		String m_TextureName;
	};

	class RevertTexDelayProcess : public ActorDelayedProcess
	{
	public:
		RevertTexDelayProcess() {}
		~RevertTexDelayProcess() {}
		virtual void Execute(ActorObject* pActor);
	};
}

#endif