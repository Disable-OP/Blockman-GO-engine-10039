/********************************************************************
filename: 	Animation.h
file path:	dev\engine\Src\Core\Anim\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ANIM_H__
#define __LORD_ANIM_H__

#include "Core.h"
#include "Resource/Resource.h"

namespace LORD
{
	class LORD_CORE_API Animation: public Resource
	{
		friend class AnimationManager;

	public:
		Animation(const String& name, ResourceType rt = RESOURCE_TYPE_NORMAL);
		virtual ~Animation();

		typedef vector<AnimKeyFrame*>::type	AnimKeyFrameList;

	public:
		//static Anim*					LoadFromFile(const String &filename, const String &animName);

		virtual AnimKeyFrame*			createAnimKeyFrame(ui32 time);
		virtual void					removeAnimKeyFrameByIdx(uint idx);
		virtual void					removeAnimKeyFrames();

		virtual uint					getFrameCount() const;
		virtual ui32					getLength() const;
		virtual uint					getFPS() const;
		virtual bool					hasScaling() const;
		virtual AnimKeyFrame*			getAnimKeyFrame(ui32 time) const;
		virtual AnimKeyFrame*			getAnimKeyFrameByIdx(uint idx) const;
		virtual const AnimKeyFrameList&	getAnimKeyFrameList() const;
		virtual uint					getFrameIndex(ui32 time) const;

	protected:
		virtual size_t					calculateSize() const;
		virtual bool					prepareImpl();
		virtual void					unprepareImpl();
		virtual bool					loadImpl();
		virtual void					unloadImpl();

	protected:
		uint				m_frameCount;
		ui32				m_length;
		uint				m_fps;
		bool				m_bHasScaling;
		AnimKeyFrameList	m_keyFrames;
		DataStream*			m_pAnimStream;
	};
}

#endif