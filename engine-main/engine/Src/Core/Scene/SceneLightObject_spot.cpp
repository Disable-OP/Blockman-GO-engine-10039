#include "Object/Root.h"
#include "SceneLightObject_spot.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Util/QueryObject.h"

namespace LORD
{
	const float editor_r = 0.3f;

	SceneLightObject_spot::SceneLightObject_spot(void)
		: GameObject()
	{
		m_Type = GOT_SPOTLIGHT;
		editor_canMirror = false;
		editor_canRotate = true;
		editor_canScale = false;
		m_Color = Color::WHITE;
		m_Radius = 2.0f;
		m_Brightness = 1.0f;
		m_Falloff = 1.0f;
		m_IndirectLightingScale = 1.f;
		m_IndirectLightingSaturation = 1.f;
		m_ShadowExponent = 2.0f;
		m_InnerAngle = 10.0f;
		m_GradientAngle = 45.0f;
		m_flags = 31;
	}


	SceneLightObject_spot::~SceneLightObject_spot(void)
	{
		_unprepare();
	}

	void SceneLightObject_spot::_prepare_res()
	{

	}

	void SceneLightObject_spot::_prepare_io()
	{

	}

	void SceneLightObject_spot::_prepare_obj()
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

	void SceneLightObject_spot::_prepare_editor()
	{
		assert (Root::Instance()->isEditorMode());

		Scene* pScene = SceneManager::Instance()->getCurrentScene();
		QueryObjectManager* pManager = pScene->getQueryObjectManager();

		m_pHoverBoxQueryObject = pManager->createQueryObject(m_Name + "hover_box");
		m_pHoverBoxQueryObject->setQueryFlag(0);
		m_pHoverBoxQueryObject->attachTo(m_SceneNode);
		m_pHoverBoxQueryObject->setTestDepth(true);
		m_pHoverBoxQueryObject->setWriteDepth(true);
		createSpot();

		m_pSelectBoxQueryObject = pManager->createQueryObject(m_Name + "select_box");
		m_pSelectBoxQueryObject->setQueryFlag(0);
		m_pSelectBoxQueryObject->attachTo(m_SceneNode);

		m_pSelectBoxQueryObject->createSphere(editor_r, 10, 10, Color::BLUE);
		m_pSelectBoxQueryObject->setTestDepth(true);
		m_pSelectBoxQueryObject->setWriteDepth(true);
	}

	void SceneLightObject_spot::_unprepare()
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

	void SceneLightObject_spot::frameMove( i32 deltaTime )
	{
	}

	void SceneLightObject_spot::_render()
	{
		if (Root::Instance()->isEditorMode() )
		{
			m_pHoverBoxQueryObject->setVisible(true);
			m_pSelectBoxQueryObject->setVisible(true);
		}
	}

	std::pair<bool, Real> SceneLightObject_spot::rayIntersects( const Ray& ray )
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

	const Box& SceneLightObject_spot::getWorldAABB() const
	{
		return m_worldAABB;
	}

