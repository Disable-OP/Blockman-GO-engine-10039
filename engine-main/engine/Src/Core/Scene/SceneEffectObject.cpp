#include "Core.h"
#include "SceneEffectObject.h"
#include "Effect/EffectSystemManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Object/Root.h"
#include "Util/QueryObject.h"
#include "Scene/SceneNode.h"

namespace LORD
{
	const float editor_r = 0.15f;

	SceneEffectObject::SceneEffectObject()
		: GameObject()
		, m_pEffectSystem(NULL)
	{
		m_Type = GOT_EFFECT;
		editor_canMirror = true;
		editor_canRotate = true;
		editor_canScale = true;
	}

	SceneEffectObject::~SceneEffectObject()
	{
		_unprepare();
	}

	void SceneEffectObject::_prepare_io()
	{
	}

	void SceneEffectObject::_prepare_res()
	{
	}

	void SceneEffectObject::_prepare_obj()
	{
		m_pEffectSystem = EffectSystemManager::Instance()->createEffectSystem("SceneEffect_"+m_Name, m_Resource);
		if (!m_pEffectSystem)
			return;

		m_ParentNode = SceneManager::Instance()->getRootNode();
		m_SceneNode = m_ParentNode->createChild();
		LordAssert(m_SceneNode);
		m_pEffectSystem->setParentSceneNode(m_SceneNode);
		m_pEffectSystem->prepare_sys();
		m_pEffectSystem->setCurrentCamera(SceneManager::Instance()->getMainCamera());
		m_pEffectSystem->start();
		// ----update scene node
		m_SceneNode->setWorldPosition(m_Position);
		m_SceneNode->setWorldOrientation(m_Oritation);
		m_SceneNode->setLocalScaling(m_Scale);
		m_SceneNode->update();
		updateWorldAABB();

		Scene* scene = SceneManager::Instance()->getCurrentScene();
		LordAssert(scene);
		scene->registerGameObjectToZoom(this);

		m_IOprepared = true;
	}

	void SceneEffectObject::_prepare_editor()
	{
		if (Root::Instance()->isEditorMode())
		{
			Scene* pScene = SceneManager::Instance()->getCurrentScene();
			QueryObjectManager* pManager = pScene->getQueryObjectManager();

			m_pHoverBoxQueryObject = pManager->createQueryObject(m_Name + "hover_box");
			m_pHoverBoxQueryObject->setQueryFlag(0);
			m_pHoverBoxQueryObject->attachTo(m_SceneNode);
			m_pHoverBoxQueryObject->createAABBFrame(m_pEffectSystem->getBoundingBox(), Color::YELLOW);
			m_pHoverBoxQueryObject->setTestDepth(true);

			m_pSelectBoxQueryObject = pManager->createQueryObject(m_Name + "select_box");
			m_pSelectBoxQueryObject->setQueryFlag(0);
			m_pSelectBoxQueryObject->attachTo(m_SceneNode);
			m_pSelectBoxQueryObject->createSphere(editor_r, 10, 10, Color::YELLOW);
			m_pSelectBoxQueryObject->setTestDepth(true);
		}
	}

	void SceneEffectObject::_unprepare()
	{
		if (!m_IOprepared)
			return;

		Scene* pScene = SceneManager::Instance()->getCurrentScene();
		pScene->unregisterGameObjectFromZoom(this);
		pScene->registerGameObjectToZoom_unprepare(this);

		if (Root::Instance()->isEditorMode())
		{
			QueryObjectManager* pManager = pScene->getQueryObjectManager();

			pManager->destroyQueryObject(m_pHoverBoxQueryObject);
			pManager->destroyQueryObject(m_pSelectBoxQueryObject);
			m_pHoverBoxQueryObject = NULL;
			m_pSelectBoxQueryObject = NULL;
		}

		EffectSystemManager::Instance()->destroyEffectSystem(m_pEffectSystem);
		m_pEffectSystem = NULL;
		m_ParentNode->destroyChild(m_SceneNode);
		m_SceneNode = NULL;
		m_IOprepared = false;
	}

	std::pair<bool, Real> SceneEffectObject::rayIntersects(const Ray& ray)
	{
		std::pair<bool, Real> result;
		Ray RayObject(ray);
		Matrix4 m_InversMat = m_SceneNode->getWorldMatrix();
		m_InversMat = m_InversMat.detInverse();
		RayObject.transform(m_InversMat);
		Real tMin = 0;
		result.first = RayObject.hitSphere(Sphere(Vector3::ZERO, editor_r), tMin);
		result.second = tMin;

		return result;
	}

	void SceneEffectObject::_render()
	{
		LordAssert(m_pEffectSystem);

		if (m_bVisible)
		{
			m_pEffectSystem->_updateRenderQueue();
		}

		if (Root::Instance()->isEditorMode())
		{
			m_pHoverBoxQueryObject->setVisible(getHovered());
		}
	}

	const Box& SceneEffectObject::getWorldAABB() const
	{
		LordAssert(m_pEffectSystem);
		return m_worldAABB;
	}

	void SceneEffectObject::updateWorldAABB()
	{
		LordAssert(m_pEffectSystem);
		m_worldAABB = m_pEffectSystem->getBoundingBox().transform(m_SceneNode->getWorldMatrix());
		//m_Center = m_worldAABB.getCenter();

		if (Root::Instance()->isEditorMode())
		{
			if (m_pHoverBoxQueryObject)
			{
				m_pHoverBoxQueryObject->createAABBFrame(m_pEffectSystem->getBoundingBox(), Color::BLUE);
			}
			if (m_pSelectBoxQueryObject)
			{
				m_pSelectBoxQueryObject->createSphere(editor_r, 10, 10, Color::BLUE);
			}
		}
	}

	void SceneEffectObject::frameMove( i32 deltaTime )
	{
		LordAssert(m_pEffectSystem);
		m_pEffectSystem->_update_sys(deltaTime);
	}
}