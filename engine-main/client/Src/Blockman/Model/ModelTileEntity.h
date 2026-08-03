/********************************************************************
filename: 	ModelChest.h
file path:	dev\client\Src\Blockman\Model

version:	1
author:		ajohn
company:	supernano
date:		2017-08-08
*********************************************************************/
#ifndef __MODEL_BIPED_HEADER__
#define __MODEL_BIPED_HEADER__

#include "ModelBase.h"

namespace BLOCKMAN
{

class ModelRenderer;
class ModelBox;

class ModelChest : public ModelBase
{
public:
	ModelRenderer* mModelRender = nullptr;
	bool m_large = false;
	ModelBox* b_below = nullptr;
	ModelBox* b_lid = nullptr;
	ModelBox* b_knob = nullptr;

	enum CHEST_BONE_IDX
	{
		BBI_BELOW = 0,
		BBI_LID,
		BBI_NKOB,
	};

public:
	ModelChest(bool isLarge = false);
	virtual ~ModelChest() override;

	/** implement override functions from ModelBase. */
	virtual int getBoneCount();
	virtual bool compileSkinMesh(SkinedRenderable* renderer, const Vector2i& begin, const Vector2i& size);
	virtual bool compileTessRender(TessRenderable* renderer, const Matrix4& mat, const Vector2i& begin, const Vector2i& size, int bright);
	virtual ModelBox* getModelBox(int i);
	virtual void updateBones(Vector4* outMatrices);

protected:
	void createNormal();
	void createLarge();
};

class ModelSign : public ModelBase
{
public:
	ModelRenderer* mModelRender = nullptr;
	ModelBox* b_board = nullptr;
	ModelBox* b_stick = nullptr;

	enum SIGN_BONE_IDX
	{
		SBI_BOARD = 0,
		SBI_STICK,
	};

public:
	ModelSign(bool withStick);
	virtual ~ModelSign();

	/** implement override functions from ModelBase. */
	virtual int getBoneCount();
	virtual bool compileSkinMesh(SkinedRenderable* renderer, const Vector2i& begin, const Vector2i& size);
	virtual bool compileTessRender(TessRenderable* renderer, const Matrix4& mat, const Vector2i& begin, const Vector2i& size, int bright);
	virtual ModelBox* getModelBox(int i);
	virtual void updateBones(Vector4* outMatrices);
};

}

#endif