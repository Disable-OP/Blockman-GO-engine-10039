/********************************************************************
filename: 	SkinnedMesh.h
file path:	dev\engine\Src\Core\Model\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_SKINNEDMESH_H__
#define __LORD_SKINNEDMESH_H__

#include "Mesh.h"

namespace LORD
{
	class LORD_CORE_API SkinnedMesh: public Mesh
	{
	public:
		friend class MeshManager;
		SkinnedMesh(const String& name);
		virtual ~SkinnedMesh();

		void MergeSubMesh(const map<SubMesh*, Vector4>::type& submeshes, const map<SubMesh*, std::pair<Vector4, int>>::type& stsubmeshes);

		static const uint MAX_BONE_WEIGHT_NUM = 3;

#pragma pack(push, 1)
		struct SkinnedMeshVertex
		{
			Vector3	position;
			Vector3	normal;
			Vector2	texCoord;
			Byte	blendIndices[4];
			float	blendWeights[MAX_BONE_WEIGHT_NUM];
		};
#pragma pack(pop)

	protected:
		virtual bool			prepareImpl();
		//virtual bool			loadImpl();
	};

}

#endif