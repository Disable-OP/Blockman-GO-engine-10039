/********************************************************************
filename: 	AnimKeyFrame.h
file path:	dev\engine\Src\Core\Anim\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ANIMKEYFRAME_H__
#define __LORD_ANIMKEYFRAME_H__

#include "Core.h"

namespace LORD
{
	class Animation;
	
	class LORD_CORE_API AnimKeyFrame: public ObjectAlloc
	{	
	public:
		AnimKeyFrame(Animation* pAnim, ui32 time);
		virtual ~AnimKeyFrame();
		
	public:
		virtual void createAnimData(Dword boneCount);
		void setAnimData(Dword handle, const Quaternion& ori, const Vector3& pos)
		{
			m_KeyOrientation[handle] = ori;
			m_KeyPosition[handle] = pos;
		}

		Animation* getAnim() const
		{
			return m_pAnim;
		}

		ui32 getTime() const
		{
			return m_time;
		}
		//virtual const Matrix4&		getAnimMatrix(uint idx) const;
		const Quaternion&	getAnimQuaternion(uint idx) const {	return m_KeyOrientation[idx]; }
		const Vector3&		getAnimTranslate(uint idx) const { return m_KeyPosition[idx]; }

		const Quaternion* getAnimQuaternionPtr() const { return m_KeyOrientation; }
		const Vector3* getAnimTranslatePtr() const { return m_KeyPosition; }
	protected:
		Animation*		m_pAnim;
		ui32			m_time;
		//Matrix4*		m_KeyMatrix;
		Quaternion*		m_KeyOrientation;
		Vector3*		m_KeyPosition;
	};
}

#endif