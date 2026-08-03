#ifndef _TEXTURESET_H_
#define _TEXTURESET_H_

#include "Core.h"

namespace LORD
{
	//------------------------------------------------------
	// 纹理包-由小纹理组成的大纹理 2012-7-20 刘亮
	// 装箱算法:
	// http://www.blackpawn.com/texts/lightmaps/default.html
	//-------------------------------------------------------
	class LORD_CORE_API TextureSet : public ObjectAlloc
	{
		typedef TRect<int> IRect;
	public:
		// 接点
		struct Node
		{
			int			m_id;				// ID>=0标识该结点已使用
			int			m_child[2];			// 记录子结点ID
			IRect		m_rc;

			// 构造函数
			Node();

			// 是否为叶结点
			bool IsLeaf() const;
		};

	public:
		TextureSet();
		TextureSet(int width, int height);
		~TextureSet();

		// 创建纹理
		void CreateTexture();

		// 插入纹理 INVALID 无效索引
		int Insert(Byte* data, PixelFormat fm, int width, int height);

		int Insert_MultiThread(Byte* data, PixelFormat fm, int width, int height);
		// 覆写纹理
		int OverWrite_MultiThread(int nodeIdx, Byte* data, PixelFormat fm, int width, int height);
		// 覆写纹理
		int OverWrite( int nodeIdx, Byte* data, PixelFormat fm, int width, int height);

		void uploadBuffer();
		// 获取当前管理纹理
		const Texture* GetTexture() const { return m_texture; }
		void addTextureRefrence() { m_texture->addRefrenceCount(); }

		// 获取块纹理Viewport
		const Vector4 GetViewport( int nodeIdx) const;

		// 获取结点信息
		const Node& GetNode( int nodeIdx) const { return m_nodes[nodeIdx]; }

		// 获取宽
		int GetWidth() const { return m_width; }

		// 获取高
		int GetHeight() const { return m_height; }
		
		// 加载
		void Load( const char* filePath);

		// 保存
		void Save( const char* filePath);

		void SaveTga(const char* filePath);
	private:
		// 插入纹理
		int Insert(  int nodeIdx, Byte* data, PixelFormat fm, int width, int height);

	private:
		static const String	s_TextureName;
		static uint			s_TextureSetID;
		int					m_width;			// 纹理宽
		int					m_height;			// 纹理高
		Texture*			m_texture;			// 所管理的纹理
		void*				m_TextureData;
		vector<Node>::type	m_nodes;			// 主节点(索引0为主节点)			
	};
}

#endif