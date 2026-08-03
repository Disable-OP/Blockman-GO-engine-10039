#include "Core.h"
#include "GameObject.h"
#include "SceneNode.h"
#include "Model/Mesh.h"
#include "Zoom.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Image/Image.h"
#include "Scene/SceneNode.h"
#include "Resource/DataStream.h"
#include "Geom/Ray.h"

namespace LORD
{
	GameObject::GameObject()
		: m_UserData(0)
		, m_IOprepared(false)
		, m_ParentNode(NULL)
		, m_SceneNode(NULL)
		, m_Position(Vector3::ZERO)
		, m_Oritation(Quaternion::IDENTITY)
		, m_Pitch(0.0f)
		, m_Yaw(0.0f)
		, m_Roll(0.0f)
		, m_Scale(Vector3::ONE)
		, m_Mirror(MIR_NONE)
		, m_Piror(1)
		, m_Resource(StringUtil::BLANK)
		, editor_canTranslate(true)
		, editor_canRotate(false)
		, editor_canScale(false)
		, editor_canMirror(false)
		, editor_selected(false)
		, editor_hovered(false)
		, m_UnPrepareTime(0)
		, m_bRequestPrepare(false)
		, m_bRequestDestroy(false)
		, m_bInsideList(false)
		, m_bFrustumCull(true)
		, m_bSceneCreate(false)
		, m_bVisible(true)
		, m_pOwnerZoom(NULL)
		, m_pHoverBoxQueryObject(NULL)
		, m_pSelectBoxQueryObject(NULL)
		, m_isNeedSave(true)
	{
		mLastUpdateFrameNumber = 0;
		memset(m_Reverse, 0, sizeof(int) * 6);

		m_Material = LordNew Material;
		m_RenderQueue = SceneManager::Instance()->getRenderQueue("Scene");
	}

	GameObject::~GameObject()
	{
		LordDelete m_Material;
		m_Material = NULL;
		//if(editor_hoverVH)
		//{
		//	ECHO_DELETE_T(editor_hoverVH, EEGObjVisualHelp, MEMCATEGORY_SCENE_OBJECTS);
		//	editor_hoverVH = NULL;
		//}
		//if(editor_selectVH)
		//{
		//	ECHO_DELETE_T(editor_selectVH, EEGObjVisualHelp, MEMCATEGORY_SCENE_OBJECTS);
		//	editor_selectVH = NULL;
		//}
	}

	void GameObject::_prepare_sys()
	{
		_prepare_res();		//资源对象
		_prepare_io();		//磁盘数据
		_prepare_obj();		//d3d资源
		_prepare_editor();	//编辑对象
	}

	//bool GameObject::importData(StreamSerialiser& dataStream, int vision /* = 100 */)
	//{
	//	dataStream.read(&m_Name);
	//	dataStream.read(&m_Position);
	//	dataStream.read(&m_Oritation);
	//	dataStream.read(&m_Scale);
	//	dataStream.read(&m_Mirror);
	//	dataStream.read(&m_Piror);
	//	dataStream.read(&m_Resource);
	//	return true;
	//}
	//
	//bool GameObject::exportData(StreamSerialiser& dataStream, int vision /* = 100 */)
	//{
	//	dataStream.write(&m_Name);
	//	dataStream.write(&m_Position);
	//	dataStream.write(&m_Oritation);
	//	dataStream.write(&m_Scale);
	//	dataStream.write(&m_Mirror);
	//	dataStream.write(&m_Piror);
	//	dataStream.write(&m_Resource);
	//	return true;
	//}

	const String& GameObject::getResourceName() const
	{
		return m_Resource;
	}

	void GameObject::setResourceName(const String& resName)
	{
		assert(m_IOprepared == false);
		m_Resource = resName;
	}

	void GameObject::setSelected(bool selected)
	{
		editor_selected = selected;
	}

	void GameObject::setHovered(bool hovered)
	{
		editor_hovered = hovered;
	}

	bool GameObject::getSelected() const
	{
		return editor_selected;
	}

	bool GameObject::getHovered() const
	{
		return editor_hovered;
	}

	/*bool GameObject::getIOprepared() const
	{
	return m_IOprepared;
	}*/

