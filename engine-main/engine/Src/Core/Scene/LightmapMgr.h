#ifndef _LIGHTMAPMGR_H_
#define _LIGHTMAPMGR_H_

#include <map>
#include <string>
#include "TextureSet.h"

namespace LORD
{
	//---------------------------------
	// 场景光照图管理器 2012-7-7
	//     添加,删除,合并,保存
	//---------------------------------
	class LORD_CORE_API LightmapMgr : public Singleton<LightmapMgr>, public ObjectAlloc
	{
	public:
		// TextureTile
		struct TileInfo
		{
			String  m_uniqueName;	// 唯一标识
			int		m_setIdx;		// 纹理索引
			int		m_tileId;		// 纹理块ID
			Vector3 m_scale;
		};
		typedef map<String, TileInfo>::type TileInfoMap;

		inline static LightmapMgr* Instance()
		{
			return ms_pSingleton;
		}
	public:
		LightmapMgr();
		LightmapMgr( int width, int height);
		~LightmapMgr();

		// 添加光照图(以实体名与mesh索引命名)
		bool addLightmap( const char* uniqueName, Color* texture, int width, int height, const Vector3& scale);

		// 查询
		bool query( const char* uniqueName, const Texture*& lightmap, Vector4& lightmapViewPort, Vector3& scale);

		// 获取打包纹理数量
		ui32 getTextureSetNum() const;

		// 根据索引获取打包纹理
		TextureSet* getTextureSet(ui32 id) const;

		// 清除所有
		void clear();

		// 加载
		bool load();

		// 保存
		bool save(const String& locationPath);

	private:
		// 新建纹理集
		TextureSet* allocateTextureSet( int width, int height);

		// 添加到指定纹理集
		bool addLightmapToTextureSet( TextureSet* texSet, int setIdx, const char* uniqueName, Color* texture, int width, int height, const Vector3& scale);

		// 覆写纹理集
		bool overWriteLightmapToTextureSet( TextureSet* texSet, int tileId, Color* texture, int width, int height, const Vector3& scale);

	private:
		int							m_tsWidth;		// TextureSet宽
		int							m_tsHeight;		// TextureSet高
		TileInfoMap					m_tileInfos;	// 块信息
		vector<TextureSet*>::type	m_texSets;		// 纹理集
	};
}

#endif