#include "LightmapMgr.h"
#include "Object/Root.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "TextureSet.h"

#define INVALID -1

namespace LORD
{
	/*LightmapMgr* LightmapMgr::Instance()
	{
		return ms_pSingleton;
	}*/
	// 构造函数
	LightmapMgr::LightmapMgr()
		: m_tsWidth( 1024)
		, m_tsHeight( 1024)
	{}

	// 构造函数
	LightmapMgr::LightmapMgr( int width, int height)
		: m_tsWidth( width)
		, m_tsHeight( height)
	{}

	// 析构函数
	LightmapMgr::~LightmapMgr()
	{
		clear();
	}

	// 添加光照图(以实体名与mesh索引命名)
	bool LightmapMgr::addLightmap( const char* uniqueName, Color* texture, int width, int height, const Vector3& scale)
	{
		LordAssert( m_tsWidth>=width && m_tsHeight>=height);

		// 1.若已存在,覆写纹理
		TileInfoMap::iterator it = m_tileInfos.find( uniqueName);
		if( it != m_tileInfos.end())
		{
			TileInfo& tInfo = it->second;
			tInfo.m_scale = scale;

			return overWriteLightmapToTextureSet( m_texSets[tInfo.m_setIdx], tInfo.m_tileId, texture, width, height, scale);
		}

		// 2.遍历插入剩余空间
		for ( ui32 i=0; i<(ui32)m_texSets.size(); i++)
		{
			if( addLightmapToTextureSet( m_texSets[i], i, uniqueName, texture, width, height, scale))
				return true;
		}

		// 3.新建TextureSet 并插入
		TextureSet* texSet = allocateTextureSet( m_tsWidth, m_tsHeight);
		LordAssert( texSet);
		m_texSets.push_back( texSet);

		return addLightmapToTextureSet( m_texSets.back(), (int)m_texSets.size()-1, uniqueName, texture, width, height, scale);
	}

	// 查询
	bool LightmapMgr::query( const char* uniqueName, const Texture*& lightmap, Vector4& lightmapViewPort, Vector3& scale)
	{
		TileInfoMap::const_iterator it = m_tileInfos.find( uniqueName);
		if( it != m_tileInfos.end())
		{
			int	texIdx = it->second.m_setIdx;
			int	tileId = it->second.m_tileId;

			lightmap		 = m_texSets[texIdx]->GetTexture();
			lightmapViewPort = m_texSets[texIdx]->GetViewport(tileId);
			scale			 = it->second.m_scale;

			return true;
		}

		return false;
	}

	// 添加到指定纹理集
	bool LightmapMgr::addLightmapToTextureSet( TextureSet* texSet, int setIdx, const char* uniqueName, Color* texture, int width, int height, const Vector3& scale)
	{
		if( !texSet)
			return false;

		/*int tileID = texSet->Insert(texture, width, height);
		if( tileID != INVALID)
		{
			TileInfo tInfo;
			tInfo.m_uniqueName = uniqueName;
			tInfo.m_setIdx = setIdx;
			tInfo.m_tileId = tileID;
			tInfo.m_scale  = scale;

			m_tileInfos[uniqueName] = tInfo;

			return true;
		}*/
		
		return false;
	}

	// 覆写纹理集
	bool LightmapMgr::overWriteLightmapToTextureSet( TextureSet* texSet, int tileId, Color* texture, int width, int height, const Vector3& scale)
	{
		/*int tileIdx = texSet->OverWrite( tileId, texture, width, height);
		if( tileIdx!=INVALID)
			return true;*/

		return false;
	}

	// 清除所有
	void LightmapMgr::clear()
	{
		m_tileInfos.clear();

		for( size_t i=0; i<m_texSets.size(); i++)
			LordSafeDelete( m_texSets[i]);

		m_texSets.clear();
	}

	// 新建纹理集
	TextureSet* LightmapMgr::allocateTextureSet( int width, int height)
	{
		TextureSet* texSet = NULL;

		if (Root::Instance()->isEditorMode())
		{
			texSet  = LordNew TextureSet(width, height);
		}
		return texSet;
	}

	// 加载保存辅助 TileInfo
	struct TileInfoLS
	{
		char	m_uniqueName[64];	// 唯一标识
		int		m_setIdx;			// 纹理索引
		int		m_tileId;			// 纹理块ID
		Vector3 m_scale;			// 亮度缩放

