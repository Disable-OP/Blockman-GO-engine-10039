/********************************************************************
filename: 	SkillAttack.h
file path:	dev\engine\Src\Core\Actor\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef _SKILLATTACK_H_
#define _SKILLATTACK_H_

#include "Core.h"
#include "Object/Singleton.h"
#include "Math/Quaternion.h"
#include "Image/Color.h"

namespace LORD
{ 
	// 非指向性技能距离常量
	const float UNDIRECTIVITY_DISTANCE = 25.f;

	class EffectSystem;
	class ActorObject;
	class SoundSource;

	class LORD_CORE_API SkillAttack : public ObjectAlloc
	{
	public:
		friend class SkillAttackManager;

		enum FlyLineType  // 飞行曲线方式
		{
			FLT_STRAIGHTLINE,	//直线
			FLT_PARABOLA,		//抛物线
			FLT_MISSILE,		//导弹
			FLT_SHOWER,			//流星雨
			FLT_SPIRAL,			//螺旋线
			FLT_BANANA,			//香蕉线
			FLT_VERTEX,			//定点
			FLT_LINK,			//连接
			FLT_RAMDOM,			//随即移动
			FLT_STRAIGHTLINE_NO_ROTAE,	//直线(不计算旋转型)
		};

		enum FlyDirection
		{
			FD_SRCTODEST,		//自身到目标
			FD_DESTTOSRC,		//目标到自身
			FD_DESTDOWN,		//目标向下	
			FD_DESTUP,			//目标向上
			FD_SRCDOWN,			//自身向下
			FD_SRCUP,			//自身向上
			FD_ONLYDEST,		//目标定点
			FD_ONLYSRC,			//自身定点
			FD_SRCLINKDEST,		//自身到目标的连线
			FD_DESTLINKSRC,		//目标到自身的连线
		};

		enum SkillAttackState
		{
			SAS_WAITTING,		//未播放
			SAS_PLAYING,		//播放状态
			SAS_STOPED,			//停止状态
		};

		enum CurrentFlyState
		{
			CFS_UNLAUNCH,		//未发射
			CFS_FLYING,			//飞行
			CFS_ATTACK,			//击中
			CFS_STOPED,			//停止
		};

	protected:
		SkillAttack();
		~SkillAttack();

	public:
		static String getSkillSceneNodeName();

		void setTargetActor(ActorObject* targetActor);

		SkillAttackState getState() const;

		// set the flying time. if not set. use the editor's data.
		void setFlyingTime(Real flyTime);

		void setSkillMiss(bool miss);

		void prepare(ActorObject* pOriginalActor);
		void play();
		void pause();
		void resume();
		void stop();

		void unprepare();

		bool IsPlaying() const;
		bool IsPause() const;
		bool IsStoped() const;

		bool update(unsigned int tickTime);

		bool loadSkillAttack(const String& fileName);
		bool loadSkillAttack(DataStream& stream);

		void setPlayWoundAction(bool bplayWoundAction) { mbPlayWoundAction = bplayWoundAction; }

		// properties
		inline void			 setFlyEnable(bool bFly) { mbFly = bFly; }
		inline bool			 getFlyEnable() const { return mbFly; }

		inline void          setFlyEffectName(const String& flyname) { mFlyEffectName = flyname; }
		inline const String& getFlyEffectName() const { return mFlyEffectName; }

		inline void			 setBeginNodeName(const String& beginNodeName) { mBeginNodeName = beginNodeName; }
		inline const String& getBeginNodeName() const { return mBeginNodeName; }

		inline void			 setBeginOffset(Vector3 beginOffset) { mBeginOffset = beginOffset; }
		inline Vector3		 getBeginOffset() const { return mBeginOffset; }

		inline void			 setEndNodeName(const String& endNodeName) { mEndNodeName = endNodeName; }
		inline const String& getEndNodeName() const { return mEndNodeName; }

		inline void			 setEndOffset(Vector3 endOffset) { mEndOffset = endOffset; }
		inline Vector3		 getEndOffset() const { return mEndOffset; }

		inline void			 setFlyScale(Real flyScale) { mFlyScale = flyScale; }
		inline Real			 getFlyScale() const { return mFlyScale; }

		inline void			 setFlySpeed(Real flySpeed) { mFlySpeed = flySpeed; }
		inline Real			 getFlySpeed() const { return mFlySpeed; }

		inline void			 setFlyTime(Real flyTime) { mFlyTime = flyTime; }
		inline Real			 getFlyTime() const { return mFlyTime; }

		inline void			 setFlyType(unsigned int flyType) { mFlyType = flyType; }
		inline unsigned int	 getFlyType() const { return mFlyType; }

		inline void			 setFlyDirection(unsigned int flyDirection) { mFlyDirection = flyDirection; }
		inline unsigned int  getFlyDirection() const { return mFlyDirection; }

		inline void			 setFlyFade(bool fade) { mFlyFade = fade; }
		inline bool			 getFlyFade() const { return mFlyFade; }

		inline void			 setFlyStayTime(float time) { mFlyStayTime = time; }
		inline float		 getFlyStayTime() { return mFlyStayTime; }

		inline void			 setFlyDeltaHeight(float height) { mFlyDeltaHeight = height; }
		inline float		 getFlyDeltaHeight() const { return mFlyDeltaHeight; }

		inline void			 setAttackEnable(bool bAttack) { mbAttack = bAttack; }
		inline bool			 getAttackEnable() const { return mbAttack; }

		inline void			 setAttackEffectName(const String& attackEffectName) { mAttackEffectName = attackEffectName; }
		inline const String& getAttackEffectName() const { return mAttackEffectName; }

		inline void			 setTargetSize(float targetSize)  { mTargetSize = targetSize; }
		inline float		 getTargetSize() const { return mTargetSize; }

		inline void			 setChangeSize(bool changeSize) { mChangeSize = changeSize; }
		inline bool			 getChangeSize() const { return mChangeSize; }

		inline void			 setAttackNodeName(const String& attackNodeName) { mAttackNodeName = attackNodeName; }
		inline const String& getAttackNodeName() const { return mAttackNodeName; }

		inline void			 setAttackOffset(Vector3 attackOffset) { mAttackOffset = attackOffset; }
		inline Vector3		 getAttackOffset() const { return mAttackOffset; }

		inline void			 setAttackScale(Real attackScale) { mAttackScale = attackScale; }
		inline Real			 getAttackScale() const { return mAttackScale; }

		inline void			 setFallowTarget(bool fallowTarget) { mFallowTarget = fallowTarget; }
		inline bool			 getFallowTarget() const { return mFallowTarget; }

		inline void			 setGroundEnable(bool enable) { mbGround = enable; }
		inline bool			 getGroundEnable() const { return mbGround; }

		inline void			 setGroundEffectName(const String& groundEffectName) { mGroundEffectName = groundEffectName; }
		inline const String& getGroundEffectName() const { return mGroundEffectName; }

		inline void			 setGroundNodeName(const String& groundNodeName) { mGroundNodeName = groundNodeName; }
		inline const String& getGroundNodeName() const { return mGroundNodeName; }

		inline void			 setGroundOffset(Vector3 groundOffset) { mGroundOffset = groundOffset; }
		inline Vector3		 getGroundOffset() const { return mGroundOffset; }

		inline void			 setGroundScale(Real groundScale) { mGroundScale = groundScale; }
		inline Real			 getGroundScale() const { return mGroundScale; }

		inline void			 setGroundTarget(bool groundTarget) { mGroundTarget = groundTarget; }
		inline bool			 getGroundTarget() const { return mGroundTarget; }

		inline void			 setChangeTargetColor(bool enable) { mbChangeTargetColor = enable; }
		inline bool			 getChangeTargetColor() const { return mbChangeTargetColor; }

		inline void			 setChangeColorNum(int num) { mChangeColorNum = num;}
		inline int			 getChangeColorNum() const { return mChangeColorNum; }

		inline void			 setBeginColor(const Color& color) { mBeginColor = color; }
		inline Color		 getBeginColor() const { return mBeginColor; }

		inline void			 setTargetColor(int index, const Color& color) { LordAssert(index>=0 && index<MAXCOLORNUM); mTargetColor[index] = color; }
		inline Color		 getTargetColor(int index) const { LordAssert(index>=0 && index<MAXCOLORNUM); return mTargetColor[index]; }

		inline void			 setTargetTime(int index, Real time) { LordAssert(index>=0 && index<MAXCOLORNUM); mTargetTime[index] = time; }
		inline Real			 getTargetTime(int index) { LordAssert(index>=0 && index<MAXCOLORNUM); return mTargetTime[index]; }

		inline void			 setExposureMultiple  (int index, Real exposure) { LordAssert(index>=0 && index<MAXCOLORNUM); mExposureMultiple[index] = exposure; }
		inline Real			 getExposureMultiple(int index) { LordAssert(index>=0 && index<MAXCOLORNUM); return mExposureMultiple[index]; }

		inline void			 setAttackedSoundEnable(bool enable) { mbHasAttackedSound = enable; }
		inline bool			 getAttackedSoundEnable() const { return mbHasAttackedSound; }

		inline void			 setAttackedSoundName(int attackedSoundName) { mAttackedSoundName = attackedSoundName; }
		inline int			 getAttackedSoundName() const { return mAttackedSoundName; }

		inline void			 setAttackedSoundVolume(Real fVolume) { mfAttackedSoundVolume = fVolume; }
		inline Real			 getAttackedSoundVolume() const { return mfAttackedSoundVolume; }

		inline void			 setBroadcastAttackedSound(bool enable) { mbBroadcastAttackedSound = enable; }
		inline bool			 getBroadcastAttackedSound() const { return mbBroadcastAttackedSound; }

		inline void			 setBroadcastAttackedSoundVolume(Real fVolume) { mfBroadcastAttackedSoundVolume = fVolume; }
		inline Real			 getBroadcastAttackedSoundVolumee() const { return mfBroadcastAttackedSoundVolume; }

		inline void			 setAttackedSoundMinDistance(Real fDistance) { mfAttackedSoundMinDistance = fDistance; }
		inline Real			 getAttackedSoundMinDistance() const { return mfAttackedSoundMinDistance; }

		inline void			 setAttackedSoundMaxDistance(Real fDistance) { mfAttackedSoundMaxDistance = fDistance; }
		inline Real			 getAttackedSoundMaxDistance() const { return mfAttackedSoundMaxDistance; }

		inline void			 setFlySoundEnable(bool enable) { mbHasFlySound = enable; }
		inline bool			 getFlySoundEnable() const { return mbHasFlySound; }

		inline void			 setFlySoundName(int flySoundName) { mFlySoundName = flySoundName; }
		inline int			 getFlySoundName() const { return mFlySoundName; }

		inline void			 setFlySoundVolume(Real fVolume) { mfFlySoundVolume = fVolume; }
		inline Real			 getFlySoundVolume() const { return mfFlySoundVolume; }

		inline void			 setBroadcastFlySoundEnable(bool enable) { mbBroadcastFlySound = enable; }
		inline bool			 getBroadcastFlySoundEnable() const { return mbBroadcastFlySound; }

		inline void			 setBroadcastFlySoundVolume(Real fVolume) { mfBroadcastFlySoundVolume = fVolume; }
		inline Real			 getBroadcastFlySoundVolume() const { return mfBroadcastFlySoundVolume; }

		inline void			 setFlySoundMinDistance(Real fDistance) { mfFlySoundMinDistance = fDistance; }
		inline Real			 getFlySoundMinDistance() const { return mfFlySoundMinDistance; }

		inline void			 setFlySoundMaxDistance(Real fDistance) { mfFlySoundMaxDistance = fDistance; }
		inline Real			 getFlySoundMaxDistance() const { return mfFlySoundMaxDistance; }

		inline void			 setCameraShakeEnble(bool enable) { mbTargetCameraShake = enable; }
		inline bool			 getCameraShakeEnble() const { return mbTargetCameraShake; }

		inline void			 setCameraShakeBeginTime(Real beginTime) { mTargetCameraShakeBeginTime = beginTime; }
		inline Real			 getCameraShakeBeginTime() const { return mTargetCameraShakeBeginTime; }

		inline void			 setCameraShakeDuration(Real duration) { mTargetCameraShakeDuration = duration; }
		inline Real			 getCameraShakeDuration() const { return mTargetCameraShakeDuration; }

		inline void			 setCameraShakeScale(Real scale) { mTargetCameraShakeScale = scale; }
		inline Real			 getCameraShakeScale() const { return mTargetCameraShakeScale; }

		inline void			 setSlowDownEnable(bool enable) { mbTargetSlowDown = enable; }
		inline bool			 getSlowDownEnable() const { return mbTargetSlowDown; }

		inline void			 setSlowDownScale(Real scale) { mTargetSlowDownScale = scale; }
		inline Real			 getSlowDownScale() const { return mTargetSlowDownScale; }

		inline void			 setSlowDownBeginTime(Real beginTime) { mTargetSlowDownBeginTime = beginTime; }
		inline Real			 getSlowDownBeginTime() const { return mTargetSlowDownBeginTime; }

		inline void			 setSlowDownDurationTime(Real durationTime) { mTargetSlowDownDurationTime = durationTime; }
		inline Real			 getSlowDownDurationTime() const { return mTargetSlowDownDurationTime; }

		inline void			 setSlowDownEqualizeTime(Real equalizeTime) { mTargetSlowDownEqualizeTime = equalizeTime; }
		inline Real			 getSlowDownEqualizeTime() const { return mTargetSlowDownEqualizeTime; }

		inline void			 setAttNameID(const String& id) {mAttNameID = id ;}
		inline const String& getAttNameID()const {return mAttNameID;}

		inline void			 setSkillName(const String& skillName) {mSkillName = skillName ;}
		inline const String& getSkillName()const {return mSkillName;}

		inline ActorObject*  getTargetActor(){return mpTargetActor; }
		inline ActorObject*  getOriginalActor(){return mpOriginalActor; }

		inline void			 setIsCallBlack(bool isCallBlack){ mbIsCallBack = isCallBlack;}
		inline bool			 getIsCallBlack(){ return mbIsCallBack;}

		inline void			 setIsDirectivity(bool isDirectivity){ mbDirectivity = isDirectivity;}
		inline bool			 getIsDirectivity(){ return mbDirectivity;}

		inline void			 setNeedReplaceATT(bool need){ mbNeedReplaceATT = need;}
		inline bool			 getNeedReplaceATT(){ return mbNeedReplaceATT;}

		inline void			 setReplaceATTIndex(int index){ mReplaceATTIndex = index;}
		inline int			 getReplaceATTIndex(){ return mReplaceATTIndex;}

		//void				 setUnDirectivityNode(SceneNode* pNode);
		SceneNode*           getUnDirectivityNode();

		void	copyAttributesTo(SkillAttack* another);
		void    Init();

		inline void			 SetWoundSkill(const String& name) { mWoundSkill = name; }
		inline const String& GetWoundSkill() { return mWoundSkill; }

		inline void			 SetCallBackIndex(int index) 
		{
			mCallbackIndex = index;
			LordAssert(mCallbackIndex > 0);
		}
		inline int			 GetCallBackIndex() { return mCallbackIndex; }

		inline void			 SetCallBackNum(int num) 
		{
			mCallbackNum = num;
			LordAssert(mCallbackIndex <= mCallbackNum);
		}
		inline int			 GetCallBackNum() const { return mCallbackNum; }

		inline void			 SetUserData(int _userdata) { mUserData = _userdata; }
		inline int			 GetUserData() const { return mUserData; }

		inline void			 SetUserData2(int _userdata) { mUserData2 = _userdata; }
		inline int			 GetUserData2() const { return mUserData2; }
		
	protected:
		Vector3 getBeginPosition();
		Vector3 getEndPosition();
		void    InitFlyParam();
		void    updateFlyingPosition(Real slerp);
		void    caculateMissPosition(); 

	private:
		static const int		MAXCOLORNUM = 4;
		static int				skillEffectCount;
	protected:
		bool					mbPause;
		bool					mbMissed;
		bool					mbFly;
		bool					mUseFlyDirection;
		bool					mFlyFade;
		bool					mbAttack;
		bool					mChangeSize;
		bool					mFallowTarget;
		bool					mbGround;
		bool					mGroundTarget;
		bool					mbChangeTargetColor;
		bool					mbHasAttackedSound;
		bool					mbBroadcastAttackedSound;
		bool					mbAttackedSoundPlayed;
		bool					mbHasFlySound;
		bool					mbBroadcastFlySound;
		bool					mbTargetCameraShake;
		bool					mbTargetSlowDown;
		bool					mbPlayWoundAction;
		bool					mbIsCallBack;
		bool                    mbDirectivity;
		bool                    mbNeedReplaceATT;

		SkillAttackState		mState;

		unsigned int			mFlyDirection;
		unsigned int			mFlyType;
		unsigned int			mChangeColorNum;

		int						mCurrentFlyState;
		int                     mReplaceATTIndex;
		int						mCallbackIndex;
		int						mCallbackNum;
		int						mUserData; // 默认第1个userdata表示回调的类型，默认是0表示通用的处理
		int						mUserData2;	// 第二个参数用作数据key值，所以默认是-1
		int						mAttackedSoundName;
		int						mFlySoundName;

		float					mCurrentFlyTime;
		float                   mCurrentCameraShakeTime;
		float					mCurrentSlowDownTime;
		float					mFlyParam[6];
		float					mFlyStayTime;
		float					mFlyDeltaHeight;	//用于抛物线的高度	
		float					mTargetSize;

		Real					mFlyScale;
		Real					mFlySpeed;
		Real					mInnerFlyTime;
		Real					mFlyTime;
		Real					mAttackScale;
		Real					mGroundScale;
		Real					mTargetTime[MAXCOLORNUM];
		Real					mExposureMultiple[MAXCOLORNUM];
		Real					mInnerChangeColorTime;
		Real					mfAttackedSoundVolume;
		Real					mfBroadcastAttackedSoundVolume;
		Real					mfAttackedSoundMinDistance;
		Real					mfAttackedSoundMaxDistance;
		Real					mfFlySoundVolume;
		Real					mfBroadcastFlySoundVolume;
		Real					mfFlySoundMinDistance;
		Real					mfFlySoundMaxDistance;
		Real					mTargetCameraShakeBeginTime;
		Real					mTargetCameraShakeScale;
		Real					mTargetCameraShakeDuration;
		Real					mTargetSlowDownScale;
		Real					mTargetSlowDownBeginTime;
		Real					mTargetSlowDownDurationTime;
		Real					mTargetSlowDownEqualizeTime;

		ActorObject*			mpTargetActor;
		ActorObject*            mpOriginalActor;
		EffectSystem*			mpFlyEffectSystem;			
		EffectSystem*			mpAttackEffectSystem;	
		EffectSystem*			mpGroundEffectSystem;
		SceneNode*				mpAttackSceneNode;
		SceneNode*				mpFlySceneNode;
		SceneNode*				mpGroundSceneNode;
		SceneNode*				mEndNode;
		SceneNode*				mAttackNode;
		SceneNode*				mGroundNode;
		SceneNode*				mpUnDirectivityNode;

		Vector3					mOriginalActorPos;
		Vector3					mBeginFlyingPosition;
		Vector3					mOldFlyPosition;
		Vector3					mMissPosition;
		Vector3					mBeginPosition;
		Vector3					mBeginOffset;
		Vector3					mEndOffset;
		Vector3					mAttackOffset;
		Vector3					mGroundOffset;

		Color					mBeginColor;
		Color					mTargetColor[MAXCOLORNUM];
		Quaternion				mAttackGroundYaw;

		String					mFlyEffectName;	
		String					mBeginNodeName;
		String					mEndNodeName;	
		String					mAttackEffectName;	
		String					mAttackNodeName;	
		String					mGroundEffectName;
		String					mGroundNodeName;
		String					mAttNameID;
		String                  mSkillName;
		String					mWoundSkill;
	};

	class  LORD_CORE_API SkillAttackManager : public Singleton<SkillAttackManager>, public ObjectAlloc
	{
	public:
		static SkillAttackManager* Instance();

	public:
		SkillAttackManager();

		~SkillAttackManager();

		SkillAttack*	createInstance(const String& attName);
		void			destroyInstance(SkillAttack* att);

		bool AddSkillAttackTemplate(const String& attName);
		bool DelSkillAttackTemplate(const String& attName);
		void DelAllSkillAttackTemplate();

	protected:
		typedef map<String, SkillAttack* >::type SkillAttackMap;
		typedef map<String, SkillAttack* >::type::iterator SkillAttackItor;

		SkillAttackMap mSkillAttackMap;
	};
}
#endif