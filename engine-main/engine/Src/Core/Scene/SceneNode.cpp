#include "Core.h"
#include "SceneNode.h"
#include "SceneManager.h"

namespace LORD
{
	SceneNode::SceneNode()
		: m_pParent(NULL)
		, m_bVisible(true)
		, m_posLocal(Vector3::ZERO)
		, m_ortLocal(Quaternion::IDENTITY)
		, m_sclLocal(Vector3::ONE)
	{
		m_sclWorld = m_sclLocal;
		m_ortWorld = m_ortLocal;
		m_posWorld = m_posLocal;

		m_matWorld = Matrix4::IDENTITY;
		
		needUpdate();
        
        m_children.clear();
	}

	SceneNode::~SceneNode()
	{
	}

	SceneNode* SceneNode::createChild()
	{
		SceneNode* pRet = LordNew SceneNode;
		pRet->m_pParent = this;
		m_children.insert(pRet);
		return pRet;
	}

	void SceneNode::destroyChild(SceneNode* pChild)
	{
		ChildNodeSet::iterator it = m_children.find(pChild);
		if(it != m_children.end())
		{
			if(pChild->m_pParent == this)
			{
				LordDelete pChild;
			}
			m_children.erase(it);
		}
	}

	void SceneNode::destroyAllChildren()
	{
		ChildNodeSet::iterator it = m_children.begin();
		if(it != m_children.end())
		{
			LordDelete (*it);
		}
		m_children.clear();
	}

	void SceneNode::setParent(SceneNode* pParent)
	{
		if(m_pParent)
		{
			ChildNodeSet::iterator it = m_pParent->m_children.find(this);
			if(it != m_pParent->m_children.end())
			{
				m_pParent->m_children.erase(it);
			}
		}

		m_pParent = pParent;

		if(m_pParent)
			m_pParent->m_children.insert(this);

		needUpdate();
	}

	void SceneNode::setVisible(bool bVisible)
	{
		m_bVisible = bVisible;
	}

	void SceneNode::scale(const Vector3& scl)
	{
		m_sclLocal = m_sclLocal * scl;
		
		needUpdate();
	}

	void SceneNode::roll(const Real randian)
	{
		rotate(Vector3::UNIT_Z, randian);
	}

	void SceneNode::pitch(const Real randian)
	{
		rotate(Vector3::UNIT_X, randian);
	}

	void SceneNode::yaw(const Real randian)
	{
		rotate(Vector3::UNIT_Y, randian);
	}

	void SceneNode::rotate(const Vector3& vAxis, const Real randian)
	{
		Quaternion rot(vAxis, randian);
		rotate(rot);
	}

	void SceneNode::rotate(const Quaternion& rot)
	{
		// Normalise quaternion to avoid drift
		Quaternion qnorm = rot;
		qnorm.normalize();

		// Note the order of the mult, i.e. q comes after
		m_ortLocal = m_ortLocal * qnorm;

		needUpdate();
	}

	void SceneNode::translate(const Vector3& d)
	{
		m_posLocal += m_ortLocal * d;

		needUpdate();
	}

	void SceneNode::setLocalScaling(const Vector3& scl)
	{
		m_sclLocal = scl;
		needUpdate();
	}

	void SceneNode::setLocalOrientation(const Quaternion& ort)
	{
		m_ortLocal = ort;
		m_ortLocal.normalize();

		needUpdate();
	}

	void SceneNode::setLocalPosition(const Vector3& pos)
	{
		m_posLocal = pos;
		needUpdate();
	}

	void SceneNode::setWorldOrientation(const Quaternion& ort)
	{
		if(m_pParent)
		{
			Quaternion ortLocal;
			m_pParent->convertWorldToLocalOrientation(ortLocal, ort);
			setLocalOrientation(ortLocal);
		}
		else
		{
			setLocalOrientation(ort);
		}
	}

	void SceneNode::setWorldPosition(const Vector3& pos)
	{
		if(m_pParent)
		{
			Vector3 posLocal;
			m_pParent->convertWorldToLocalPosition(posLocal, pos);
			setLocalPosition(posLocal);
		}
		else
		{
			setLocalPosition(pos);
		}
	}