	//void GameObject::registerSceneNode(SceneNode* parent, bool updateBounds /* = true */)
	//{
	//	assert(m_ParentNode == NULL);
	//	assert(m_SceneNode != NULL);
	//	assert(m_IOprepared);
	//
	//	m_ParentNode = parent;
	//	m_ParentNode->addChild(m_SceneNode);
	//	//m_SceneNode->setPosition(m_Position);
	//	//m_SceneNode->setOrientation(m_Oritation);
	//	//m_SceneNode->setScale(m_Scale);
	//	//m_SceneNode->_updateBounds();
	//
	//	//if(updateBounds)
	//	//	m_ParentNode->_updateBounds();
	//}

	///以下四个函数都需要更新obj的所注册的包围盒的状态
	void GameObject::setPosition(const Vector3& pos)
	{
		if (m_IOprepared == false)
		{
			m_Position = pos;
			return;
		}

		m_SceneNode->setWorldPosition(pos);
		m_SceneNode->update();
		m_Position = pos;

		Scene* scene = SceneManager::Instance()->getCurrentScene();
		LordAssert(scene);
		scene->unregisterGameObjectFromZoom(this);
		updateWorldAABB();
		scene->registerGameObjectToZoom(this);

		//EESceneManager* sceneManager = getEESceneManager();
		//sceneManager->UnRegisterGameObject(this);
		//sceneManager->UnRegisterGameNode(this, true);
		//m_Position = pos;
		//sceneManager->RegisterGameNode(this, true);
		//objectMoved();
		//sceneManager->RegisterGameObject(this);
	}
	void GameObject::setOritation(const Quaternion& ori)
	{
		if (m_IOprepared == false)
		{
			m_Oritation = ori;
			return;
		}

		m_SceneNode->setWorldOrientation(ori);
		m_SceneNode->update();
		m_Oritation = ori;

		updateWorldAABB();

		//EESceneManager* sceneManager = getEESceneManager();
		//sceneManager->UnRegisterGameObject(this);
		//sceneManager->UnRegisterGameNode(this, true);
		//m_Oritation = ori;
		//sceneManager->RegisterGameNode(this, true);
		//sceneManager->RegisterGameObject(this);
		//objectMoved();
	}
	void GameObject::setScale(const Vector3& scale)
	{
		if (m_IOprepared == false)
		{
			m_Scale = scale;
			return;
		}

		m_SceneNode->setLocalScaling(scale);
		m_SceneNode->update();
		m_Scale = scale;

		Scene* scene = SceneManager::Instance()->getCurrentScene();
		LordAssert(scene);
		scene->unregisterGameObjectFromZoom(this);
		updateWorldAABB();
		scene->registerGameObjectToZoom(this);

		//EESceneManager* sceneManager = getEESceneManager();
		//sceneManager->UnRegisterGameObject(this);
		//sceneManager->UnRegisterGameNode(this, true);
		//m_Scale = scale;
		//sceneManager->RegisterGameNode(this, true);
		//sceneManager->RegisterGameObject(this);
		//objectMoved();
	}

	//void GameObject::unregisterSceneNode(bool updateBounds /* = true */)
	//{
	//	assert(m_ParentNode);
	//	assert(m_SceneNode);
	//	assert(m_IOprepared);
	//
	//	m_ParentNode->removeChild(m_Name);
	//	//if(updateBounds)
	//	//	m_ParentNode->_updateBounds();
	//
	//	m_ParentNode = NULL;
	//}

	String GameObject::getMirrorTypeString() const
	{
		String ret;
		switch (m_Mirror)
		{
		case MIR_XY:
			ret = "XY"; break;
		case MIR_XZ:
			ret = "XZ"; break;
		case MIR_YZ:
			ret = "YZ"; break;
		case MIR_NONE:
			ret = "NONE"; break;
		default:
			break;
		}
		return ret;
	}

	void GameObject::setMirrorType(const String& ms)
	{
		Vector3 scale = m_Scale;
		scale.x = Math::Abs(scale.x);
		scale.y = Math::Abs(scale.y);
		scale.z = Math::Abs(scale.z);
		if (ms == "XY")
		{
			m_Mirror = MIR_XY;
			scale.z = -scale.z;
		}
		else if (ms == "XZ")
		{
			m_Mirror = MIR_XZ;
			scale.y = -scale.y;
		}
		else if (ms == "YZ")
		{
			m_Mirror = MIR_YZ;
			scale.x = -scale.x;
		}
		else
		{
			m_Mirror = MIR_NONE;
		}
		setScale(scale);
	}

