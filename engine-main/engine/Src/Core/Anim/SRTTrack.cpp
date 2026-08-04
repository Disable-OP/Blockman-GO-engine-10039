#include "Core.h"
#include "SRTTrack.h"

namespace LORD
{
	SRTTrack::SRTTrack(Dword handle)
		:AnimTrack(handle)
		,m_bUpdate(true)
	{
		m_scaling.one();
		m_rotation.identity();
		m_translation.zero();
	}

	SRTTrack::~SRTTrack()
	{
	}

	void SRTTrack::setScaling(const Vector3& scaling)
	{
		m_scaling = scaling;
		m_bUpdate = true;
	}

	void SRTTrack::setRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
		m_bUpdate = true;
	}

	void SRTTrack::setTranslation(const Vector3& translation)
	{
		m_translation = translation;
		m_bUpdate = true;
	}

	void SRTTrack::setTransformMatrix(const Matrix4& mat)
	{
		m_matTransform = mat;
		m_bUpdate = false;
	}

	void SRTTrack::calcTransformMatrix() const
	{
		m_matTransform.makeScaling(m_scaling);

		Matrix4 matRot;
		m_rotation.toMat4(matRot);
		m_matTransform = m_matTransform * matRot;

		m_matTransform.translate(m_translation);
		m_bUpdate = false;
	}

	const Vector3& SRTTrack::getScaling() const
	{
		return m_scaling;
	}

	const Quaternion& SRTTrack::getRotation() const
	{
		return m_rotation;
	}

	const Vector3& SRTTrack::getTranslation() const
	{
		return m_translation;
	}

	const Matrix4& SRTTrack::getTransformMatrix() const
	{
		if(m_bUpdate)
		{
			calcTransformMatrix();
		}

		return m_matTransform;
	}
}
