#include "Core.h"
#include "EffectLayerModel.h"
#include "EffectSystemManager.h"
#include "Model/MeshManager.h"
#include "Model/Mesh.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneNode.h"
#include "Resource/DataStream.h"
#include "Actor/ActorManager.h"

namespace LORD
{
	EffectLayerModel::EffectLayerModel()
		: mbIsTransparent(false)
		, mModelName(StringUtil::BLANK)
		, mModelNode(NULL)
		, mModelEntity(NULL)
		, mMesh(NULL)
	{
		originalDirection = Vector3::UNIT_Z;
		direction = Vector3::UNIT_Z;
		mType = ELT_Model;
	}

	EffectLayerModel::~EffectLayerModel()
	{

	}

	void EffectLayerModel::prepare()
	{
		EffectLayer::prepare();

		if(mModelName != StringUtil::BLANK && mParentSystem->getEffectSceneNode())
		{
			mMesh = MeshManager::Instance()->createMesh(mModelName, false, true);
			mMesh->load();

			mModelEntity = LordNew ModelEntity;
			mModelEntity->setMesh(mMesh);

			mModelNode = mParentSystem->getEffectSceneNode()->createChild();
			if (mbIsTransparent)
			{
				mModelEntity->setRenderType(ModelEntity::RT_FOR_ACTOR_TRANSPARENT);
				mModelEntity->setMaterial(ActorManager::Instance()->GetStaticMaterial_transparent());
			}
			else
			{
				mModelEntity->setRenderType(ModelEntity::RT_FOR_ACTOR);
				mModelEntity->setMaterial(ActorManager::Instance()->GetStaticMaterial());
			}
			mModelEntity->attachTo(mModelNode);
			mModelEntity->createRenderable();

			for (int i = 0; i < mMesh->getSubMeshNum(); ++i)
			{
				String name = mMesh->getTextureName(i, 0);
				Texture* pTex = TextureManager::Instance()->createTexture(name);
				if (pTex->load())
				{
					mModelEntity->replaceTexture(pTex, i, 0);
				}
				else
				{
					LordLogError("EffectLayerModel::prepare: Texture isn't exits %s.", name.c_str());
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
					char buf[MAX_PATH];
					sprintf_s(buf, MAX_PATH, "EffectLayerModel::prepare: Texture isn't exits %s.", name.c_str());
					MessageBox(0, buf, 0, 0);
#endif
				}
			}
		}
	}

	void EffectLayerModel::unprepare()
	{
		LordSafeDelete(mModelEntity);

		if(mModelNode)
		{
			mParentSystem->getEffectSceneNode()->destroyChild(mModelNode);
			mModelNode = NULL;
		}

		if(mMesh)
		{
			MeshManager::Instance()->releaseResource(mMesh);
			mMesh = NULL;
		}
	}

	int EffectLayerModel::_update(i32 time)
	{
		int ret = EffectLayer::_update(time);

		if(ret && mbAffected && mModelNode)
		{
			mModelNode->setLocalScaling(Vector3(scale, scale, scale));
			mModelNode->setLocalPosition(position);
			mModelNode->setLocalOrientation(oritation);
            mModelNode->update();
		}
		return ret;
	}

	void EffectLayerModel::_updateRenderQueue()
	{
		if ((!mbAffected) || (!mbVisible) )
			return;

		if(mModelEntity && mModelNode)
		{
			const Matrix4* viewProj = mParentSystem->getIsUIEffect() ? (&SceneManager::Instance()->getGUICamera()->getViewProjMatrix()) : NULL;
			mModelEntity->update(viewProj);
			mModelEntity->setNaturalColor(color);
			mModelEntity->updateRenderQueue(SceneManager::Instance()->getRenderQueue("Scene"));
		}
	}

	void EffectLayerModel::importData(DataStream* pStream, int version)
	{
		EffectLayer::importData(pStream, version);
		pStream->read(&mbIsTransparent,sizeof(bool));
		size_t strLength;
		pStream->read(&strLength, sizeof(strLength));
		char str[128];
		pStream->read(str, strLength);
		str[strLength] = 0;
		mModelName = str;
	}

	void EffectLayerModel::exportData(DataStream* pStream, int version)
	{
		EffectLayer::exportData(pStream, version);
		pStream->write(&mbIsTransparent,sizeof(bool));
		ui32 strlength = (ui32)mModelName.length();
		pStream->write(&strlength, sizeof(ui32));
		pStream->write(mModelName.c_str(), strlength);
	}

	void EffectLayerModel::copyAttributesTo(EffectParticle* particle)
	{
		EffectLayer::copyAttributesTo(particle);
		EffectLayerModel* model = static_cast<EffectLayerModel*>(particle);
		model->mModelName = mModelName;
		model->mbIsTransparent = mbIsTransparent;
	}

	void EffectLayerModel::mergeAligenBox(Box& aabb)
	{
		if(mModelNode && mMesh)
		{
			mModelNode->setLocalScaling(Vector3(scale, scale, scale));
			mModelNode->setLocalPosition(position);
			mModelNode->setLocalOrientation(oritation);
            mModelNode->update();
			Box modelBox = mMesh->getBox();
			Matrix4 worldTrans = mModelNode->getWorldMatrix();
			modelBox.transform(worldTrans);
			aabb.unionBox(modelBox);
		}
	}

	void EffectLayerModel::getPropertyList(PropertyList& list)
	{
		EffectLayer::getPropertyList(list);
		list.push_back(token[TOKEN_LY_SMD_TRANSPARENT]);
		list.push_back(token[TOKEN_LY_SMD_NAME]);
	}

	bool EffectLayerModel::getPropertyType(const String& name, PropertyType& type)
	{
		if(EffectLayer::getPropertyType(name, type))
			return true;
		if(name == token[TOKEN_LY_SMD_TRANSPARENT])
		{
			type = IElement::PT_BOOL; return true;
		}
		else if(name == token[TOKEN_LY_SMD_NAME])
		{
			type = IElement::PT_MODEL_NAME_TYPE; return true;
		}
		return false;
	}


	bool EffectLayerModel::getPropertyValue(const String& name, String& value)
	{
		if(EffectLayer::getPropertyValue(name, value))
			return true;

		if(name == token[TOKEN_LY_SMD_TRANSPARENT])
		{
			value = StringUtil::ToString(mbIsTransparent); return true;
		}
		else if(name == token[TOKEN_LY_SMD_NAME])
		{
			value = mModelName; return true;
		}
		return false;
	}

	bool EffectLayerModel::setPropertyValue(const String& name,const String& value)
	{
		if(EffectLayer::setPropertyValue(name, value))
			return true;

		if(name == token[TOKEN_LY_SMD_TRANSPARENT])
		{
			mbIsTransparent = StringUtil::ParseBool(value); return true;
		}
		else if(name == token[TOKEN_LY_SMD_NAME])
		{
			mModelName = value; return true;
		}

		return false;
	}
}