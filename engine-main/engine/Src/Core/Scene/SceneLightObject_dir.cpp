#include "Object/Root.h"
#include "SceneLightObject_dir.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Util/QueryObject.h"

namespace LORD
{
	const float editor_r = 0.3f;

	SceneLightObject_dir::SceneLightObject_dir(void)
		: GameObject()
	{
		m_Type = GOT_DIRLIGHT;
		editor_canMirror = false;
		editor_canRotate = true;
		editor_canScale = false;
		m_Color = Color::WHITE;
		m_Brightness = 1.0f;
		m_IndirectLightingScale = 1.f;
		m_IndirectLightingSaturation = 1.f;
		m_ShadowExponent = 2.0f;
		m_flags = 123;
	}


	SceneLightObject_dir::~SceneLightObject_dir(void)
	{
		_unprepare();
	}

	void SceneLightObject_dir::_prepare_res()
	{

	}

	void SceneLightObject_dir::_prepare_io()
	{

	}

	void SceneLightObject_dir::_prepare_obj()
	{
		assert(Root::Instance()->isEditorMode());
		m_LocalAABB = Box(-editor_r, -editor_r, -editor_r, editor_r, editor_r, editor_r);

		m_ParentNode = SceneManager::Instance()->getRootNode();
		m_SceneNode = m_ParentNode->createChild();
		LordAssert(m_SceneNode);
		// ----update scene node
		m_SceneNode->setWorldPosition(m_Position);
		m_SceneNode->setWorldOrientation(m_Oritation);
		m_SceneNode->setLocalScaling(m_Scale);
		m_SceneNode->update();

		m_IOprepared = true;

		adjustSize();

		Scene* scene = SceneManager::Instance()->getCurrentScene();
		LordAssert(scene);
		scene->registerGameObjectToZoom(this);

	}

	void SceneLightObject_dir::_prepare_editor()
	{
		assert (Root::Instance()->isEditorMode());

		Scene* pScene = SceneManager::Instance()->getCurrentScene();
		QueryObjectManager* pManager = pScene->getQueryObjectManager();

		m_pHoverBoxQueryObject = pManager->createQueryObject(m_Name + "hover_box");
		m_pHoverBoxQueryObject->setQueryFlag(0);
		m_pHoverBoxQueryObject->attachTo(m_SceneNode);

		m_pHoverBoxQueryObject->clear();
		m_pHoverBoxQueryObject->beginPolygon(QueryObject::QO_SOLIDQUAD);
		m_pHoverBoxQueryObject->setPosition(Vector3::ZERO); m_pHoverBoxQueryObject->setColor(Color::YELLOW);
		m_pHoverBoxQueryObject->setPosition(Vector3::ONE*10.0f); m_pHoverBoxQueryObject->setColor(Color::YELLOW);
		m_pHoverBoxQueryObject->endPolygon();
		m_pHoverBoxQueryObject->setTestDepth(true);
		m_pHoverBoxQueryObject->setWriteDepth(true);
		createDirLine();

		m_pSelectBoxQueryObject = pManager->createQueryObject(m_Name + "select_box");
		m_pSelectBoxQueryObject->setQueryFlag(0);
		m_pSelectBoxQueryObject->attachTo(m_SceneNode);

		m_pSelectBoxQueryObject->createSphere(editor_r, 10, 10, Color::BLUE);
		m_pSelectBoxQueryObject->setTestDepth(true);
		m_pSelectBoxQueryObject->setWriteDepth(true);
	}

	void SceneLightObject_dir::_unprepare()
	{
		if (!m_IOprepared)
			return;

		Scene* pScene = SceneManager::Instance()->getCurrentScene();
		pScene->unregisterGameObjectFromZoom(this);
		//pScene->registerGameObjectToZoom_unprepare(this);  光源对象只在编辑器模式下存在
		QueryObjectManager* pManager = pScene->getQueryObjectManager();
		pManager->destroyQueryObject(m_pHoverBoxQueryObject);
		pManager->destroyQueryObject(m_pSelectBoxQueryObject);
		m_pHoverBoxQueryObject = NULL;
		m_pSelectBoxQueryObject = NULL;
		
		m_ParentNode->destroyChild(m_SceneNode);
		m_SceneNode = NULL;
		m_IOprepared = false;
	}

	void SceneLightObject_dir::frameMove( i32 deltaTime )
	{
	}