	void SceneLightObject_spot::createSpot()
	{
		if (m_pHoverBoxQueryObject)
		{
			m_pHoverBoxQueryObject->clear();
			m_pHoverBoxQueryObject->beginPolygon(QueryObject::QO_WIREFRAME);

			//float AngleLongitude = 0.0f;
			Real SegmentLongitude = Math::PI_2/10.0f;
			vector<Vector3>::type innerVertexArray, GradVertexArray;
			for(i32 i=0; i<12; ++i)
			{
				Vector3 innerpos = Vector3::NEG_UNIT_Z * 10.0f;
				innerpos.x = 10.0f*Math::Tan(Math::DEG2RAD*m_InnerAngle)*sin(30*i*2*Math::PI/360.0f);
				innerpos.y = -10.0f*Math::Tan(Math::DEG2RAD*m_InnerAngle)*cos(30*i*2*Math::PI/360.0f);

				m_pHoverBoxQueryObject->setPosition(Vector3::ZERO); 
				m_pHoverBoxQueryObject->setColor(m_Color);
				m_pHoverBoxQueryObject->setPosition(innerpos);
				m_pHoverBoxQueryObject->setColor(m_Color);
				innerVertexArray.push_back(innerpos);
				//AngleLongitude += SegmentLongitude;

				if (i<8)
				{
					Vector3 gradpos = Vector3::NEG_UNIT_Z * 10.0f;
					gradpos.x = 10.0f*Math::Tan(Math::DEG2RAD*m_GradientAngle)*sin(45*i*2*Math::PI/360.0f);
					gradpos.y = -10.0f*Math::Tan(Math::DEG2RAD*m_GradientAngle)*cos(45*i*2*Math::PI/360.0f);
					m_pHoverBoxQueryObject->setPosition(Vector3::ZERO); 
					m_pHoverBoxQueryObject->setColor(m_Color*0.5f);
					m_pHoverBoxQueryObject->setPosition(gradpos);
					m_pHoverBoxQueryObject->setColor(m_Color*0.5f);
					GradVertexArray.push_back(gradpos);
				}
			}
			for (ui32 n=0; n<innerVertexArray.size(); ++n)
			{
				if (n<innerVertexArray.size()-1)
				{
					m_pHoverBoxQueryObject->setPosition(innerVertexArray[n]); 
					m_pHoverBoxQueryObject->setColor(m_Color);
					m_pHoverBoxQueryObject->setPosition(innerVertexArray[n+1]);
					m_pHoverBoxQueryObject->setColor(m_Color);
				}
				else
				{
					m_pHoverBoxQueryObject->setPosition(innerVertexArray[n]); 
					m_pHoverBoxQueryObject->setColor(m_Color);
					m_pHoverBoxQueryObject->setPosition(innerVertexArray[0]);
					m_pHoverBoxQueryObject->setColor(m_Color);
				}
			}
			for (ui32 n=0; n<GradVertexArray.size(); ++n)
			{
				if (n<GradVertexArray.size()-1)
				{
					m_pHoverBoxQueryObject->setPosition(GradVertexArray[n]); 
					m_pHoverBoxQueryObject->setColor(m_Color*0.5f);
					m_pHoverBoxQueryObject->setPosition(GradVertexArray[n+1]);
					m_pHoverBoxQueryObject->setColor(m_Color*0.5f);
				}
				else
				{
					m_pHoverBoxQueryObject->setPosition(GradVertexArray[n]); 
					m_pHoverBoxQueryObject->setColor(m_Color*0.5f);
					m_pHoverBoxQueryObject->setPosition(GradVertexArray[0]);
					m_pHoverBoxQueryObject->setColor(m_Color*0.5f);
				}
			}
			ui32 spotVertexNum = m_pHoverBoxQueryObject->getVerticesNum();
			for (ui32 i=0; i<spotVertexNum; ++i)
			{
				m_pHoverBoxQueryObject->setIndex(i);
			}
			

			// create circle
			float segment = 40;
			for(int i=0; i<segment; ++i)
			{
				float x = Math::Sin(float(i)/float(segment)*Math::PI_2);
				float y = Math::Cos(float(i)/float(segment)*Math::PI_2);
				m_pHoverBoxQueryObject->setPosition(0.f, x*m_Radius, y*m_Radius);
				m_pHoverBoxQueryObject->setColor(m_Color.r, m_Color.g, m_Color.b, m_Color.a);
				m_pHoverBoxQueryObject->setPosition(x*m_Radius, 0.f, y*m_Radius);
				m_pHoverBoxQueryObject->setColor(m_Color.r, m_Color.g, m_Color.b, m_Color.a);
				m_pHoverBoxQueryObject->setPosition(x*m_Radius, y*m_Radius, 0.f);
				m_pHoverBoxQueryObject->setColor(m_Color.r, m_Color.g, m_Color.b, m_Color.a);
			}
			for(int j=0;j<3;j++)
			{
				for(int i=0; i<segment; ++i)
				{
					m_pHoverBoxQueryObject->setIndex(i*3 + j + spotVertexNum); 
					if(i==segment-1)
						m_pHoverBoxQueryObject->setIndex(j + spotVertexNum);
					else
						m_pHoverBoxQueryObject->setIndex((i+1)*3+j + spotVertexNum);
				}
			}
			m_pHoverBoxQueryObject->endPolygon();
		}
	}

