/********************************************************************
filename: 	EffectLayerParticles.h
file path:	dev\engine\Src\Core\Effect\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __EFFECT_LAYER_PARTICLES_H__
#define __EFFECT_LAYER_PARTICLES_H__

#include "EffectLayer.h"
#include "EffectParticleControllerSet.h"

namespace LORD
{
	class LORD_CORE_API EffectLayerParticles : public EffectLayer
	{
	public:
		static const i32	DEFAULT_PT_QUOTA;			
		static const bool	DEFAULT_PT_3DBILLBOARD;		
		static const bool	DEFAULT_PT_NOTRANSSCALE;	
		static const i32    DEFAULT_PT_EMITTERRATE;    
		static const float  DEFAULT_PT_EMITTERANGLE;   
		static const float  DEFAULT_PT_PTSPEED;	
		static const float  DEFAULT_PT_MAXPTSPEED;
		static const float  DEFAULT_PT_PTADDSPEED;
		static const i32    DEFAULT_PT_PTLEAVETIME;
		static const i32    DEFAULT_PT_MAXPTLEAVETIME;
		static const Color	DEFAULT_PT_MINCOLOR;
		static const Color	DEFAULT_PT_MAXCOLOR;
		static const float  DEFAULT_PT_MINSCALE;
		static const float  DEFAULT_PT_MAXSCALE;
		static const float  DEFAULT_PT_MINANGLE;
		static const float  DEFAULT_PT_MAXANGLE;
		static const bool	DEFAULT_PT_FALLOWPARENT;
		static const bool	DEFAULT_PT_EMITTERINSURFACE;

	public:
		virtual void getPropertyList(PropertyList& list);
		virtual bool getPropertyType(const String& name, PropertyType& type);
		virtual bool getPropertyValue(const String& name, String& value);
		virtual bool setPropertyValue(const String& name,const String& value);

	public:
		EffectLayerParticles();
		virtual ~EffectLayerParticles();

		virtual void prepare();

		virtual void unprepare();

		virtual void emitterParticle(EffectParticle* particle);

		virtual void _notifyStart(void);
		virtual void _notifyReStart(void);
		virtual void _notifyStop();

		virtual void _notifyShowVisualHelp(bool show);

		virtual int _update(i32 time);

		virtual void _updateRenderQueue();
		void renderBillboard(const EffectParticle* billboard, EffectVertexFmt* pVertexPtr);

		virtual void copyAttributesTo(EffectParticle* layer);
		virtual void exportData(DataStream* pStream, int version);
		virtual void importData(DataStream* pStream, int version);
		virtual void mergeAligenBox(Box& aabb);

		void		 clearPool();
		void		 createPool(size_t size);

		EffectParticleControllerSet* createParticleControllerSet();
		void   setParticleControllerSet(EffectParticleControllerSet* particleControllerSet);
		EffectParticleControllerSet* getParticleControllerSet() const;
		void destroyParticleControllerSet();

	protected:
		i32				mParticleQuota;
		float			mWidth;
		float			mHeight;
		bool			mb3dBillboard;
		bool			mbPerpendicular;
		bool			mb3DBillboardTrans;
		bool			mbNoTransScale;
		bool			mbNoScaleWidth;
		bool			mbNoScaleHeight;
		float			mCenterOffsetW;
		float			mCenterOffsetH;
		i32				mEmitterRate;
		float			mEmitterAngle;
		float			mParticleSpeed;
		float			mParticleMaxSpeed;
		float			mParticleAddSpeed;
		i32				mParticleLeaveTime;
		i32				mParticleMaxLeaveTime;
		Color			mMinColor;
		Color			mMaxColor;
		float			mMinScale;
		float			mMaxScale;
		float			mMin2dAngle;
		float			mMax2dAngle;
		bool			mFallowParent;
		bool			mEmitterInSurface;
		bool			mbStopEmitter;

		typedef list<EffectParticle* >::type ParticlePool;
		typedef list<EffectParticle* >::type::iterator ParticlePoolItor;
		ParticlePool		mParticlePoolInUse;
		ParticlePool		mParticlePoolFree;

		EffectParticleControllerSet* mParticleControllerSet;

		i32				mEmitterTimeStep;		
		i32				mEmitterTime;			
		i32				mInnerParticleQuota;  
	};
}

#endif