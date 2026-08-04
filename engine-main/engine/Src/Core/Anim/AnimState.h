/********************************************************************
filename: 	AnimState.h
file path:	dev\engine\Src\Core\Anim\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ANIMSTATE_H__
#define __LORD_ANIMSTATE_H__

namespace LORD
{
	class Animation;

	class LORD_CORE_API AnimState : public ObjectAlloc
	{
	public:
		AnimState(Animation* pAnim);
		virtual ~AnimState();

	public:
		virtual void			enable(bool bEnable);
		virtual void			enableLoop(bool bEnable);
		virtual void			setLength(ui32 length);
		virtual void			setTimePosition(ui32 timePos);
		virtual void			setTimeScale(float timeScale) { m_timeScale = timeScale; }

		virtual Animation*		getAnim() const;
		virtual bool			isEnable() const;
		virtual bool			isLoop() const;
		virtual ui32			getLength() const;
		virtual ui32			getTimePosition() const;
		virtual uint			getCurrentFrameIndex() const;
		virtual uint			getNextFrameIndex() const;
		virtual AnimKeyFrame*	getCurrentAnimKeyFrame() const;
		virtual AnimKeyFrame*	getAnimKeyFrame(uint index) const;

		virtual void			frameMove(ui32 deltaTime);

	protected:
		Animation*			m_pAnim;
		bool				m_bEnable;
		bool				m_bLoop;
		ui32				m_length;
		ui32				m_timePos;
		uint				m_nFrame;
		float				m_timeScale;
	};
}

#endif