	void SceneLightObject_dir::_render()
	{
		if (Root::Instance()->isEditorMode() )
		{
			m_pHoverBoxQueryObject->setVisible(true);
			m_pSelectBoxQueryObject->setVisible(true);
		}
	}

	std::pair<bool, Real> SceneLightObject_dir::rayIntersects( const Ray& ray )
	{
		std::pair<bool, Real> result;
		Ray RayObject(ray);
		Matrix4 m_InversMat = m_SceneNode->getWorldMatrix();
		m_InversMat.detInverse();
		RayObject.transform(m_InversMat);
		Real tMin = 0;
		result.first = RayObject.hitSphere(Sphere(Vector3::ZERO, editor_r), tMin);
		result.second = tMin;

		return result;
	}

	const Box& SceneLightObject_dir::getWorldAABB() const
	{
		return m_worldAABB;
	}

	void SceneLightObject_dir::createDirLine()
	{
		m_pHoverBoxQueryObject->clear();
		m_pHoverBoxQueryObject->beginPolygon(QueryObject::QO_WIREFRAME);
		m_pHoverBoxQueryObject->setPosition(Vector3::ZERO); m_pHoverBoxQueryObject->setColor(m_Color);
		m_pHoverBoxQueryObject->setPosition(Vector3::NEG_UNIT_Z*10.0f); m_pHoverBoxQueryObject->setColor(m_Color);
		m_pHoverBoxQueryObject->setPosition(Vector3::UNIT_X); m_pHoverBoxQueryObject->setColor(m_Color);
		m_pHoverBoxQueryObject->setPosition(Vector3::UNIT_X + Vector3::NEG_UNIT_Z*10.0f); m_pHoverBoxQueryObject->setColor(m_Color);
		m_pHoverBoxQueryObject->setPosition(Vector3::NEG_UNIT_X); m_pHoverBoxQueryObject->setColor(m_Color);
		m_pHoverBoxQueryObject->setPosition(Vector3::NEG_UNIT_X + Vector3::NEG_UNIT_Z*10.0f); m_pHoverBoxQueryObject->setColor(m_Color);
		m_pHoverBoxQueryObject->setPosition(Vector3::UNIT_Y); m_pHoverBoxQueryObject->setColor(m_Color);
		m_pHoverBoxQueryObject->setPosition(Vector3::UNIT_Y + Vector3::NEG_UNIT_Z*10.0f); m_pHoverBoxQueryObject->setColor(m_Color);
		m_pHoverBoxQueryObject->setPosition(Vector3::NEG_UNIT_Y); m_pHoverBoxQueryObject->setColor(m_Color);
		m_pHoverBoxQueryObject->setPosition(Vector3::NEG_UNIT_Y + Vector3::NEG_UNIT_Z*10.0f); m_pHoverBoxQueryObject->setColor(m_Color);
		m_pHoverBoxQueryObject->endPolygon();
	}

	void SceneLightObject_dir::updateWorldAABB()
	{
		m_worldAABB = m_LocalAABB.transform(m_SceneNode->getWorldMatrix());
		assert(Root::Instance()->isEditorMode());

		if (m_pHoverBoxQueryObject)
		{
			createDirLine();
		}
		if (m_pSelectBoxQueryObject)
		{
			m_pSelectBoxQueryObject->createSphere(editor_r, 10, 10, Color::BLUE);
		}
	}

