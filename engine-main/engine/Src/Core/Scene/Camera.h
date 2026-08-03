/********************************************************************
filename: 	Camera.h
file path:	dev\engine\Src\Core\Scene\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_CAMERA_H__
#define __LORD_CAMERA_H__

#include "Core.h"
#include "Math/Quaternion.h"
#include "Geom/Frustum.h"

#define LORD_CAMERA_SUPPORT_SCREENOFFSET

namespace LORD
{
	class LORD_CORE_API Camera: public ObjectAlloc
	{
	public:
		enum ProjMode
		{
			PM_PERSPECTIVE,
			PM_ORTHO,
		};

	public:	
		Camera(ProjMode mode = PM_PERSPECTIVE, bool isFixedYaw = true );
		virtual ~Camera();

	public:
		virtual void			setPosition(const Vector3& pos);
		virtual void			setTarget(const Vector3& target);
		virtual void			setDirection(const Vector3& dir);
		virtual void			setUp(const Vector3& vUp);

		virtual void			yaw(Real rad);
		virtual void			pitch(Real rad);
		virtual void			roll(Real rad);
		virtual void			rotate(const Vector3& vAxis, Real rad);
		virtual void			move(const Vector3& offset);
		/*const Vector3&		getPosition() const;
		Vector3*			getPositionPtr();
		const Vector3&		getDirection() const;
		const Quaternion&	getOritation() const;
		const Vector3&		getRight() const;
		const Vector3&		getUp() const;
		const Matrix4&		getViewMatrix() const;
		const Matrix4&		getProjMatrix() const;
		const Matrix4&		getViewProjMatrix() const;*/
		const Vector3& getPosition() const
		{
			return m_position;
		}

		Vector3 luaGetPosition()
		{
			return m_position;
		}

		inline Vector3* getPositionPtr()
		{
			return &m_position;
		}

		inline const Quaternion& getOritation() const
		{
			return m_oritation;
		}

		inline const Vector3& getDirection() const
		{
			return m_dir;
		}


		inline Vector3 luaGetDirection()
		{
			return m_dir;
		}

		inline const Vector3& getRight() const
		{
			return m_right;
		}

		inline Vector3 luaGetRight() const
		{
			return m_right;
		}

		inline const Vector3& getUp() const
		{
			return m_up;
		}

		inline Vector3 luaGetUp() const
		{
			return m_up;
		}

		inline const Matrix4& getViewMatrix() const
		{
			return m_matView;
		}
		
		inline const Matrix4& getProjMatrix() const
		{
			return m_matProj;
		}

		inline const Matrix4& getProjMatrix2() const
		{
			return m_matProj2;
		}

		inline const Matrix4& getViewProjMatrix() const
		{
			return m_matVP;
		}

		inline const Matrix4& getViewProjMatrix2() const
		{
			return m_matVP2;
		}

		void			setProjectionMode(ProjMode mode);
		ProjMode		getProjectionMode() const;

		void			setFov(Real fov);
		void			setFov2(Real fov);
		void			setWidth(uint width);
		void			setHeight(uint height);
		void			setNearClip(Real nearClip);
		void			setFarClip(Real farClip);
		Real			getFov() const;
		Real			getFov2() const;
		uint			getWidth() const;
		uint			getHeight() const;
		Real			getNearClip() const;
		Real			getFarClip() const;
		const Frustum&	getFrustum() const;

		void			getCameraRay(Ray& ray, const Vector2& screenPos);

		void			update();

#ifdef LORD_CAMERA_SUPPORT_SCREENOFFSET
		void			setScreenOffset( float xOffset, float yOffset);
#endif
		inline	void	setFixedYawAxis(bool fixed) {m_bFixedYawAxis = fixed;}
	protected:
		bool			m_bFixedYawAxis;
		bool			m_bNeedUpdateView;
		bool			m_bNeedUpdateProj;

		ProjMode		m_projMode;

		uint			m_width;
		uint			m_height;

		Real			m_fov;
		Real			m_fov2;
		Real			m_aspect;
		Real			m_nearClip;
		Real			m_farClip;

		Vector3			m_position;
		Vector3			m_dir;
		Vector3			m_up;
		Vector3			m_right;
		//Vec3			m_zAxis;
		Vector3			m_fixedYawAxis;
		Quaternion		m_oritation;
		Matrix4			m_matView;
		Matrix4			m_matProj;
		Matrix4			m_matProj2;
		Matrix4			m_matVP;
		Matrix4			m_matVP2;

#ifdef LORD_CAMERA_SUPPORT_SCREENOFFSET
		Matrix4			m_matScreenOffset;		// ÆÁÄ»¿Õ¼äÆ«ÒÆ¾ØÕó
#endif

		Frustum			m_frustum;
	};

}

#endif