/********************************************************************
filename: 	GUIImagesetManager.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_IMAGESET_MANAGER_H__
#define __GUI_IMAGESET_MANAGER_H__

#include "Core.h"
#include "Resource/ResourceManager.h"
#include "Object/Singleton.h"
#include "GUIString.h"

namespace LORD
{
	class GUIImageset;
	class GUIImage;

	class LORD_CORE_API GUIImagesetManager : public ResourceManager, public Singleton<GUIImagesetManager>
	{
	public:

		GUIImagesetManager();
		virtual ~GUIImagesetManager();

		static GUIImagesetManager* Instance();

		GUIImageset*	CreateFromFile(const GUIString& strName);

		GUIImageset*	CreateManual(const GUIString& strName, const int nTextureSize);

		void			ParseImageString(const GUIString& strWholeName, GUIString& strSetName, GUIString& strImageName);

		// 多线程接口
		GUIImageset*	createImageset(const GUIString& strName, ResourceType rt = RESOURCE_TYPE_NORMAL);
		GUIImageset*	createOrRetrieveImageset(const GUIString& strName, ResourceType rt = RESOURCE_TYPE_NORMAL);
		
		void			handleImagesetRelease();

		void			pushbackReleaseImageset(GUIImageset* imageset);

		GUIImageset*    findImageSet(const Texture* texture) const;

		void            registerGUIImageset(GUIImageset* imageset);

	protected:

		Resource*	createImpl(const String& name, ResourceType rt);

		vector<GUIImageset*>::type m_deleteList;
		map<const Texture*, GUIImageset*>::type m_textureToImageSetMap;
	};
}


#endif