/********************************************************************
filename: 	ActorTemplateData.h
file path:	dev\engine\Src\Core\Actor\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef _ACTOR_TEMPLATE_DATA_H_
#define _ACTOR_TEMPLATE_DATA_H_

#include "Core.h"
#include "Math/Vector3.h"
#include "Image/Color.h"

namespace LORD
{

	// 颜色改变过渡时间
	const ui32 COLOR_CHANGE_TIME = 500;

	// 动画时间常量
	const ui32 ANIM_FADE_TIME = 200;
	const ui32 ANIM_FLOAT_DISTIME = 1;

	// 加载和存储Actor模板数据
	/* 
	说明：同样种类的Actor的原始数据（如mesh文件、skeleton文件、技能、基本动作、附加效果、
	原始颜色、原始缩放等）是相同的，这些信息直接存储到ActorObject中会产生重复，应该
	提取出来单独存储。
	*/
	class SkinedModel;
	class ActorTemplateDataManager;
	class LORD_CORE_API ActorTemplateData  : public ObjectAlloc
	{
		friend class ActorTemplateDataManager;
		friend class ActorObject;
		friend class SkillSystem;
		friend class ActorManager;
		friend class ActorAttachMeshEvent;
		friend class ActorPrepareEvent;

	public:
		// 受击硬直音效 默认3d且不循环
		struct WoundSound
		{
			// 随机文件样本数
			int soundFileNum;

			// 音量
			float volume;

			// 最近距离
			float minDistance;

			// 最远距离
			float maxDistance;

			// 最小间隔
			float intervalTime;

			// 声音名 随机文件1
			String soundName1;

			// 声音名 随机文件2
			String soundName2;

			// 声音名 随机文件3
			String soundName3;

			// 声音名 随机文件4
			String soundName4;

			// 声音名 随机文件5
			String soundName5;

			void init()
			{
				soundName1 = StringUtil::BLANK;
				soundName2 = StringUtil::BLANK;
				soundName3 = StringUtil::BLANK;
				soundName4 = StringUtil::BLANK;
				soundName5 = StringUtil::BLANK;
				soundFileNum = 0;
				volume = 0.8f;
				minDistance = 1.0f;
				maxDistance = 16.0f;
				intervalTime = 2.0f;
			}
		};

		// 音效（技能用）
		struct Sound
		{
			Sound()
				: length(0.0f)
			{
			}
			// 是否循环播放
			bool bloop;

			//是否广播
			bool isAddToAllActors;

			// 随机文件样本数
			int soundFileNum;

			// 开始时间
			float beginTime;  

			// 持续时间
			float length;

			// 音量
			float volume;

			// 最近距离
			//float minDistance;

			// 最远距离
			//float maxDistance;

			float addToAllActorVolume;

			//触发概率
			float showRate;

			// 声音名 随机文件1
			int soundName;

			// 随机文件2
			int soundName2;
		};

		// 摄像机震动效果（技能用）
		struct CameraShake
		{
			// 幅度
			float shakescale;

			// 开始时间
			float beginTime;

			// 持续时间
			float durationTime;

			//是否广播
			bool isAddToAllActors;

			//震动次数
			int  shakeTimes;

			//是否显示
			bool isShow;

			CameraShake()
			{
				isShow = true;
				shakeTimes = 1;
			}
		};

		// 特效（技能和附加效果用）
		struct ActorEffect
		{
			// 挂点名称
			String SocketName;  

			// 特效文件名称
			String EffectFileName;                                           

			// 开始时间
			float beginTime;

			// 播放次数(小于等于0为无限次)
			int times;

			// 播放速度
			float TimeScale;

			// 初始Alpha
			float Alpha;

			// 播放周期
			float CycleTime;

			// 是否跟随Actor
			bool FollowActor;

			// 是否持续存在
			bool Persist;

			// 偏移
			Vector3 position;
			float yaw;
			float pitch;
			float roll;
			float scale;

			//是否参与顿针
			bool isSlowdownValuable;

			ActorEffect()
			{
				isSlowdownValuable = true;
			}
		};

		// 攻击事件
		struct AttackEvent
		{
			// 开始时间
			float beginTime;

			// 飞行击中att文件名
			String AttackFileName;

			// 播放次数(1至100次，由于反复创建不能太多)
			int times;

			// 播放周期(大于0.01秒)
			float CycleTime;

			//是否显示
			bool isShow;

			bool isCallback;

			AttackEvent()
			{
				isShow = true;
				isCallback = true;
			}
		};

		// 击飞腾空
		struct HitFly
		{
			// 开始时间
			float beginTime;

			// 击飞最高点
			float high;

			// 持续时间
			float durationTime;

			//是否显示
			bool isShow;

			HitFly()
			{
				beginTime    = 0.0f;
				high		 = 1.0f;
				isShow		 = true;
				durationTime = 0.5f;
			}
		};

		// 技能断点
		struct SkillBreak
		{
			// 开始时间
			float beginTime;

			//是否回调
			bool isCallBlack;

			//是否冰冻角色
			bool isFrozen;

			//是否隐藏角色
			bool isHide;

			SkillBreak()
			{
				beginTime     = 0.0f;
				isHide        = false;
				isCallBlack   = false;
				isFrozen      = false;
			}
		};

		// 摄像机拉近拉远
		struct CameraPush
		{
			// 开始时间
			float beginTime;

			// 拉近时间
			float closeTime;

			// 持续时间
			float durationTime;

			// 拉远时间
			float farawayTime;

			// 振幅
			float scale;

			//是否翻转
			bool isInversion;

			//是否显示
			bool isShow;

			//是否广播
			bool isAddToAllActors;

			CameraPush()
			{
				beginTime    = 0.0f;
				scale        = 0.0f;
				durationTime = 0.0f;
				closeTime	 = 0.0f;
				farawayTime  = 0.0f;
				isInversion  = false; 
				isShow		 = true;
				isAddToAllActors = false;
			}
		};

		// 颜色改变
		struct ColorChange
		{
			// 开始时间
			float beginTime;

			//过度时间
			float smoothTime;

			// 持续时间
			float durationTime;

			//恢复时间
			float resumeTime;

			// 改变的颜色
			Color color;

			// 是否用于子Actor
			bool bCascade;

			ColorChange()
			{
				beginTime = 0.0f;
				smoothTime = 0.2f;
				durationTime = 0.2f;
				resumeTime = 0.2f;
				color = Color::BLACK;
				bCascade = true;
			}
		};

		//角色缩放
		struct ActorScale
		{
			float beginTime;

			float durationTime;
			
			float beginScale;

			float endScale;

			ActorScale()
			{
				beginTime = 0.0f;
				durationTime = 0.2f;
				beginScale   = 1.0f;
				endScale     = 1.0f;
			}
		};

		// 顿帧
		struct SlowDown
		{
			//慢放比例
			float timeScale;

			// 开始时间
			float beginTime;

			// 持续时间
			float durationTime; 

			// 补偿时间
			float equalizeTime;

			// 是否用于子Actor
			bool bCascade;

			//是否显示
			bool isShow;

			//触发概率
			float showRate;

			SlowDown()
			{
				bCascade = false;
				isShow	 = true;
				showRate = 0.4f;
			}
		};

		// 动画
		struct AnimationInfo
		{
			// 开始时间
			ui32 beginTime;

			// 播放次数(小于等于0为无限次)
			int times;

			// 基本动作名称(Echo的动画名称)
			String AnimName;

			// 作用的骨骼组名称(为空则影响所有骨骼)
			String channel;

			String fileName;

			// Echo骨骼动画的时间长度
			ui32 Length;

			// 动画过渡时间
			ui32 FadeTime;

			// 动画播放速度
			float TimeScale;

			//技能名
			String skillName;

			Vector2* pSetScale;

			AnimationInfo(): FadeTime(ANIM_FADE_TIME)
			{
				TimeScale = 1.0f;
				pSetScale = NULL;
			}
		};

		// 摄像机动画
		struct CameraAnim
		{
			// 开始时间
			float beginTime;
			String animName;
		};

		//暗化场景
		struct ChangeDark
		{
			float beginTime;
			float darkTime;
			float darkness;
			float duration;
			float lightTime;
			float lightness;
			ChangeDark()
				: darkTime(0.3f)
				, darkness(0.5f)
				, duration(1.0f)
				, lightTime(0.3f)
				, lightness(0.5f)
			{
				
			}
		};

		// 技能
		struct LORD_CORE_API Skill
		{
			Skill()
			{
				bWoundEnable = false;
				UserDefineTime = -2.0f;
				TextMoveMode = 0;
				pSetScale = NULL;
			}

			~Skill();

			// 特效集合
			vector<ActorEffect*>::type ActorEffects;

			// 声音集合
			vector<Sound*>::type Sounds;

			// 颜色改变集合
			vector<ColorChange*>::type ColorChanges;

			// 摄像机震动集合
			vector<CameraShake*>::type CameraShakes;

			// 顿帧集合
			vector<SlowDown*>::type SlowDowns;

			// 顿帧集合
			vector<HitFly*>::type HitFlys;

			// 顿帧集合
			vector<ActorScale*>::type ActorScales;

			// 技能断点集合
			vector<SkillBreak*>::type SkillBreaks;

			// 摄像机拉近拉远集合
			vector<CameraPush*>::type CameraPushs;

			// 攻击事件集合
			vector<AttackEvent*>::type AttackEvents;

			// 动画集合
			map<String, AnimationInfo*>::type Animations;

			// 摄像机动画集合
			vector<CameraAnim*>::type CameraAnims;

			// 场景暗化集合
			vector<ChangeDark*>::type ChangeDarks;

			// 用户定义的技能时间
			// 该变量为-2时，技能时间以动作为准，否则以该变量为准（-1为无限循环播放）
			float UserDefineTime;

			// 是否使用受击
			bool bWoundEnable; 

			// 头顶字移动模式 0 不动（默认）；1 仅跟随骨骼上下动；2 全方位跟随骨骼移动
			unsigned char TextMoveMode;

			// 播放Skill时，缩放Actor,x为初始大小，y为最终大小。
			Vector2* pSetScale;
		};

		// 皮肤
		struct Skin
		{
			// Skin文件名
			String MeshName;

			// 换色
			Color ChangeColor;

			Vector2 UVspeed;

			Skin()
			{
				UVspeed = Vector2::ZERO;
			}

		};

		struct StaticMesh
		{
			// 挂点名称
			String SocketName;  
			// Mesh文件名
			String MeshName;
			// 偏移
			Vector3 position;
			Vector2 UVspeed;
			float yaw;
			float pitch;
			float roll;
			float scale;
			StaticMesh()
			{
				yaw = 0.0f;
				pitch = 0.0f;
				roll = 0.0f;
				scale = 1.0f;
				position = Vector3::ZERO;
				UVspeed = Vector2::ONE;
			}
		};

		// 身体部分（用于换装）
		struct LORD_CORE_API BodyPart
		{
			BodyPart()
			{
				bDefaultUse = false;
				m_edgeColor = Color::WHITE;
				m_colorParams = Vector4::ONE;
			}

			~BodyPart();

			// 装备名称
			String SlaveName;
			String MasterName;
			
			//钩边相关
			Color m_edgeColor;
			Vector4 m_colorParams;

			// 装备附加皮肤
			vector<Skin*>::type Skins;

			vector<StaticMesh*>::type StaticMeshs;

			// 装备附加特效
			vector<ActorEffect*>::type ActorEffects;

			// 编辑器是否默认启用
			bool bDefaultUse;

			// 分组（编辑器）
			String m_group;
		};

		// 部件类型分组
		struct TypeGroup
		{
			String TypeGroupName;
		};

	protected:
		ActorTemplateData(void);
		~ActorTemplateData(void);

	public:
		bool LoadActorFile(const String& FileName);
		bool LoadParentTemplateFile(const String& FileName);
		void Free();
		void setUpperBodyBonesName(const vector<String>::type& namelist);

		String GetMasterSkeletonName() const{ return SkeletonName; }
		//String GetMasterMeshName() const{ return MasterMeshName; }
		void GetMeshNames(const String& MasterName, const String &slaveName, StringArray& retMeshNames) const;

		// 根据骨骼文件初始化信息，客户端仅第一次创建Actor实例时调用
		void InitBySkeleton(Skeleton* pSkel);

		// 设置受击硬直音效
		void SetWoundSound(const ActorTemplateData::WoundSound& woundSound) { mWoundSound = woundSound; }

		// 获得受击硬直音效
		const ActorTemplateData::WoundSound& GetWoundSound(void) const { return mWoundSound; }

		const map<String, Skill*>::type& GetSkillMap() const { return SkillsMap; }

		String GetParentTemplateName() const { return ParentTemplateName; }

	protected:
		bool Load(void* pNode, bool isParentTemplate = false);
		bool LoadBodyParts(void* pNode, bool isParentTemplate = false);
		bool LoadSkills(void* pNode);
		bool LoadAnims(void* pNode);
		StaticMesh* LoadStaticMesh(void* pNode);
		ActorEffect* LoadEffect(void* pNode);
		bool LoadBoneGroups(void* pNode);
		bool LoadAppendEffects(void* pNode);

	protected:
		//设置是否显示阴影
		bool mbShowShadow;
		// 记录是否至少创建过一次实例
		bool mbInstanceCreate;

		// 飞行击中爆炸比例
		float mAttackScale;
		// 选中光圈尺寸
		float mSelectedRingSize;
		// 头顶字偏移高度
		float mTextHeight;
		// 记录头顶字原始高度（从骨骼读出）
		float mTextHeightOriginal;
		// 受击最大幅度的转角
		float mWoundAngle;
		// 角色透明度
		float mfAlpha;

		// 原始缩放
		Vector3 OriginalScale;
		// 记录主要根骨骼位置
		Vector3 mMainRootBonePosition;

		// 受击硬直音效
		WoundSound mWoundSound;

		// 模板名
		String TemplateName;
		// 父模板名
		String ParentTemplateName;
		// 主要根骨骼名称（动态处理包围盒用）
		String mMainRootBoneName;
		// 主网格文件名
		//String MasterMeshName;
		// 主骨骼文件名
		String SkeletonName;
		// 受击骨骼名称
		String mWoundBoneName;
		// 技能集合
		map<String, Skill*>::type SkillsMap;
		//身体部分集合（key为MasterName.SlaveName的形式，具有唯一性）
		map<String, BodyPart*>::type BodyPartsMap;
		map<String, BodyPart*>::type ChildBodyPartsMap;
		vector<String>::type AnimsList;
		// 骨骼分组
		// 上半身骨骼名集合
		set<String>::type mUpperBodyBonesName;
		// 上下半身骨骼标号集合
		vector<unsigned short>::type m_UpperBoneIndices;
		vector<unsigned short>::type m_LowerBoneIndices;
		// 附加效果集合
		list<ActorEffect*>::type mAppendEffects;
	};

	// 单件，负责管理所有的Actor模板数据，负责所有ActorTemplateData的new和delete
	class LORD_CORE_API ActorTemplateDataManager : public ObjectAlloc
	{ 
	public:
		ActorTemplateDataManager(void);
		~ActorTemplateDataManager(void);

	public:
		static ActorTemplateDataManager* getSingletonPtr();
		static ActorTemplateDataManager& getSingleton();

		// 通过文件名打开Actor文件，已存在的直接返回Actor模板数据，不会重复读文件
		ActorTemplateData* LoadByFile(const String& FileName);
		ActorTemplateData* LoadByStream(const String& FileName, DataStream* datastream);
		// 清空所有已加载的Actor模板
		void Clear();

	private:
		static ActorTemplateDataManager* mSingleton;

		// key为actor文件的文件名（不带路径），value为Actor模板数据指针
		map<String, ActorTemplateData*>::type mActorTemplateData;
	};
}

#endif