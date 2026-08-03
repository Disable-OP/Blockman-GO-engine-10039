/********************************************************************
filename: 	ModelBase.h
file path:	dev\client\Src\Blockman\Model

version:	1
author:		ajohn
company:	supernano
date:		2017-3-8
*********************************************************************/
#ifndef __MODEL_BASE_HEADER__
#define __MODEL_BASE_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class ModelRenderer;
class Entity;
class EntityLivingBase;
class ModelBox;
class Random;

class ModelBase : public ObjectAlloc
{
public:
	typedef vector<ModelRenderer*>::type ModelRenderArr;
	typedef map<String, Vector2i>::type TextureOffsetMap;

protected:
	TextureOffsetMap modelTextureMap;

public:
	float swingProgress = 0.f;
	bool isRiding = false;
	bool isChild = true;		// = true;
	ModelRenderArr boxList;

protected:
	void setTextureOffset(const String& partName, int x, int y);
	
public:
	ModelBase();
	virtual ~ModelBase();
	virtual int getBoneCount() = 0;

	virtual bool compileSkinMesh(SkinedRenderable* renderer, const Vector2i& begin, const Vector2i& size) = 0;
	virtual bool compileTessRender(TessRenderable* renderer, const Vector3& pos, int bright, const Vector2i& begin, const Vector2i& size) { return false; }
	virtual ModelBox* getModelBox(int i) = 0;
	virtual void updateBones(Vector4* outMatrices) = 0;
	
	/** Sets the model's various rotation angles. For bipeds, limbSwing and limbSwingAmount are used for animating the movement of arms
	* and legs, where limbSwing represents the time(so that arms and legs swing back and forth) and limbSwingAmount represents how
	* "far" arms and legs can swing at most. */
	virtual void setRotationAngles(float limbSwing, float swingAmount, float ticks, float headYaw, float headPitch, Entity* entityIn) { }

	/** Used for easily adding entity-dependent animations. The second and third float params here are the same second
	* and third as in the setRotationAngles method. */
	virtual void setLivingAnimations(EntityLivingBase* pLiving, float limbSwing, float swingAmount, float ticks) { }

	ModelRenderer* getRandomModelBox(Random* rand);
	Vector2i getTextureOffset(const String& partName);
	
	void setModelAttributes(ModelBase* model);
};

}

#endif