		// 构造函数
		TileInfoLS()
			: m_setIdx(0), m_tileId(0)
		{
			m_uniqueName[0] = '\0';
		}
	};

	// 加载
	bool LightmapMgr::load()
	{
		String name = SceneManager::Instance()->getCurrentScene()->getSceneName() + ".lmb";
		DataStream* dataStream = ResourceGroupManager::Instance()->openResource(name);

		if (dataStream)
		{
			ui32 tileInfSize = 0;
			dataStream->read(&tileInfSize, sizeof(ui32));
			TileInfoLS tileInfoLS;
			for (size_t i=0; i<tileInfSize; ++i)
			{
				dataStream->read(&tileInfoLS, sizeof(TileInfoLS));
				TileInfo info;
				info.m_uniqueName = tileInfoLS.m_uniqueName;
				info.m_setIdx = tileInfoLS.m_setIdx;
				info.m_tileId = tileInfoLS.m_tileId;
				info.m_scale = tileInfoLS.m_scale;
				m_tileInfos[info.m_uniqueName] = info;
			}
			int texSets = 0;
			dataStream->read(&texSets, sizeof(int));

			char* texsetPath = static_cast<char*>(LordMalloc(64));
			for ( int i=0; i<texSets; i++)
			{
				dataStream->read(texsetPath, sizeof(char)*64);

				TextureSet* texSet = LordNew TextureSet;
				texSet->Load(String(texsetPath).c_str());
				m_texSets.push_back(texSet);
			}
			
			LordDelete dataStream;
			LordFree(texsetPath);
			
			return true;
		}

		//// 写文件
		//format txmlFmt( "%slightmap.lmb"); txmlFmt % locationPath ;
		//FILE* fileHandle = fopen( txmlFmt.str().c_str(), "rb");
		//if( fileHandle)
		//{
		//	// 保存TileInfo数量
		//	int tileInfSize = 0;
		//	fread( &tileInfSize, sizeof(ui32), 1, fileHandle);

		//	TileInfoLS tileInfoLS;
		//	for ( int i=0; i<tileInfSize; i++)
		//	{
		//		// 保存TileInfo
		//		fread( &tileInfoLS, sizeof(tileInfoLS), 1, fileHandle);

		//		TileInfo tInfo;
		//		tInfo.m_uniqueName = tileInfoLS.m_uniqueName;
		//		tInfo.m_setIdx	   = tileInfoLS.m_setIdx;
		//		tInfo.m_tileId	   = tileInfoLS.m_tileId;

		//		m_tileInfos[tInfo.m_uniqueName] = tInfo;
		//	}

		//	// 加载纹理顺序
		//	// 保存纹理顺序
		//	int texSets = 0;
		//	fread( &texSets, sizeof(texSets), 1, fileHandle);

		//	char texsetPath[64];
		//	for ( int i=0; i<texSets; i++)
		//	{
		//		fread( texsetPath, sizeof(char)*64, 1, fileHandle);

		//		TextureSet* texSet = LordNew TextureSet;
		//		texSet->Load( (string( locationPath)+string(texsetPath)).c_str());
		//		m_texSets.push_back( texSet);
		//	}

		//	fflush( fileHandle);
		//	fclose( fileHandle);

		//	return true;
		//}

		return false;
	}