	std::pair<bool, Real> GameObject::rayIntersectsMesh(const Ray& localRay, Mesh* pMesh)
	{
		vector<PickInfo>::type PickResults;
		std::pair<bool, Real> result;
		result.first = false;
		result.second = 10000.0f;

		size_t		vertexSize;
		const float*vertexPtr;
		ui16		indexNum;
		const ui16*	indexPtr;
		Real		tMin;

		for (int i = 0; i < pMesh->getSubMeshNum(); i++)
		{
			SubMesh* pSubMesh = pMesh->getSubMesh(i);
			if (pSubMesh)
			{
				vertexPtr = (float*)pSubMesh->getVertices();
				indexPtr = (ui16*)pSubMesh->getIndices();
				vertexSize = pSubMesh->getVertexStride() / sizeof(float);
				indexNum = pSubMesh->getIndexCount();

				for (ui16 i = 0; i < static_cast<int>(indexNum); i += 3)
				{
					bool h = localRay.hitTri(Triangle(*(Vector3*)(vertexPtr + indexPtr[i] * vertexSize),
						*(Vector3*)(vertexPtr + indexPtr[i + 1] * vertexSize),
						*(Vector3*)(vertexPtr + indexPtr[i + 2] * vertexSize)),
						tMin);

					if (!h)
						continue;

					PickInfo pickInfo;
					pickInfo.info.first = h;
					pickInfo.info.second = tMin;
					PickResults.push_back(pickInfo);
				}
			}
		}

		std::sort(PickResults.begin(), PickResults.end());

		vector<PickInfo>::type::iterator iter = PickResults.begin();
		if (iter != PickResults.end())
			result = iter->info;

		return result;
	}

	std::pair<bool, Real> GameObject::rayIntersectsMesh(const Ray& localRay, Mesh* pMesh, bool bTopPoint)
	{
		vector<PickInfo>::type PickResults;
		std::pair<bool, Real> result;
		//result.first = false;
		//if (bTopPoint)
		//	result.second = 10000.0f;
		//else
		//	result.second = 0.0f;

		//size_t				vertexSize;			///单个顶点字节数
		//const float*		vertexPtr;			///顶点指针
		//uint16		indexNum;			///索引个数
		//const uint16*	indexPtr;		///索引指针
		//SubMesh*		subMesh;
		//VertexData*	pVb;
		//IndexData*	pIb;

		//for(size_t iSubMeshIndex = 0; iSubMeshIndex < pMesh->getNumSubMeshes(); ++iSubMeshIndex)
		//{
		//	subMesh = pMesh->getSubMesh(iSubMeshIndex);
		//	assert(subMesh);
		//	assert(subMesh->vertexData);
		//	assert(subMesh->indexData);

		//	pVb = subMesh->vertexData;
		//	pIb = subMesh->indexData;
		//	vertexSize = (pVb->vertexDeclaration->getVertexSize(0))/sizeof(float);
		//	vertexPtr = static_cast<float*>(pVb->vertexBufferBinding->getBuffer(0)->getSystemMemory());
		//	indexNum = pIb->indexCount;
		//	indexPtr = static_cast<uint16*>(pIb->indexBuffer->getSystemMemory());

		//	for(uint16 i = 0; i < static_cast<int>(indexNum); i += 3)
		//	{
		//		std::pair<bool, Real> temp_result;
		//		temp_result = Math::intersects(localRay,
		//			*(Vec3*)(vertexPtr+indexPtr[i]*vertexSize),
		//			*(Vec3*)(vertexPtr+indexPtr[i+1]*vertexSize),
		//			*(Vec3*)(vertexPtr+indexPtr[i+2]*vertexSize), true, false);

		//		if (!temp_result.first)
		//			continue;

		//		if (bTopPoint)
		//		{
		//			PickInfo pickInfo;
		//			pickInfo.info = temp_result;
		//			PickResults.push_back(pickInfo);
		//		}
		//		else
		//		{
		//			return temp_result;
		//		}
		//	}
		//}

		//std::sort(PickResults.begin(), PickResults.end());

		//vector<PickInfo>::type::iterator iter = PickResults.begin();
		//if (iter!=PickResults.end())
		//	result = iter->info;

		return result;
	}

	std::pair<bool, Real> GameObject::rayIntersects(const Ray& ray)
	{
		std::pair<bool, Real> result;
		const Box& box = getWorldAABB();
		Real tMin;
		HitInfo hitInfo;
		result.first = ray.hitBox(box, tMin, hitInfo);
		if (result.first)
		{
			result.second = tMin;
		}
		return result;
	}

