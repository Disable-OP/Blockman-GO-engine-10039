/********************************************************************
filename: 	ModelBiped.h
file path:	dev\client\Src\Blockman\Model

version:	1
author:		ajohn
company:	supernano
date:		2017-3-8
*********************************************************************/
#ifndef __MODEL_BIPED_HEADER__
#define __MODEL_BIPED_HEADER__

#include "ModelBase.h"

namespace BLOCKMAN
{

class ModelRenderer;
class ModelBox;

class ModelBiped : public ModelBase
{
public:
	enum BIP_BONE_IDX
	{
		BBI_BODY = 0,
		BBI_HEAD,
		BBI_ARM_R,
		BBI_ARM_L,
		BBI_LEG_R,
		BBI_LEG_L,
	//	BBI_SWEAR,
	//	BBI_EARS,
	//	BBI_CLOAK,
	};

	ModelRenderer* mModelRender = nullptr;

	ModelBox* b_body = nullptr;
	ModelBox* b_head = nullptr;
	ModelBox* b_arm_r = nullptr;
	ModelBox* b_arm_l = nullptr;
	ModelBox* b_leg_r = nullptr;
	ModelBox* b_leg_l = nullptr;
	
	/** Records whether the model should be rendered holding an item in the left hand, and if that item is a block. */
	int heldItemLeft = 0;
	/** Records whether the model should be rendered holding an item in the right hand, and if that item is a block. */
	int heldItemRight = 0;
	bool isSneak = false;
	/** Records whether the model should be rendered aiming a bow. */
	bool aimedBow = false;

public:
	ModelBiped(float extend, float yOffset);
	~ModelBiped();

	/** implement override functions from ModelBase. */
	virtual int getBoneCount();
	virtual bool compileSkinMesh(SkinedRenderable* renderer, const Vector2i& pos, const Vector2i& size);
	virtual ModelBox* getModelBox(int i);
	virtual void updateBones(Vector4* outMatrices);

	/** Sets the model's various rotation angles. For bipeds, par1 and par2 are used for animating the movement of arms
	* and legs, where par1 represents the time(so that arms and legs swing back and forth) and par2 represents how
	* "far" arms and legs can swing at most. */
	void setRotationAngles(float limbSwing, float swingAmount, float ticks, float headYaw, float headPitch, Entity* pEntity);
};

}

#endif