	// 保存
	bool LightmapMgr::save(const String& locationPath)
	{
		LORD::String sceneName = LORD::SceneManager::Instance()->getCurrentScene()->getSceneName();
		String rootPath = locationPath + "\\" + sceneName + ".lmb";
		std::fstream f;
		f.open(rootPath.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
		LORD::DataStream* filePtr(LordNew LORD::FileStreamDataStream(&f, false));

		if (filePtr)
		{
			int tileInfSize = (int)m_tileInfos.size();
			filePtr->write(&tileInfSize, sizeof(int));
			TileInfoLS tileInfoLS;
			for ( TileInfoMap::iterator it=m_tileInfos.begin(); it!=m_tileInfos.end(); ++it)
			{
				strcpy( tileInfoLS.m_uniqueName, it->second.m_uniqueName.c_str());
				tileInfoLS.m_setIdx = it->second.m_setIdx;
				tileInfoLS.m_tileId = it->second.m_tileId;
				tileInfoLS.m_scale = it->second.m_scale;

				// 保存TileInfo
				filePtr->write(&tileInfoLS, sizeof(tileInfoLS));
			}
			// 保存纹理顺序
			int texSets = (int)m_texSets.size();
			filePtr->write(&texSets, sizeof(texSets));

			char texsetPath[64];
			for ( size_t i=0; i<m_texSets.size(); i++)
			{
				sprintf(texsetPath, (sceneName + "lightmap_%d.ts").c_str(), i);
				filePtr->write(texsetPath, sizeof(char)*64);
			}

			// 保存textureset
			int index =	(int)rootPath.find_last_of("\\");
			String newScenePath = rootPath.substr(0, index);
			for ( size_t i=0; i<m_texSets.size(); i++)
			{
				sprintf(texsetPath, (sceneName + "lightmap_%d.ts").c_str(), i);
				m_texSets[i]->Save((newScenePath + "\\" + texsetPath).c_str());
			}
			return true;
		}

		//// 写文件
		//format txmlFmt( "%slightmap.lmb"); txmlFmt % locationPath ;
		//FILE* fileHandle = fopen( txmlFmt.str().c_str(), "wb");
		//if( fileHandle)
		//{
		//	// 保存TileInfo数量
		//	int tileInfSize = (int)m_tileInfos.size();
		//	fwrite( &tileInfSize, sizeof(ui32), 1, fileHandle);

		//	TileInfoLS tileInfoLS;
		//	for ( TileInfoMap::iterator it=m_tileInfos.begin(); it!=m_tileInfos.end(); it++)
		//	{
		//		std::strcpy( tileInfoLS.m_uniqueName, it->second.m_uniqueName.c_str());
		//		tileInfoLS.m_setIdx = it->second.m_setIdx;
		//		tileInfoLS.m_tileId = it->second.m_tileId;

		//		// 保存TileInfo
		//		fwrite( &tileInfoLS, sizeof(tileInfoLS), 1, fileHandle);
		//	}

		//	// 保存纹理顺序
		//	int texSets = (int)m_texSets.size();
		//	fwrite( &texSets, sizeof(texSets), 1, fileHandle);

		//	char texsetPath[64];
		//	for ( size_t i=0; i<m_texSets.size(); i++)
		//	{
		//		format tFmt( "lightmap_%d.ts"); tFmt % i;
		//		std::strcpy( texsetPath, tFmt.str().c_str());

		//		fwrite( texsetPath, sizeof(char)*64, 1, fileHandle);
		//	}

		//	// 保存textureset
		//	for ( size_t i=0; i<m_texSets.size(); i++)
		//	{
		//		string tsPath;
		//		format tsFmt( "lightmap_%d.ts"); tsFmt % i;
		//		FileUtils::ReplaceNameExt( tsPath, locationPath, tsFmt.str());

		//		m_texSets[i]->Save( tsPath.c_str());
		//	}

		//	fflush( fileHandle);
		//	fclose( fileHandle);

		//	return true;
		//}

		return false;
	}

	ui32 LightmapMgr::getTextureSetNum() const
	{
		return (ui32)m_texSets.size();
	}

	TextureSet* LightmapMgr::getTextureSet( ui32 id ) const
	{
		return m_texSets[id];
	}

	//	// 加载xml格式
//	bool LightmapMgr::LoadXml( const char* locationPath)
//	{
//        using namespace rapidxml;
//		using namespace boost::property_tree::detail::rapidxml;
//
//		try
//		{
//			// 读文件
//			format txmlFmt( "%slightmap.xml"); txmlFmt % locationPath;
//			file<> fdoc( txmlFmt.str().c_str());
//			xml_document<>  doc;
//			doc.parse<0>( fdoc.data());
//
//			xml_node<>* rootNode =  doc.first_node("lightmaps");
//			if( !rootNode)
//				return false;
//
//			// 加载TileInfo
//			m_tileInfos.clear();
//			xml_node<>* tileInfosNode = rootNode->first_node( "tile_infos");
//			xml_node<>* tileInfoNode  = tileInfosNode->first_node( "tile");
//			for ( ; tileInfoNode; tileInfoNode=tileInfoNode->next_sibling( "tile"))
//			{
//				string attName   = tileInfoNode->first_attribute( "name")->value();
//				int    attSetIdx = lexical_cast<int>(tileInfoNode->first_attribute( "set_idx")->value());
//				int    attTileId = lexical_cast<int>(tileInfoNode->first_attribute( "tile_id")->value());
//
//				TileInfo tInfo;
//				tInfo.m_uniqueName = attName;
//				tInfo.m_setIdx     = attSetIdx;
//				tInfo.m_tileId	   = attTileId;
//
//				m_tileInfos[attName] = tInfo;
//			}
//
//			// 加载纹理顺序
//			m_texSets.clear();
//			xml_node<>* textureSetsNode = rootNode->first_node( "texture_sets");
//			xml_node<>* textureSetNode  = textureSetsNode->first_node("texture_set");
//			for ( ; textureSetNode; textureSetNode=textureSetNode->next_sibling("texture_set"))
//			{
//				string attPath     = textureSetNode->first_attribute("path")->value();
//				TextureSet* texSet = new_ TextureSet;
//				texSet->Load( (locationPath+attPath).c_str());
//				m_texSets.push_back( texSet);
//			}
//		}
//		catch (...)
//		{
//			return false;
//		}
//
//		return true;
//	}
//
//	// 保存xml格式
//	bool LightmapMgr::SaveXml( const char* locationPath)
//	{
//        using namespace rapidxml;
//		using namespace boost::property_tree::detail::rapidxml;
//
//		std::vector<string> saveHelper; saveHelper.reserve(256);
//
//		xml_document<>  doc;
//		xml_node<>* xmlNode  = doc.allocate_node(node_pi,doc.allocate_string("xml version='1.0' encoding='utf-8'"));
//		xml_node<>* rootNode =  doc.allocate_node(node_element,"lightmaps");  
//
//		doc.append_node( xmlNode);
//		doc.append_node( rootNode);
//
//		// 保存TileInfo
//		if( rootNode)
//		{
//			xml_node<>* tileInfosNode = doc.allocate_node( node_element, "tile_infos");
//			rootNode->append_node( tileInfosNode);
//
//			for ( TileInfoMap::iterator it=m_tileInfos.begin(); it!=m_tileInfos.end(); it++)
//			{
//				xml_node<>*		 tNode    = doc.allocate_node( node_element, "tile");
//				saveHelper.push_back(it->second.m_uniqueName.c_str());
//				xml_attribute<>* attName  = doc.allocate_attribute( "name", saveHelper.back().c_str());
//				saveHelper.push_back(lexical_cast<string>(it->second.m_setIdx));
//				xml_attribute<>* attTexId = doc.allocate_attribute( "set_idx",saveHelper.back().c_str());
//				saveHelper.push_back(lexical_cast<string>(it->second.m_tileId));
//				xml_attribute<>* attTileId= doc.allocate_attribute( "tile_id",saveHelper.back().c_str());
//				
//				tNode->append_attribute(attName);
//				tNode->append_attribute(attTexId);
//				tNode->append_attribute(attTileId);
//				tileInfosNode->append_node( tNode);
//			}
//		}
//
//		// 保存纹理顺序
//		if( rootNode)
//		{
//			xml_node<>* textureSetsNode = doc.allocate_node( node_element, "texture_sets");
//			rootNode->append_node( textureSetsNode);
//
//			for ( size_t i=0; i<m_texSets.size(); i++)
//			{
//				format tFmt( "lightmap_%d.ts"); tFmt % i;
//
//				xml_node<>*		 tNode    = doc.allocate_node( node_element, "texture_set");
//				saveHelper.push_back(tFmt.str());
//				xml_attribute<>* attName  = doc.allocate_attribute( "path", saveHelper.back().c_str());
//
//				tNode->append_attribute(attName);
//				textureSetsNode->append_node( tNode);
//			}
//		}
//
//		// 写文件
//		format txmlFmt( "%slightmap.xml"); txmlFmt % locationPath ;
//		std::ofstream out( txmlFmt.str().c_str());
//#ifdef ARES_PLATFORM_WINDOWS
//		//A_ASSERT(FALSE);
//        //out << doc;
//#else
//        A_ASSERT(FALSE);
//#endif
//
//		// 保存textureset
//		for ( size_t i=0; i<m_texSets.size(); i++)
//		{
//			string tsPath;
//			format tsFmt( "lightmap_%d.ts"); tsFmt % i;
//			FileUtils::ReplaceNameExt( tsPath, locationPath, tsFmt.str());
//
//			m_texSets[i]->Save( tsPath.c_str());
//		}
//
//		return true;
//	}
}