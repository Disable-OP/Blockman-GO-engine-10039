#include "Core.h"
#include "EffectLayer2DGrid.h"
#include "EffectSystemManager.h"
#include "Scene/SceneNode.h"

namespace LORD
{
	EffectLayer2DGrid::EffectLayer2DGrid()
		: EffectLayer3DGrid() 
	{
		mType = ELT_2DEditableSurface;
	}

	EffectLayer2DGrid::~EffectLayer2DGrid()
	{

	}

	void EffectLayer2DGrid::getMatrixWorld(Matrix4& mtxWorld)
	{
		SceneNode* parentNode = mParentSystem->getEffectSceneNode();

		Matrix4 mtxLocal;
		mtxLocal.makeScaling(scale, scale, scale);
		mCurrentCamOritation = mParentSystem->getCurrentCamera()->getOritation();
		Matrix4 mtxRot;
		Quaternion quanRot;
		quanRot.rotateAxis(originalDirection, selfRotation*Math::DEG2RAD);
		quanRot = quanRot*mCurrentCamOritation;
		quanRot.toMat4(mtxRot);
		mtxLocal = mtxLocal * mtxRot;
		mtxLocal.translate(position);

		Vector3 scale = parentNode->getWorldScaling();
		Vector3 pos = parentNode->getWorldPosition();
		mtxWorld.makeScaling(scale);
		mtxWorld.translate(pos);

		mtxWorld = mtxLocal * mtxWorld;
	}

	const Quaternion& EffectLayer2DGrid::getSurfaceOritation()const
	{
		return mCurrentCamOritation;
	}
}
