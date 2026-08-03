#include "ModelRenderer.h"

#include "ModelBase.h"
#include "ModelBox.h"

namespace BLOCKMAN
{

void ModelRenderer::constructor()
{
	compiled = false;
	displayList = false;
	showModel = true;
}

ModelRenderer::ModelRenderer(ModelBase* model, const String& boxNameIn)
	: baseModel(model)
	, m_name(boxNameIn)
{
	constructor();
	model->boxList.push_back(this);
}

ModelRenderer::ModelRenderer(ModelBase* model)
	: baseModel(model)
{
	constructor();
	model->boxList.push_back(this);
}

ModelRenderer::~ModelRenderer()
{
	for (ModelBoxArr::iterator it = cubeList.begin(); it != cubeList.end(); ++it)
	{
		LordSafeDelete(*it);
	}
	cubeList.clear();
}

void ModelRenderer::addChild(ModelRenderer* renderer)
{
	childModels.push_back(renderer);
}

ModelBox* ModelRenderer::addBox(int boneIdx)
{
	LordAssert(boneIdx == cubeList.size());
	ModelBox* pbox = LordNew ModelBox(this, boneIdx);
	cubeList.push_back(pbox);
	pbox->setBoxName(StringUtil::Format("%s_d", m_name.c_str(), boneIdx));
	return pbox;
}

void ModelRenderer::render()
{
	if (!showModel)
		return;
	
	if (!compiled)
	{
		compile();
	}

	/*
	GlStateManager.translate(offsetX, offsetY, offsetZ);

	if (rotateAngle == Vector3::ZERO)
	{
		if (rotationPoint == Vector3::ZERO)
		{
			GlStateManager.callList(displayList);

			if (!childModels.empty())
			{
				for (int k = 0; k < childModels.size(); ++k)
				{
					((ModelRenderer)childModels.get(k)).render(scale);
				}
			}
		}
		else
		{
			GlStateManager.translate(rotationPointX * scale, rotationPointY * scale, rotationPointZ * scale);
			GlStateManager.callList(displayList);

			if (!childModels.empty())
			{
				for (int j = 0; j < childModels.size(); ++j)
				{
					((ModelRenderer)childModels.get(j)).render(scale);
				}
			}

			GlStateManager.translate(-rotationPointX * scale, -rotationPointY * scale, -rotationPointZ * scale);
		}
	}
	else
	{
		GlStateManager.pushMatrix();
		GlStateManager.translate(rotationPointX * scale, rotationPointY * scale, rotationPointZ * scale);

		if (rotateAngleZ != 0.0F)
		{
			GlStateManager.rotate(rotateAngleZ * (180F / (float)Math.PI), 0.0F, 0.0F, 1.0F);
		}

		if (rotateAngleY != 0.0F)
		{
			GlStateManager.rotate(rotateAngleY * (180F / (float)Math.PI), 0.0F, 1.0F, 0.0F);
		}

		if (rotateAngleX != 0.0F)
		{
			GlStateManager.rotate(rotateAngleX * (180F / (float)Math.PI), 1.0F, 0.0F, 0.0F);
		}

		GlStateManager.callList(displayList);

		if (childModels != null)
		{
			for (int i = 0; i < childModels.size(); ++i)
			{
				((ModelRenderer)childModels.get(i)).render(scale);
			}
		}

		GlStateManager.popMatrix();
	}

	GlStateManager.translate(-offsetX, -offsetY, -offsetZ);
	*/
}

void ModelRenderer::renderWithRotation()
{
	if (!showModel)
		return;


	if (!compiled)
	{
		compile();
	}
	/*
	GlStateManager.pushMatrix();
	GlStateManager.translate(rotationPointX * scale, rotationPointY * scale, rotationPointZ * scale);

	if (rotateAngleY != 0.0F)
	{
		GlStateManager.rotate(rotateAngleY * (180F / (float)Math.PI), 0.0F, 1.0F, 0.0F);
	}

	if (rotateAngleX != 0.0F)
	{
		GlStateManager.rotate(rotateAngleX * (180F / (float)Math.PI), 1.0F, 0.0F, 0.0F);
	}

	if (rotateAngleZ != 0.0F)
	{
		GlStateManager.rotate(rotateAngleZ * (180F / (float)Math.PI), 0.0F, 0.0F, 1.0F);
	}

	GlStateManager.callList(displayList);
	GlStateManager.popMatrix();
	*/
}

void ModelRenderer::postRender()
{
	if (!showModel)
		return;

	if (!compiled)
	{
		compile();
	}

	/*
	if (rotateAngleX == 0.0F && rotateAngleY == 0.0F && rotateAngleZ == 0.0F)
	{
		if (rotationPointX != 0.0F || rotationPointY != 0.0F || rotationPointZ != 0.0F)
		{
			GlStateManager.translate(rotationPointX * scale, rotationPointY * scale, rotationPointZ * scale);
		}
	}
	else
	{
		GlStateManager.translate(rotationPointX * scale, rotationPointY * scale, rotationPointZ * scale);

		if (rotateAngleZ != 0.0F)
		{
			GlStateManager.rotate(rotateAngleZ * (180F / (float)Math.PI), 0.0F, 0.0F, 1.0F);
		}

		if (rotateAngleY != 0.0F)
		{
			GlStateManager.rotate(rotateAngleY * (180F / (float)Math.PI), 0.0F, 1.0F, 0.0F);
		}

		if (rotateAngleX != 0.0F)
		{
			GlStateManager.rotate(rotateAngleX * (180F / (float)Math.PI), 1.0F, 0.0F, 0.0F);
		}
	}
	*/
}

void ModelRenderer::compile()
{
	/*displayList = GLAllocation.generateDisplayLists(1);
	GlStateManager.glNewList(displayList, 4864);
	VertexBuffer vertexbuffer = Tessellator.getInstance().getBuffer();

	for (int i = 0; i < cubeList.size(); ++i)
	{
		((ModelBox)cubeList.get(i)).render(vertexbuffer, scale);
	}

	GlStateManager.glEndList();
	compiled = true;*/
}

}