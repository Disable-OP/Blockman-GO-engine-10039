#include "MeshManager.h"
#include "Object/Exception.h"
#include "Util/PathUtil.h"
#include "SkinnedMesh.h"

namespace LORD
{
	//---------------------------------------------------------------------
	/*MeshManager* MeshManager::Instance()
	{
		return ms_pSingleton;
	}*/

	MeshManager::MeshManager()
		: m_bSkinMesh(false)
	{
	}

	MeshManager::~MeshManager()
	{
		// subclasses should unregister with resource group manager
	}

	Mesh* MeshManager::createMesh(const String& name,bool needBackUp, bool hasMaskTexture)
	{
		m_bSkinMesh = false;
		Mesh* pMesh =(Mesh*)ResourceManager::createResource(name, RESOURCE_TYPE_NORMAL);
		pMesh->m_bNeedBackUp = needBackUp;
		pMesh->m_bHasMaskTexture = hasMaskTexture;
		return pMesh;
	}

	SkinnedMesh* MeshManager::createSkinnedMesh(const String& name, bool needBackUp, bool hasMaskTexture)
	{
		m_bSkinMesh = true;
		SkinnedMesh *pSkinnedMesh =(SkinnedMesh*)ResourceManager::createResource(name, RESOURCE_TYPE_NORMAL);
		pSkinnedMesh->m_bNeedBackUp = needBackUp;
		pSkinnedMesh->m_bHasMaskTexture = hasMaskTexture;
		return pSkinnedMesh;
	}

	Resource* MeshManager::createImpl(const String& name, ResourceType rt)
	{
		if(m_bSkinMesh)
		{
			return LordNew SkinnedMesh(name);
		}
		else
		{
			return LordNew Mesh(name);
		}
	}
}