	SceneNode* SceneNode::getParent() const
	{
		return m_pParent;
	}

	bool SceneNode::isVisible() const
	{
		return m_bVisible;
	}

	const Vector3& SceneNode::getLocalScaling() const
	{
		return m_sclLocal;
	}

	const Quaternion& SceneNode::getLocalOrientation() const
	{
		return m_ortLocal;
	}

	const Vector3& SceneNode::getLocalPosition() const
	{
		return m_posLocal;
	}

	const Vector3& SceneNode::getWorldScaling() const
	{
		return m_sclWorld;
	}

	const Quaternion& SceneNode::getWorldOrientation() const
	{
		return m_ortWorld;
	}

	const Vector3& SceneNode::getWorldPosition() const
	{
		return m_posWorld;
	}

	void SceneNode::setWorldMatrix(const Matrix4& tempMatrix)
	{
		m_matWorld = tempMatrix;
	}
	
	const Matrix4& SceneNode::getWorldMatrix()
	{
		if (m_bMatrixDirty)
		{
			if(m_bModify)
				update();

			m_matWorld.makeScaling(getWorldScaling());

			Matrix4 matRot;
			matRot.fromQuan(getWorldOrientation());
			m_matWorld = m_matWorld * matRot;

			m_matWorld.translate(getWorldPosition());
			m_bMatrixDirty = false;
		}

		return m_matWorld;
	}

	Matrix4* SceneNode::getWorldMatrixPtr()
	{
		return &m_matWorld;
	}

	Matrix4 SceneNode::getInverseWorldMatrix() const
	{
		Matrix4 inverseMat;
		inverseMat.makeTranslation(-m_posWorld);
		Matrix4 matRot;
		Quaternion rot = m_ortWorld;
		rot.conjugate();
		matRot.fromQuan(rot);
		inverseMat = inverseMat * matRot;
		matRot.makeScaling(1.0f/m_sclWorld.x, 1.0f/m_sclWorld.y, 1.0f/m_sclLocal.z);
		inverseMat = inverseMat * matRot;
		return inverseMat;
	}

	void SceneNode::convertWorldToLocalPosition(Vector3& posLocal, const Vector3& posWorld)
	{
		Quaternion ortWorldInv = m_ortWorld;
		ortWorldInv.inverse();
		posLocal = ortWorldInv * (posWorld - m_posWorld) / m_sclWorld;
	}

	void SceneNode::convertWorldToLocalOrientation(Quaternion& ortLocal, const Quaternion& ortWorld)
	{
		Quaternion ortWorldInv = m_ortWorld;
		ortWorldInv.inverse();

		ortLocal = ortWorldInv * ortWorld;
	}

	void SceneNode::needUpdate()
	{
		m_bModify = true;
		m_bMatrixDirty = true;

		ChildNodeSet::iterator it = m_children.begin();
		for(; it!=m_children.end(); ++it)
		{
			(*it)->needUpdate();
		}
	}

	void SceneNode::updateImpl(bool bUpdateChildren)
	{
		if(m_bModify || bUpdateChildren)
		{
			if(m_pParent)
			{
				m_ortWorld = m_pParent->m_ortWorld * m_ortLocal;

				// Update scale
				m_sclWorld = m_pParent->m_sclWorld * m_sclLocal;

				// Change position vector based on parent's orientation & scale
				m_posWorld = m_pParent->m_ortWorld * (m_pParent->m_sclWorld * m_posLocal);
				m_posWorld += m_pParent->m_posWorld;
			}
			else
			{
				m_ortWorld = m_ortLocal;
				m_posWorld = m_posLocal;
				m_sclWorld = m_sclLocal;
			}

			if(bUpdateChildren)
			{
				ChildNodeSet::iterator it = m_children.begin();
				for (; it != m_children.end(); ++it)
				{
					(*it)->update(true);
				}
			}

			m_bModify = false;
			getWorldMatrix();
		}
	}

	void SceneNode::update(bool bUpdateChildren)
	{
		if (m_pParent)
		{
			m_pParent->updateImpl(false);
		}
		updateImpl(bUpdateChildren);
	}

}