	void SceneLightObject_spot::updateWorldAABB()
	{
		m_worldAABB = m_LocalAABB.transform(m_SceneNode->getWorldMatrix());
		assert(Root::Instance()->isEditorMode());

		createSpot();

		if (m_pSelectBoxQueryObject)
		{
			m_pSelectBoxQueryObject->createSphere(editor_r, 10, 10, Color::BLUE);
		}
	}

	void SceneLightObject_spot::getPropertyList( PropertyList & lst )
	{
		GameObject::getPropertyList(lst);
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_COLOR], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_RADIUS], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_BRIGHTNESS], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_FALLOFF], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_INDIRSCALE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_INDIRSATURATE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_SHADOWEXP], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_INNERANGLE], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_LT_GRADIENTANGLE], true));
	}

	bool SceneLightObject_spot::getPropertyType( const String & name, PropertyType &type )
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
		else if (name == objToken[TOKEN_LT_INNERANGLE])
		{
			type = PT_FLOAT; return true;
		}
		else if (name == objToken[TOKEN_LT_GRADIENTANGLE])
		{
			type = PT_FLOAT; return true;
		}
		else if (name == objToken[TOKEN_LT_CASTSHADOWS])
		{
			type = PT_BOOL; return true;
		}
		return false;
	}

	bool SceneLightObject_spot::getPropertyValue( const String & name, String &value )
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
		else if (name == objToken[TOKEN_LT_INNERANGLE])
		{
			value = StringUtil::ToString(m_InnerAngle);
			return true;
		}
		else if (name == objToken[TOKEN_LT_GRADIENTANGLE])
		{
			value = StringUtil::ToString(m_GradientAngle);
			return true;
		}
		else if (name == objToken[TOKEN_LT_CASTSHADOWS])
		{
			value = StringUtil::ToString(m_flags&0x00000001?true:false);
			return true;
		}
		return false;
	}

	bool SceneLightObject_spot::setPropertyValue( const String & name,const String & value )
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
		else if (name == objToken[TOKEN_LT_INNERANGLE])
		{
			m_InnerAngle = StringUtil::ParseReal(value);
			adjustSize();
			return true;
		}
		else if (name == objToken[TOKEN_LT_GRADIENTANGLE])
		{
			m_GradientAngle = StringUtil::ParseReal(value);
			adjustSize();
			return true;
		}
		else if (name == objToken[TOKEN_LT_CASTSHADOWS])
		{
			if (StringUtil::ParseBool(value))
			{
				m_flags |= 0x00000001;
			}
			else
			{
				m_flags = m_flags ^ 0x00000001;
			}
			
			return true;
		}

		return false;
	}

	void SceneLightObject_spot::adjustSize()
	{
		if (!m_IOprepared)
			return;
		m_LocalAABB = Box(-editor_r, -editor_r, -editor_r, editor_r, editor_r, editor_r);
		updateWorldAABB();
	}

	void SceneLightObject_spot::setResourceName(const LORD::String& resName)
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

	const Color& SceneLightObject_spot::getColor() const
	{	
		return m_Color;
	}

	float SceneLightObject_spot::getBrightness() const
	{
		return m_Brightness;
	}
	
	float SceneLightObject_spot::getIndirectScale() const
	{
		return m_IndirectLightingScale;
	}

	float SceneLightObject_spot::getIndirectSaturation() const
	{
		return m_IndirectLightingSaturation;
	}

	float SceneLightObject_spot::getShadowExponent() const
	{
		return m_ShadowExponent;
	}

	LORD::ui32 SceneLightObject_spot::getFlags() const
	{
		return m_flags;
	}

	float SceneLightObject_spot::getInnerAngle() const
	{
		return m_InnerAngle;
	}

	float SceneLightObject_spot::getGradientAngle() const
	{
		return m_GradientAngle;
	}

	float SceneLightObject_spot::getRadius() const
	{
		return m_Radius;
	}

	float SceneLightObject_spot::getFalloff() const
	{
		return m_Falloff;
	}

}