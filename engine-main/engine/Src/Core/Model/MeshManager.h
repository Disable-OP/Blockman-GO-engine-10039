/********************************************************************
filename: 	MeshManager.h
file path:	dev\engine\Src\Core\Model\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_MESHMANAGER_H__
#define __LORD_MESHMANAGER_H__

#include "Core.h"
#include "Resource/ResourceManager.h"
#include "Object/Singleton.h"

namespace LORD
{
	class Mesh;
	class SkinnedMesh;

	class LORD_CORE_API MeshManager : public ResourceManager, public Singleton<MeshManager>
	{
	public:
		inline static MeshManager* Instance()
		{
			return ms_pSingleton;
		}

	public:
		MeshManager();
		virtual ~MeshManager();

	public:
		Mesh*			createMesh(const String& name, bool needBackUp = false, bool hasMaskTexture = false);
		SkinnedMesh*	createSkinnedMesh(const String& name, bool needBackUp = false, bool hasMaskTexture = false);

	protected:
		/// @copydoc ResourceManager::createImpl
		Resource* createImpl(const String& name, ResourceType rt = RESOURCE_TYPE_NORMAL);

	protected:
		bool		m_bSkinMesh;
	};
}

#endif
