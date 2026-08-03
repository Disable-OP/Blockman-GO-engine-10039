/********************************************************************
filename: 	SRTTrack.h
file path:	dev\engine\Src\Core\Anim\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_SRTTRACK_H__
#define __LORD_SRTTRACK_H__

#include "Core.h"
#include "Math/Quaternion.h"
#include "AnimTrack.h"

namespace LORD
{
	class LORD_CORE_API SRTTrack: public AnimTrack
	{
	public:
		SRTTrack(Dword handle);
		virtual ~SRTTrack();

	public:
		virtual void			setScaling(const Vector3& scaling);
		virtual void			setRotation(const Quaternion& rotation);
		virtual void			setTranslation(const Vector3& translation);
		virtual void			setTransformMatrix(const Matrix4& mat);
		virtual void			calcTransformMatrix() const;

		virtual const Vector3&		getScaling() const;
		virtual const Quaternion&		getRotation() const;
		virtual const Vector3&		getTranslation() const;
		virtual const Matrix4&		getTransformMatrix() const;

	protected:
		Vector3			m_scaling;
		Quaternion			m_rotation;
		Vector3			m_translation;
		mutable bool	m_bUpdate;
		mutable Matrix4	m_matTransform;
	};
}
#endif