#include "ModelTileEntity.h"
#include "ModelRenderer.h"
#include "ModelBox.h"

namespace BLOCKMAN
{

ModelChest::ModelChest(bool isLarge)
	: m_large(isLarge)
{
	mModelRender = LordNew ModelRenderer(this, "chest");
	
	if (isLarge)
	{
		createLarge();
	}
	else
	{
		createNormal();
	}
}

ModelChest::~ModelChest()
{
	LordSafeDelete(mModelRender);
}

void ModelChest::createNormal()
{
	float scale = 0.0625f;

	b_below = mModelRender->addBox(BBI_BELOW);
	b_below->setPosition(Vector3(-7.f, 0.f, -7.F) * scale, Vector3(14.f, 10.f, 14.f) * scale, 0.f);
	b_below->setTexCoord(Vector2i(0, 19), Vector3i(14, 10, 14));
	b_below->setRotateAxis(Vector3(0.F, 0.F, 0.F) * scale);
	b_below->finish();

	b_lid = mModelRender->addBox(BBI_LID);
	b_lid->setPosition(Vector3(-7.F, -1.F, -14.F) * scale, Vector3(14.f, 5.f, 14.f) * scale, 0.f);
	b_lid->setTexCoord(Vector2i(0, 0), Vector3i(14, 5, 14));
	b_lid->setRotateAxis(Vector3(0.f, 10.f, 7.f) * scale);
	b_lid->finish();

	b_knob = mModelRender->addBox(BBI_NKOB);
	b_knob->setPosition(Vector3(-1.0F, -2.0F, -15.0F) * scale, Vector3(2.f, 4.f, 1.f) * scale, 0.f);
	b_knob->setTexCoord(Vector2i(0, 0), Vector3i(2, 4, 1));
	b_knob->setRotateAxis(Vector3(0.f, 10.f, 7.f) * scale);
	b_knob->finish();
}

void ModelChest::createLarge()
{
	float scale = 0.0625f;
	
	b_below = mModelRender->addBox(BBI_BELOW);
	b_below->setPosition(Vector3(-7.f, 0.f, 0.F) * scale, Vector3(30, 10.f, 14.f) * scale, 0.f);
	b_below->setTexCoord(Vector2i(0, 19), Vector3i(30, 10, 14));
	b_below->setRotateAxis(Vector3(0.F, 0.F, 0.F) * scale);
	b_below->finish();

	b_lid = mModelRender->addBox(BBI_LID);
	b_lid->setPosition(Vector3(-7.F, -1.F, -14.F) * scale, Vector3(30.f, 5.f, 14.f) * scale, 0.f);
	b_lid->setTexCoord(Vector2i(0, 0), Vector3i(30, 5, 14));
	b_lid->setRotateAxis(Vector3(0.f, 10.f, 14.f) * scale);
	b_lid->finish();

	b_knob = mModelRender->addBox(BBI_NKOB);
	b_knob->setPosition(Vector3(-1.0F, -2.0F, -15.0F) * scale, Vector3(2.f, 4.f, 1.f) * scale, 0.f);
	b_knob->setTexCoord(Vector2i(0, 0), Vector3i(2, 4, 1));
	b_knob->setRotateAxis(Vector3(8.f, 10.f, 14) * scale);
	b_knob->finish();
}

int ModelChest::getBoneCount()
{
	return mModelRender->getBoxCount();
}

ModelBox* ModelChest::getModelBox(int i)
{
	LordAssert(i >= 0 && i < mModelRender->getBoxCount());
	return mModelRender->getBox(i);
}

bool ModelChest::compileSkinMesh(SkinedRenderable* renderer, const Vector2i& pos, const Vector2i& size)
{
	int boneCount = mModelRender->getBoxCount();
	for (int i = 0; i < boneCount; ++i)
	{
		ModelBox* pBox = mModelRender->getBox(i);
		pBox->render(renderer, pos, size);
	}
	return true;
}

bool ModelChest::compileTessRender(TessRenderable* renderer, const Matrix4& mat, const Vector2i& begin, const Vector2i& size, int bright)
{
	int boneCount = mModelRender->getBoxCount();
	for (int i = 0; i < boneCount; ++i)
	{
		ModelBox* pBox = mModelRender->getBox(i);
		Matrix4 matBone = pBox->getBoneMatrix();;
		matBone = mat * matBone;

		pBox->render(renderer, matBone, begin, size, bright);
	}
	return true;
}

void ModelChest::updateBones(Vector4* outMatrices)
{
	int boneCount = mModelRender->getBoxCount();
	for (int i = 0; i < boneCount; ++i)
	{
		ModelBox* pBox = mModelRender->getBox(i);
		pBox->caculateBoneMatrix(outMatrices + i * 3);
	}
}

ModelSign::ModelSign(bool withStick)
{
	mModelRender = LordNew ModelRenderer(this, "sign");
	float scale = 0.0625f;

	b_board = mModelRender->addBox(SBI_BOARD);
	b_board->setPosition(Vector3(-12.f, 12.f, -1.F) * scale, Vector3(24.f, 12.f, 2.f) * scale, 0.f);
	b_board->setTexCoord(Vector2i(0, 0), Vector3i(24, 12, 2));
	b_board->setRotateAxis(Vector3(0.F, 0.F, 0.F) * scale);
	b_board->finish();

	if (withStick)
	{
		b_stick = mModelRender->addBox(SBI_STICK);
		b_stick->setPosition(Vector3(-1.F, -2.F, -1.F) * scale, Vector3(2.f, 14.f, 2.f) * scale, 0.f);
		b_stick->setTexCoord(Vector2i(0, 14), Vector3i(2, 14, 2));
		b_stick->setRotateAxis(Vector3(0.f, 0.f, 0.f) * scale);
		b_stick->finish();
	}
}

ModelSign::~ModelSign()
{
	LordSafeDelete(mModelRender);
}


int ModelSign::getBoneCount()
{
	return mModelRender->getBoxCount();
}

ModelBox* ModelSign::getModelBox(int i)
{
	LordAssert(i >= 0 && i < mModelRender->getBoxCount());
	return mModelRender->getBox(i);
}

bool ModelSign::compileSkinMesh(SkinedRenderable* renderer, const Vector2i& pos, const Vector2i& size)
{
	int boneCount = mModelRender->getBoxCount();
	for (int i = 0; i < boneCount; ++i)
	{
		ModelBox* pBox = mModelRender->getBox(i);
		pBox->render(renderer, pos, size);
	}
	return true;
}

bool ModelSign::compileTessRender(TessRenderable* renderer, const Matrix4& mat, const Vector2i& begin, const Vector2i& size, int bright)
{
	int boneCount = mModelRender->getBoxCount();
	for (int i = 0; i < boneCount; ++i)
	{
		ModelBox* pBox = mModelRender->getBox(i);
		Matrix4 matBone = pBox->getBoneMatrix();;
		matBone = mat * matBone;

		pBox->render(renderer, matBone, begin, size, bright);
	}
	return true;
}

void ModelSign::updateBones(Vector4* outMatrices)
{
	int boneCount = mModelRender->getBoxCount();
	for (int i = 0; i < boneCount; ++i)
	{
		ModelBox* pBox = mModelRender->getBox(i);
		pBox->caculateBoneMatrix(outMatrices + i * 3);
	}
}

}