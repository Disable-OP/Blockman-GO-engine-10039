#include "ModelBase.h"
#include "ModelRenderer.h"

#include "Util/Random.h"

namespace BLOCKMAN
{

ModelBase::ModelBase()
	: swingProgress(0.f)
	, isRiding(false)
	, isChild(true)
{}

ModelBase::~ModelBase()
{

}

void ModelBase::setTextureOffset(const String& partName, int x, int y)
{
	modelTextureMap.insert(std::make_pair(partName, Vector2i(x, y)));
}

ModelRenderer* ModelBase::getRandomModelBox(Random* rand)
{
	return boxList[rand->nextInt(boxList.size())];
}

Vector2i ModelBase::getTextureOffset(const String& partName)
{
	return modelTextureMap[partName];
}

void ModelBase::setModelAttributes(ModelBase* model)
{
	swingProgress = model->swingProgress;
	isRiding = model->isRiding;
	isChild = model->isChild;
}

}