	void SceneLightObject_dir::getPropertyList( PropertyList & lst )
	{
		GameObject::getPropertyList(lst);
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_COLOR], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_BRIGHTNESS], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_INDIRSCALE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_INDIRSATURATE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_SHADOWEXP], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_CASTSHADOWS], true));
	}

	bool SceneLightObject_dir::getPropertyType( const String & name, PropertyType &type )
	{
		if (GameObject::getPropertyType(name, type))
		{
			return true;
		}

		if(name == objToken[TOKEN_LT_COLOR])
		{
			type = PT_COLOR; return true;
		}
		else if (name == objToken[TOKEN_LT_BRIGHTNESS])
		{
			type = PT_FLOAT; return true;
		}
		else if (name == objToken[TOKEN_LT_INDIRSCALE])
		{
			type = PT_FLOAT; return true;
		}
		else if (name == objToken[TOKEN_LT_INDIRSATURATE])
		{
			type = PT_FLOAT; return true;
		}
		else if (name == objToken[TOKEN_LT_SHADOWEXP])
		{
			type = PT_FLOAT; return true;
		}
		else if (name == objToken[TOKEN_LT_CASTSHADOWS])
		{
			type = PT_BOOL; return true;
		}
		return false;
	}

	bool SceneLightObject_dir::getPropertyValue( const String & name, String &value )
	{
		if (GameObject::getPropertyValue(name, value))
		{
			return true;
		}

		if(name == objToken[TOKEN_LT_COLOR])
		{
			value = StringUtil::ToString(m_Color);
			return true;
		}
		else if(name == objToken[TOKEN_LT_BRIGHTNESS])
		{
			value = StringUtil::ToString(m_Brightness);
			return true;
		}
		else if (name == objToken[TOKEN_LT_INDIRSCALE])
		{
			value = StringUtil::ToString(m_IndirectLightingScale);
			return true;
		}
		else if (name == objToken[TOKEN_LT_INDIRSATURATE])
		{
			value = StringUtil::ToString(m_IndirectLightingSaturation);
			return true;
		}
		else if (name == objToken[TOKEN_LT_SHADOWEXP])
		{
			value = StringUtil::ToString(m_ShadowExponent);
			return true;
		}
		else if (name == objToken[TOKEN_LT_CASTSHADOWS])
		{
			value = StringUtil::ToString(m_flags&0x00000001?true:false);
			return true;
		}
		return false;
	}

	bool SceneLightObject_dir::setPropertyValue( const String & name,const String & value )
	{
		if (GameObject::setPropertyValue(name, value))
		{
			return true;
		}

		if(name == objToken[TOKEN_LT_COLOR])
		{
			m_Color = StringUtil::ParseColor(value);
			adjustSize();
			return true;
		}
		else if(name == objToken[TOKEN_LT_BRIGHTNESS])
		{
			m_Brightness = StringUtil::ParseReal(value);
			return true;
		}
		else if (name == objToken[TOKEN_LT_INDIRSCALE])
		{
			m_IndirectLightingScale = StringUtil::ParseReal(value);
			return true;
		}
		else if (name == objToken[TOKEN_LT_INDIRSATURATE])
		{
			m_IndirectLightingSaturation = StringUtil::ParseReal(value);
			return true;
		}
		else if (name == objToken[TOKEN_LT_SHADOWEXP])
		{
			m_ShadowExponent = StringUtil::ParseReal(value);
			return true;
		}
		else if (name == objToken[TOKEN_LT_CASTSHADOWS])
		{
			if (StringUtil::ParseBool(value))
			{
				m_flags = m_flags | 0x00000001;
			}
			else
			{
				m_flags = m_flags ^ 0x00000001;

			}
			
			return true;
		}

		return false;
	}

	void SceneLightObject_dir::adjustSize()
	{
		if (!m_IOprepared)
			return;
		m_LocalAABB = Box(-editor_r, -editor_r, -editor_r, editor_r, editor_r, editor_r);
		updateWorldAABB();
	}

	void SceneLightObject_dir::setResourceName(const LORD::String& resName)
	{
		assert(m_IOprepared==false);
		m_Resource = resName;

		if(m_Resource == "红色光源") m_Color = LORD::Color(1.0f, 0.3f, 0.3f, 1.0f);
		else if(m_Resource == "绿色光源") m_Color = LORD::Color(0.3f, 1.0f, 0.3f, 1.0f);
		else if(m_Resource == "蓝色光源") m_Color = LORD::Color(0.3f, 0.3f, 1.0f, 1.0f);
		else if(m_Resource == "黄色光源") m_Color = LORD::Color(1.0f, 1.0f, 0.3f, 1.0f);
		else if(m_Resource == "紫色光源") m_Color = LORD::Color(1.0f, 0.3f, 1.0f, 1.0f);
		else if(m_Resource == "青色光源") m_Color = LORD::Color(0.3f, 1.0f, 1.0f, 1.0f);

	}

	const Color& SceneLightObject_dir::getColor() const
	{	
		return m_Color;
	}

	float SceneLightObject_dir::getBrightness() const
	{
		return m_Brightness;
	}
	
	float SceneLightObject_dir::getIndirectScale() const
	{
		return m_IndirectLightingScale;
	}

	float SceneLightObject_dir::getIndirectSaturation() const
	{
		return m_IndirectLightingSaturation;
	}

	float SceneLightObject_dir::getShadowExponent() const
	{
		return m_ShadowExponent;
	}

	LORD::ui32 SceneLightObject_dir::getFlags() const
	{
		return m_flags;
	}

}