	Quaternion GameObject::reCaculateOritation()
	{
		float fCosHRoll = Math::Cos(m_Roll*Math::DEG2RAD * 0.5f);
		float fSinHRoll = Math::Sin(m_Roll*Math::DEG2RAD * 0.5f);
		float fCosHPitch = Math::Cos(m_Pitch*Math::DEG2RAD * 0.5f);
		float fSinHPitch = Math::Sin(m_Pitch*Math::DEG2RAD * 0.5f);
		float fCosHYaw = Math::Cos(m_Yaw*Math::DEG2RAD * 0.5f);
		float fSinHYaw = Math::Sin(m_Yaw*Math::DEG2RAD * 0.5f);

		float w, x, y, z;
		w = fCosHRoll * fCosHPitch * fCosHYaw + fSinHRoll * fSinHPitch * fSinHYaw;
		x = fCosHRoll * fSinHPitch * fCosHYaw + fSinHRoll * fCosHPitch * fSinHYaw;
		y = fCosHRoll * fCosHPitch * fSinHYaw - fSinHRoll * fSinHPitch * fCosHYaw;
		z = fSinHRoll * fCosHPitch * fCosHYaw - fCosHRoll * fSinHPitch * fSinHYaw;

		return Quaternion(w, x, y, z);
	}

	void GameObject::reCaculateAngle()
	{
		float w, x, y, z;
		w = m_Oritation.w;
		x = m_Oritation.x;
		y = m_Oritation.y;
		z = m_Oritation.z;

		float roll = Math::ATan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (z * z + x * x));
		float pitch = Math::ASin(2.0f * (w * x - y * z));
		float yaw = Math::ATan2(2.0f * (w * y + z * x), 1.0f - 2.0f * (x * x + y * y));

