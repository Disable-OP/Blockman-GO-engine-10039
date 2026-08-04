/********************************************************************
filename: 	EffectSystem.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EFFECT_SYSTEM_BASE_H__
#define __LORD_EFFECT_SYSTEM_BASE_H__

#include "Core.h"
#include "EffectRender.h"
#include "EffectIElement.h"
#include "Resource/LordThread.h"

namespace LORD
{
	class EffectCalcEvent;
	class EffectPrepareEvent;
	class EffectDestroyEvent;

	class LORD_CORE_API EffectSystem : public ObjectAlloc, public IElement
	{
		friend class EffectPrepareEvent;
		friend class EffectDestroyEvent;
		friend class EffectCalcEvent;
		friend class EffectSystemManager;
	public:
		//interface
		virtual void getPropertyList(PropertyList& list);
		virtual bool getPropertyType(const String& name, PropertyType& type);
		virtual bool getPropertyValue(const String& name, String& value);
		virtual bool setPropertyValue(const String& name,const String& value);

	public:
		//default property
		static const i32	DEFAULT_TIME_STEP;
		static const float	DEFAULT_SCALE_VELOCITY;
		static const float	DEFAULT_SCALE_TIME;

		//effect state
		enum EffectSystemState
		{
			PSS_CREATED,	
			PSS_PREPARING,	
			PSS_PREPARED,	
			PSS_STARTED,	
			PSS_DELAYED,
			PSS_STOPPED,	
			PSS_PAUSED		
		};

		//delay queue
		enum DelayQueueType
		{
			DQT_START,
			DQT_PAUSE,
			DQT_STOP,
			DQT_UNPREPARE
		};

		struct DelayQueueItem
		{
			DelayQueueType mType;
			i32 mTime;
		};

		// typedef
		typedef vector<EffectLayer*>::type			EffectLayerList;
		typedef EffectLayerList::iterator			EffectLayerIterator;
		typedef EffectLayerList::const_iterator     EffectLayerConstIterator;

		typedef vector<DelayQueueItem>::type		EffectDelayQueue;
		typedef EffectDelayQueue::iterator			EffectDelayIterator;

		typedef vector<ui32>::type				UbiLayerFallowPair;

		// constructor
		EffectSystem(const String& name,bool isUIeffect = false, bool isSpecial = false);
		virtual ~EffectSystem(void);
		EffectSystem& operator=(const EffectSystem& ps);

		// name time.
		inline const String& getName() const { return mName; }
		void setTemplateName(const String& templateName) { mTemplateName = templateName; }
		inline const String& getTemplateName() const { return mTemplateName; }
		inline i32 getTimeElapsedSinceStart(void) const { return mTimeElapsedSinceStart; };
		float getEffectTimeLength(void) const;

		// layer
		EffectLayer* createLayer(EFFECT_LAYER_TYPE layerType);
		virtual void addLayer(EffectLayer* layer);
		EffectLayer* getLayer(size_t index) const;
		EffectLayer* getLayer(const String& name) const;
		size_t getNumLayers(void) const;
		void destroyLayer(EffectLayer* layer);
		void destroyLayer(size_t index);
		void destroyAllLayers(void);

		// inilised and update
		virtual void prepare_sys(bool bSingleThread = false );
		virtual void prepare_res();
		virtual void prepare_io_sys(void);  // stream threading
		void prepareLayer(void);			// main threading
		void unprepare_sys(void);
		void unprepareLayer(void);

		void importData(DataStream* datastream);
		void exportData(DataStream* datastream);

		virtual void _update_sys(i32 timeElapsed);
		virtual void _update(i32 timeElapsed);
		virtual void _preUpdate(i32 timeElapsed);
		virtual int _updateLayers(i32 timeElapsed);
		virtual void _updateRenderQueue();

		// operate
		void start(void);
		void stop(bool isDelay=true);
		void pause(void);
		void resume(void);
		void startAndPause(i32 time);
		virtual void stepIn(i32 time);
		EffectSystemState getState(void) const { return mState; }
		bool isPlaying(void) { return (PSS_STARTED == mState); }
		bool isPaused(void) { return PSS_PAUSED == mState; }
		bool isStoped(void) { return PSS_STOPPED == mState; }

		// about render system and scene manager.
		void	setCurrentCamera(Camera* camera);
		Camera* getCurrentCamera();
		inline const Vector3& getCameraPosInEffect() const { return mCameraPosInEffect; }
		inline const Quaternion& getCameraOriInEffect() const { return mCameraOriInEffect; }
		inline SceneNode* getEffectSceneNode() { return mParentNode; }
		void	setParentSceneNode(SceneNode* pNode);
		inline const Box& getBoundingBox() const { return mAABB; };

		// only the timescale control, no position, no oritation, no scale, no color. 
		// pos, ori and scale use the parent bone to control
		// color and alpha use the layer to set.
		inline float getScaleTime(void) const
		{
			return mEffectSystemScaleTime;
		}
		//-----------------------------------------------------------------------
		inline void setScaleTime(float scaleTime)
		{
			mEffectSystemScaleTime = scaleTime;
		}

		// this is to reduce the keyframe cal cost. use more.
		const LORD::vector<ui32>::type& getFallowPair(void) const;
		String getFallowPairString(void) const;
		void setFallowPair(const String& pairString);
		void setFallowPair(const LORD::vector<ui32>::type& pairVector);

		/** external params */
		void setExtenalParam(const Vector3& p1, const Vector3& p2);

		/** special params */
		void setSpecialParam(float p1, float p2);

		/** parabolic params */
		void setParabolicParam(const Vector3& p1, const Vector3& p2, const Vector3& p3);

		// editor about.
		EffectLayerList getLayers(void) const;
		bool getShowVisualHelp(void) const;
		void setShowVisualHelp(bool show);
		void changeLayerPosition(size_t p1, size_t p2);
		void caculateAligenBox();
		bool getVisible(void) const;
		void setVisible(bool visible);
		void setIsUIEffect(bool isUIeffect){mbIsUIEffect = isUIeffect;}
		bool getIsUIEffect(){return mbIsUIEffect;}
		void setIsChild() { mbIsChild = true; }
		bool isChild() { return mbIsChild;  }

		void setSpecialRender(bool bSpecial) { mbIsSpecial = bSpecial; }
		bool IsSpecialRender() const { return mbIsSpecial; }

		void setLayersViewSpace(bool isViewSpace);

	protected:
		bool effectVisible();
		void handleDelayQueue();

	protected:
		// effect name, resource name
		String		mName;
		String		mTemplateName;

		// playing state about
		EffectSystemState mState;
		i32			mTimeElapsedSinceStart;
		i32			mDelayTimeTick;		
		bool		mIsPreparing;
		bool		mIsDestroying;
		float		mEffectSystemScaleTime;

		// render system need.
		Box			mAABB;
		Camera*		mCurrentCamera;
		Vector3		mCameraPosInEffect;
		Quaternion		mCameraOriInEffect;
		SceneNode*	mParentNode;

		// core data.
		EffectLayerList mLayers;

		// stream thread about.
		EffectDelayQueue mDelayQueue;	
		UbiLayerFallowPair mFallowPair;

		//editor about.
		bool		mbEffectSystemShowVisualHelp;
		bool		mbIsVisible;
		bool        mbIsUIEffect;
		bool		mbIsChild;
		bool		mbIsSpecial;

		EffectPrepareEvent* m_pPrepareEvent;
		EffectDestroyEvent* m_pDestroyEvent;

		EffectCalcEvent* m_pCalcEvent;
	};


	class EffectPrepareEvent : public ThreadEvent
	{
	public:
		EffectPrepareEvent(EffectSystem* pEffect);
		virtual ~EffectPrepareEvent();

		virtual bool ProcessEvent();

		virtual bool RespondEvent();

		virtual bool GetRepeatRespondIfFail()	{ return true; }
		virtual bool IsAutoDestroy()			{ return true; }

	protected:		
		EffectSystem*	mpEffect;
	};

	class EffectDestroyEvent : public ThreadEvent
	{
	public:
		EffectDestroyEvent(EffectSystem* pEffect);
		virtual ~EffectDestroyEvent();

		virtual bool RespondEvent() ;

		virtual bool ProcessEvent()	{ return true; /* nothing to do...*/ }

		virtual bool GetRepeatRespondIfFail()	{ return true; }
		virtual bool IsAutoDestroy()			{ return true; }

	protected:
		EffectSystem*	mpEffect;
		bool			mbRespond;
	};


	class EffectCalcEvent : public ThreadEvent
	{
	public:
		EffectCalcEvent(EffectSystem* pEffect);
		virtual ~EffectCalcEvent();

		virtual bool RespondEvent() { return true; };

		virtual bool ProcessEvent();
		
		virtual void SetInCalcThread(bool inThread);
		virtual bool GetInCalcThread() const;
		virtual void ChangeCalcateState(int state);

		virtual void SetEscapeTime(int _escapeTime);

		virtual void RestoreEvent();
	protected:
		EffectSystem*	m_pEffect;
		bool			m_InCalcThread;
		int				m_CalcateState;	// 0 需要计算 ；1 不需要计算
		int				m_EscapeTime;
	};
}
#endif
