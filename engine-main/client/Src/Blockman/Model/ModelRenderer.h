/********************************************************************
filename: 	ModelRender.h
file path:	dev\client\Src\Blockman\Model

version:	1
author:		ajohn
company:	supernano
date:		2017-3-8
*********************************************************************/
#ifndef __MODEL_RENDER_HEADER__
#define __MODEL_RENDER_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class ModelBox;
class ModelBase;

class ModelRenderer : public ObjectAlloc
{
public:
	typedef vector<ModelBox*>::type ModelBoxArr;
	typedef vector<ModelRenderer*>::type ModelRenderArr;

protected:
	bool compiled = false;
	/** The GL display list rendered by the Tessellator for this model */
	int displayList = 0;
	ModelBase* baseModel = nullptr;

public:
	bool showModel = false;

	ModelBoxArr cubeList;
	ModelRenderArr childModels;
	String m_name;

protected:
	/** Compiles a GL display list for this model */
	void compile();
	void constructor();

public:
	ModelRenderer(ModelBase* model, const String& boxNameIn);
	ModelRenderer(ModelBase* model);
	virtual ~ModelRenderer();

	/** Sets the current box's rotation points and rotation angles to another box. */
	void addChild(ModelRenderer* renderer);
	
	ModelBox* addBox(int boneIdx);
	int getBoxCount() const { return int(cubeList.size()); }
	ModelBox* getBox(int index) { return cubeList[index]; }
	
	void render();
	void renderWithRotation();
	/** Allows the changing of Angles after a box has been rendered */
	void postRender();
};

}

#endif