		m_Pitch = pitch * Math::RAD2DEG;
		m_Yaw = yaw * Math::RAD2DEG;
		m_Roll = roll * Math::RAD2DEG;
	}

	void GameObject::getPropertyList(PropertyList & lst)
	{
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_NAME], false));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_POSITION], editor_canTranslate));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_ORITATION], false));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_PITCH], editor_canRotate));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_YAW], editor_canRotate));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_ROLL], editor_canRotate));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_SCALE], editor_canScale));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_MIRROR], editor_canMirror));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_PIROR], true));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_RESOURCE], false));
		lst.push_back(std::pair<String, bool>(objToken[TOKEN_OBJ_NOTE], true));
	}

	bool GameObject::getPropertyType(const String & name, PropertyType &type)
	{
		if (name == objToken[TOKEN_OBJ_NAME])
		{
			type = PT_STRING; return true;
		}
		else if (name == objToken[TOKEN_OBJ_POSITION])
		{
			type = PT_VECTOR3; return true;
		}
		else if (name == objToken[TOKEN_OBJ_ORITATION])
		{
			type = PT_QUATERNION; return true;
		}
		else if (name == objToken[TOKEN_OBJ_PITCH])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_OBJ_YAW])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_OBJ_ROLL])
		{
			type = PT_REAL; return true;
		}
		else if (name == objToken[TOKEN_OBJ_SCALE])
		{
			type = PT_VECTOR3; return true;
		}
		else if (name == objToken[TOKEN_OBJ_MIRROR])
		{
			type = PT_MIRROR; return true;
		}
		else if (name == objToken[TOKEN_OBJ_PIROR])
		{
			type = PT_PIROR; return true;
		}
		else if (name == objToken[TOKEN_OBJ_RESOURCE])
		{
			type = PT_STRING; return true;
		}
		else if (name == objToken[TOKEN_OBJ_NOTE])
		{
			type = PT_STRING; return true;
		}
		return false;
	}

	bool GameObject::getPropertyValue(const String & name, String &value)
	{
		if (name == objToken[TOKEN_OBJ_NAME])
		{
			value = m_Name;
			return true;
		}
		else if (name == objToken[TOKEN_OBJ_POSITION])
		{
			value = StringUtil::ToString(m_Position);
			return true;
		}
		else if (name == objToken[TOKEN_OBJ_ORITATION])
		{
			String w = StringUtil::ToString(m_Oritation.w);
			String x = StringUtil::ToString(m_Oritation.x);
			String y = StringUtil::ToString(m_Oritation.y);
			String z = StringUtil::ToString(m_Oritation.z);
			// value = StringUtil::ToString(m_Oritation); return true;
			value = w + " " + x + " " + y + " " + z;
			return true;
		}
		else if (name == objToken[TOKEN_OBJ_PITCH])
		{
			value = StringUtil::ToString(m_Pitch); return true;
		}
		else if (name == objToken[TOKEN_OBJ_YAW])
		{
			value = StringUtil::ToString(m_Yaw); return true;
		}
		else if (name == objToken[TOKEN_OBJ_ROLL])
		{
			value = StringUtil::ToString(m_Roll); return true;
		}
		else if (name == objToken[TOKEN_OBJ_SCALE])
		{
			value = StringUtil::ToString(m_Scale); return true;
		}
		else if (name == objToken[TOKEN_OBJ_MIRROR])
		{
			value = getMirrorTypeString(); return true;
		}
		else if (name == objToken[TOKEN_OBJ_PIROR])
		{
			value = StringUtil::ToString(m_Piror); return true;
		}
		else if (name == objToken[TOKEN_OBJ_RESOURCE])
		{
			value = m_Resource; return true;
		}
		else if (name == objToken[TOKEN_OBJ_NOTE])
		{
			value = m_Note; return true;
		}
		return false;
	}

	bool GameObject::setPropertyValue(const String & name, const String & value)
	{
		if (name == objToken[TOKEN_OBJ_POSITION])
		{
			m_Position = StringUtil::ParseVec3(value);

			setPosition(m_Position);
			return true;
		}
		else if (name == objToken[TOKEN_OBJ_ORITATION])
		{
			Vector4 oritation = StringUtil::ParseVec4(value);
			Quaternion q(oritation.x, oritation.y, oritation.z, oritation.w);
			setOritation(q);
			if (m_IOprepared)
				reCaculateAngle();
			return true;
		}
		else if (name == objToken[TOKEN_OBJ_PITCH])
		{
			m_Pitch = StringUtil::ParseReal(value);
			if (m_IOprepared)
			{
				Quaternion q = reCaculateOritation();
				setOritation(q);
			}

			return true;
		}
		else if (name == objToken[TOKEN_OBJ_YAW])
		{
			m_Yaw = StringUtil::ParseReal(value);
			if (m_IOprepared)
			{
				Quaternion q = reCaculateOritation();
				setOritation(q);
			}
			return true;
		}
		else if (name == objToken[TOKEN_OBJ_ROLL])
		{
			m_Roll = StringUtil::ParseReal(value);
			if (m_IOprepared)
			{
				Quaternion q = reCaculateOritation();
				setOritation(q);
			}
			return true;
		}
		else if (name == objToken[TOKEN_OBJ_SCALE])
		{
			m_Scale = StringUtil::ParseVec3(value);
			setScale(m_Scale);
			return true;
		}
		else if (name == objToken[TOKEN_OBJ_MIRROR])
		{
			setMirrorType(value); return true;
		}
		else if (name == objToken[TOKEN_OBJ_PIROR])
		{
			m_Piror = StringUtil::ParseUI32(value); return true;
		}
		else if (name == objToken[TOKEN_OBJ_RESOURCE])
		{
			assert(m_IOprepared == false);
			m_Resource = value; return true;
		}
		else if (name == objToken[TOKEN_OBJ_NOTE])
		{
			m_Note = value; return true;
		}
		return false;
	}

	bool GameObject::editor_IsCanTranslate() const
	{
		return editor_canTranslate;
	}

	bool GameObject::editor_IsCanRotate() const
	{
		return editor_canRotate;
	}

	bool GameObject::editor_IsCanScale() const
	{
		return editor_canScale;
	}

	bool GameObject::editor_IsCanMirror() const
	{
		return editor_canMirror;
	}

	void GameObject::editor_SetVisualHelpFront(bool bFront)
	{
		//if (editor_selectVH)
		//	editor_selectVH->setDepthEnable(bFront);
		//if (editor_hoverVH)		
		//	editor_hoverVH->setDepthEnable(bFront);
	}

	void GameObject::setFadeIn(bool bFade)
	{

	}

	bool GameObject::isFadeIn() const
	{
		return false;
	}

	bool GameObject::isNormalFade() const
	{
		return true;
	}

	void GameObject::setSceneCreate(bool bscene)
	{
		m_bSceneCreate = bscene;
	}

	bool GameObject::IsSceneCreate() const
	{
		return m_bSceneCreate;
	}

	void GameObject::frameMove(i32 deltaTime)
	{

	}

	bool GameObject::exportData(rapidxml::xml_node<>* parentNode, rapidxml::xml_document<>& doc)
	{
		// write property
		GameObject::PropertyList propertyList;
		getPropertyList(propertyList);
		GameObject::PropertyList::iterator itor = propertyList.begin();
		for (; itor != propertyList.end(); ++itor)
		{
			String propertyName = (*itor).first;
			String proertyValue;
			// oritation save this ortation property
			if (propertyName == objToken[TOKEN_OBJ_PITCH] ||
				propertyName == objToken[TOKEN_OBJ_YAW] ||
				propertyName == objToken[TOKEN_OBJ_ROLL])
			{
				continue;
			}

			getPropertyValue(propertyName, proertyValue);
			char* strPropertyName = doc.allocate_string(propertyName.c_str());
			char* strPropertyValue = doc.allocate_string(proertyValue.c_str());

			rapidxml::xml_node<>* propertyNode = doc.allocate_node(rapidxml::node_element, "Property", NULL);
			propertyNode->append_attribute(doc.allocate_attribute("name", strPropertyName));
			propertyNode->append_attribute(doc.allocate_attribute("value", strPropertyValue));

			parentNode->append_node(propertyNode);
		}

		return true;
	}

	bool GameObject::importData(rapidxml::xml_node<>* parentNode)
	{
		// parse property nodes
		rapidxml::xml_node<> *pPropertyNode = parentNode->first_node();
		while (pPropertyNode)
		{
			rapidxml::xml_attribute<> *pPropertyName = pPropertyNode->first_attribute();
			String propertyName = pPropertyName->value();
			rapidxml::xml_attribute<> *pPropertyValue = pPropertyName->next_attribute();
			String propertyValue = pPropertyValue->value();

			setPropertyValue(propertyName, propertyValue);

			pPropertyNode = pPropertyNode->next_sibling();
		}

		return true;
	}


	bool GameObject::exportData(rapidjson::Value& node, rapidjson::Document* doc)
	{
		GameObject::PropertyList propertyList;
		getPropertyList(propertyList);
		GameObject::PropertyList::iterator itor = propertyList.begin();


		rapidjson::Value propertyArray(rapidjson::kObjectType);
		node.AddMember("Property", propertyArray, doc->GetAllocator());

		rapidjson::Value& refProperty = node["Property"];
		for (; itor != propertyList.end(); ++itor)
		{
			String propertyName = (*itor).first;
			String propertyValue;
			// oritation save this ortation property
			if (propertyName == objToken[TOKEN_OBJ_PITCH] ||
				propertyName == objToken[TOKEN_OBJ_YAW] ||
				propertyName == objToken[TOKEN_OBJ_ROLL])
			{
				continue;
			}

			getPropertyValue(propertyName, propertyValue);

			rapidjson::Value valPropertyName(propertyName.c_str(), doc->GetAllocator());
			rapidjson::Value valPropertyValue(propertyValue.c_str(), doc->GetAllocator());
			refProperty.AddMember(valPropertyName, valPropertyValue, doc->GetAllocator());
		}

		return true;
	}

	bool GameObject::importData(rapidjson::Value& node)
	{
		rapidjson::Value::ConstMemberIterator itPropertyNode = node.MemberBegin();

		for (; itPropertyNode != node.MemberEnd(); ++itPropertyNode)
		{
			String propertyName = itPropertyNode->name.GetString();
			String propertyValue = itPropertyNode->value.GetString();

			setPropertyValue(propertyName, propertyValue);
		}

		return true;
	}

	//---------------------------class ObjectPrepareEvent-----------------------
	ObjectPrepareEvent::ObjectPrepareEvent(GameObject* pObject)
		: m_pObject(pObject)
	{
		m_pObject->m_bRequestPrepare = true;
	}

	ObjectPrepareEvent::~ObjectPrepareEvent()
	{
		m_pObject = NULL;
	}

	bool ObjectPrepareEvent::ProcessEvent()
	{
		if (m_pObject)
		{
			m_pObject->_prepare_io();
			return true;
		}
		return false;
	}

	bool ObjectPrepareEvent::RespondEvent()
	{
		if (m_pObject)
		{
			m_pObject->_prepare_obj();
			m_pObject->m_bRequestPrepare = false;
			return true;
		}
		return false;
	}

}