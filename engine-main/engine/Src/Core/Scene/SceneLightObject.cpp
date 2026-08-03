#include "Object/Root.h"
#include "SceneLightObject.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Util/QueryObject.h"

namespace LORD
{
	const float editor_r = 0.3f;

	SceneLightObject::SceneLightObject(void)
		: GameObject()
	{
		m_Type = GOT_POINTLIGHT;
		editor_canMirror = false;
		editor_canRotate = false;
		editor_canScale = false;
		m_Color = Color::WHITE;
		m_Radius = 2.0f;
		m_Brightness = 1.0f;
		m_Falloff = 1.0f;
		m_IndirectLightingScale = 1.f;
		m_IndirectLightingSaturation = 1.f;
		m_ShadowExponent = 2.0f;
		m_pLightQueryObject = NULL;
	}


	SceneLightObject::~SceneLightObject(void)
	{
		_unprepare();
	}

	void SceneLightObject::_prepare_res()
	{

	}

	void SceneLightObject::_prepare_io()
	{

	}

	void SceneLightObject::_prepare_obj()
	{
		assert(Root::Instance()->isEditorMode());
		m_LocalAABB = Box(-m_Radius, -m_Radius, -m_Radius, m_Radius, m_Radius, m_Radius);

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

	void SceneLightObject::_prepare_editor()
	{
		assert (Root::Instance()->isEditorMode());

		Scene* pScene = SceneManager::Instance()->getCurrentScene();
		QueryObjectManager* pManager = pScene->getQueryObjectManager();

		m_pHoverBoxQueryObject = pManager->createQueryObject(m_Name + "hover_box");
		m_pHoverBoxQueryObject->setQueryFlag(0);
		m_pHoverBoxQueryObject->attachTo(m_SceneNode);

		m_pHoverBoxQueryObject->create3DCircle(m_Radius, 40, Color::YELLOW, Color::YELLOW, Color::YELLOW);
		m_pHoverBoxQueryObject->setTestDepth(true);
		m_pHoverBoxQueryObject->setWriteDepth(true);

		m_pSelectBoxQueryObject = pManager->createQueryObject(m_Name + "select_box");
		m_pSelectBoxQueryObject->setQueryFlag(0);
		m_pSelectBoxQueryObject->attachTo(m_SceneNode);

		m_pSelectBoxQueryObject->createSphere(editor_r, 10, 10, Color::BLUE);
		m_pSelectBoxQueryObject->setTestDepth(true);
		m_pSelectBoxQueryObject->setWriteDepth(true);

		m_pLightQueryObject = pManager->createQueryObject(m_Name + "light_circle");
		m_pLightQueryObject->setQueryFlag(0);
		m_pLightQueryObject->attachTo(m_SceneNode);

		m_pLightQueryObject->create3DCircle(m_Radius, 40, m_Color, m_Color, m_Color);
		m_pLightQueryObject->setTestDepth(true);
		m_pLightQueryObject->setWriteDepth(true);
	}

	void SceneLightObject::_unprepare()
	{
		if (!m_IOprepared)
			return;

		Scene* pScene = SceneManager::Instance()->getCurrentScene();
		pScene->unregisterGameObjectFromZoom(this);
		//pScene->registerGameObjectToZoom_unprepare(this);  光源对象只在编辑器模式下存在
		QueryObjectManager* pManager = pScene->getQueryObjectManager();
		pManager->destroyQueryObject(m_pHoverBoxQueryObject);
		pManager->destroyQueryObject(m_pSelectBoxQueryObject);
		pManager->destroyQueryObject(m_pLightQueryObject);
		m_pHoverBoxQueryObject = NULL;
		m_pSelectBoxQueryObject = NULL;
		m_pLightQueryObject = NULL;

		m_ParentNode->destroyChild(m_SceneNode);
		m_SceneNode = NULL;
		m_IOprepared = false;
	}

	void SceneLightObject::frameMove( i32 deltaTime )
	{
	}

	void SceneLightObject::_render()
	{
		if (Root::Instance()->isEditorMode() )
		{
			m_pHoverBoxQueryObject->setVisible(editor_hovered);
			m_pLightQueryObject->setVisible(editor_selected);
		}
	}

	std::pair<bool, Real> SceneLightObject::rayIntersects( const Ray& ray )
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

	const Box& SceneLightObject::getWorldAABB() const
	{
		return m_worldAABB;
	}

	void SceneLightObject::updateWorldAABB()
	{
		m_worldAABB = m_LocalAABB.transform(m_SceneNode->getWorldMatrix());
		assert(Root::Instance()->isEditorMode());

		if (m_pHoverBoxQueryObject)
		{
			m_pHoverBoxQueryObject->create3DCircle(m_Radius, 40, Color::YELLOW, Color::YELLOW, Color::YELLOW);
		}
		if (m_pSelectBoxQueryObject)
		{
			m_pSelectBoxQueryObject->createSphere(editor_r, 10, 10, Color::BLUE);
		}
		if (m_pLightQueryObject)
		{
			m_pLightQueryObject->create3DCircle(m_Radius, 40, m_Color, m_Color, m_Color);
		}
	}

	void SceneLightObject::getPropertyList( PropertyList & lst )
	{
		GameObject::getPropertyList(lst);
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_COLOR], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_RADIUS], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_BRIGHTNESS], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_FALLOFF], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_INDIRSCALE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_INDIRSATURATE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_SHADOWEXP], true));
	}

	bool SceneLightObject::getPropertyType( const String & name, PropertyType &type )
	{
		if (GameObject::getPropertyType(name, type))
		{
			return true;
		}

		if(name == objToken[TOKEN_LT_COLOR])
		{
			type = PT_COLOR; return true;
		}
		else if (name == objToken[TOKEN_LT_RADIUS])
		{
			type = PT_FLOAT; return true;
		}
		else if (name == objToken[TOKEN_LT_BRIGHTNESS])
		{
			type = PT_FLOAT; return true;
		}
		else if (name == objToken[TOKEN_LT_FALLOFF])
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

		return false;
	}

	bool SceneLightObject::getPropertyValue( const String & name, String &value )
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
		else if(name == objToken[TOKEN_LT_RADIUS])
		{
			value = StringUtil::ToString(m_Radius);
			return true;
		}
		else if(name == objToken[TOKEN_LT_BRIGHTNESS])
		{
			value = StringUtil::ToString(m_Brightness);
			return true;
		}
		else if(name == objToken[TOKEN_LT_FALLOFF])
		{
			value = StringUtil::ToString(m_Falloff);
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
		return false;
	}

	bool SceneLightObject::setPropertyValue( const String & name,const String & value )
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
		else if(name == objToken[TOKEN_LT_RADIUS])
		{
			m_Radius = StringUtil::ParseReal(value);
			adjustSize();
			return true;
		}
		else if(name == objToken[TOKEN_LT_BRIGHTNESS])
		{
			m_Brightness = StringUtil::ParseReal(value);
			return true;
		}
		else if(name == objToken[TOKEN_LT_FALLOFF])
		{
			m_Falloff = StringUtil::ParseReal(value);
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

		return false;
	}

	void SceneLightObject::adjustSize()
	{
		if (!m_IOprepared)
			return;
		m_LocalAABB = Box(-m_Radius, -m_Radius, -m_Radius, m_Radius, m_Radius, m_Radius);
		updateWorldAABB();
	}

	void SceneLightObject::setResourceName(const LORD::String& resName)
	{
		assert(m_IOprepared==false);
		m_Resource = resName;
		LORD::String _resName = m_Resource;
		LORD::StringArray resNameVector = LORD::StringUtil::Split(_resName, "_");

		if (resNameVector.size() == 2)
		{
			if(resNameVector[0] == "红") m_Color = LORD::Color(1.0f, 0.3f, 0.3f, 1.0f);
			else if(resNameVector[0] == "绿") m_Color = LORD::Color(0.3f, 1.0f, 0.3f, 1.0f);
			else if(resNameVector[0] == "蓝") m_Color = LORD::Color(0.3f, 0.3f, 1.0f, 1.0f);
			else if(resNameVector[0] == "黄") m_Color = LORD::Color(1.0f, 1.0f, 0.3f, 1.0f);
			else if(resNameVector[0] == "紫") m_Color = LORD::Color(1.0f, 0.3f, 1.0f, 1.0f);
			else if(resNameVector[0] == "青") m_Color = LORD::Color(0.3f, 1.0f, 1.0f, 1.0f);
			m_Radius = LORD::StringUtil::ParseReal(resNameVector[1]);
		}
	}

	const Color& SceneLightObject::getColor() const
	{	
		return m_Color;
	}

	float SceneLightObject::getRadius() const
	{
		return m_Radius;
	}

	float SceneLightObject::getBrightness() const
	{
		return m_Brightness;
	}

	float SceneLightObject::getFalloff() const
	{
		return m_Falloff;
	}

	float SceneLightObject::getIndirectScale() const
	{
		return m_IndirectLightingScale;
	}

	float SceneLightObject::getIndirectSaturation() const
	{
		return m_IndirectLightingSaturation;
	}

	float SceneLightObject::getShadowExponent() const
	{
		return m_ShadowExponent;
	}


}