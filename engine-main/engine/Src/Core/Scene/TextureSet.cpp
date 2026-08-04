#include "TextureSet.h"
#include "Render/TextureManager.h"

#define INVALID -1

namespace LORD
{
	const String TextureSet::s_TextureName = "TextureSet_";
	LORD::uint TextureSet::s_TextureSetID = 0;
	
	// 构造函数
	TextureSet::Node::Node()
		: m_id( INVALID)
	{
		++s_TextureSetID;
		m_child[0] = INVALID;
		m_child[1] = INVALID;
	}

	// 是否为叶结点
	bool TextureSet::Node::IsLeaf() const
	{ 
		return m_child[0]==INVALID && m_child[1]==INVALID; 
	}

	TextureSet::TextureSet()
		: m_width(512)
		, m_height(512)
		, m_texture(NULL)
		, m_TextureData(NULL)
	{

	}

	// 构造函数
	TextureSet::TextureSet(int width, int height)
		: m_width(width)
		, m_height(height)
		, m_texture(NULL)
		, m_TextureData(NULL)
	{
		// 插入默认主节点
		Node rootNode;
		rootNode.m_rc = IRect( 0, 0, m_width, m_height);
		m_nodes.reserve(256);
		m_nodes.push_back( rootNode);
	}

	TextureSet::~TextureSet()
	{
		if (m_texture)
		{
			TextureManager::Instance()->releaseResource(m_texture);
			m_texture = NULL;
		}
		LordSafeFree(m_TextureData);
	}

	// 获取块纹理Viewport
	const Vector4 TextureSet::GetViewport( int nodeIdx) const
	{
		LordAssert( nodeIdx>=0 && nodeIdx<static_cast<int>(m_nodes.size()));

		Vector4	result;
		const IRect& tRc   = m_nodes[nodeIdx].m_rc;
		result.x = static_cast<float>(tRc.left)  / static_cast<float>(m_width);
		result.y = static_cast<float>(tRc.top)   / static_cast<float>(m_height);
		result.z = static_cast<float>(tRc.width) / static_cast<float>(m_width);
		result.w = static_cast<float>(tRc.height)/ static_cast<float>(m_height);

		return result;
	}

	// 创建贴图
	void TextureSet::CreateTexture()
	{
		Buffer buffer;
		m_texture = TextureManager::Instance()->createManual(s_TextureName + StringUtil::ToString(s_TextureSetID), Texture::TT_2D, PF_RGBA8_UNORM, Texture::TU_STATIC, m_width, m_height, 1, 0, buffer);
	}

	// 插入函数
	int TextureSet::Insert(Byte* data, PixelFormat fm, int width, int height)
	{
		if( !data)
			return INVALID;

		int nodeIdx = Insert( 0, data, fm, width, height);

		return OverWrite( nodeIdx, data, fm, width, height);
	}

	int TextureSet::Insert_MultiThread(Byte* data, PixelFormat fm, int width, int height)
	{
		if (!data)
			return INVALID;

		int nodeIdx = Insert(0, data, fm, width, height);

		return OverWrite_MultiThread(nodeIdx, data, fm, width, height);
	}

	int TextureSet::OverWrite_MultiThread(int nodeIdx, Byte* data, PixelFormat fm, int width, int height)
	{
		if (nodeIdx != INVALID)
		{
			m_nodes[nodeIdx].m_id = nodeIdx;
			uint pixelsize = PixelUtil::GetPixelSize(PF_RGBA8_UNORM);
			bool hasAlpha = PixelUtil::HasAlpha(fm);

			// copy pixels over from texture to pNode->m_rc part of texture
			if (!m_TextureData)
			{
				m_TextureData = LordMalloc(m_width*m_height*pixelsize);
				memset(m_TextureData, 0, m_width*m_height*pixelsize);
			}
			const IRect& rc = m_nodes[nodeIdx].m_rc;

			Dword* pDestData = (Dword*)m_TextureData;
			for (int h = 0; h<height; h++)
			{
				for (int w = 0; w<width; w++)
				{
					int destIdx = (rc.top + h) * m_width + w + rc.left;
					int srcIdx = h * width + w;
					if (hasAlpha)
					{
						PixelUtil::BulkPixelConversion(data + srcIdx * 4, fm, (int*)m_TextureData + destIdx, PF_RGBA8_UNORM, 1);
					}
					else
					{
						PixelUtil::BulkPixelConversion(data + srcIdx * 3, fm, (int*)m_TextureData + destIdx, PF_RGBA8_UNORM, 1);
					}
				}
			}

			return nodeIdx;
		}

		return INVALID;
	}

	// 覆写纹理
	int TextureSet::OverWrite( int nodeIdx, Byte* data, PixelFormat fm, int width, int height )
	{
		if( nodeIdx!=INVALID)
		{
			m_nodes[nodeIdx].m_id = nodeIdx;
			uint pixelsize = PixelUtil::GetPixelSize(PF_RGBA8_UNORM);

			// copy pixels over from texture to pNode->m_rc part of texture
			if (!m_TextureData)
			{
				m_TextureData = LordMalloc(m_width*m_height*pixelsize);
				memset(m_TextureData, 0, m_width*m_height*pixelsize);
			}
			const IRect& rc = m_nodes[nodeIdx].m_rc;

			Dword* pDestData = (Dword*)m_TextureData;	
			for ( int h=0; h<height; h++)
			{
				for( int w=0; w<width; w++)
				{
					int destIdx = ( rc.top + h) * m_width + w + rc.left;
					int srcIdx  = h * width + w;
					Color color = data[srcIdx];
					// Determine the largest color component
					float maxCeil = std::max<float>(std::max<float>( std::max<float>( color.r, color.g), color.b), 0.1f);
					color.a  = maxCeil / 8.0f;
					color.r /= maxCeil;
					color.g /= maxCeil;
					color.b /= maxCeil;

					pDestData[destIdx] = color.getABGR();
				}
			}
			
			if (m_texture)
			{
				ERect rt(0, 0, float(m_width), float(m_height));
				m_texture->updateSubTex2D(0, rt, m_TextureData);
			}
			else
			{
				Buffer buffer(m_width*m_height*pixelsize, m_TextureData, false);
				m_texture = TextureManager::Instance()->createManual(s_TextureName + StringUtil::ToString(s_TextureSetID), Texture::TT_2D, PF_RGBA8_UNORM, Texture::TU_STATIC, m_width, m_height, 1, 0, buffer);
			}
			
			return nodeIdx;
		}

		return INVALID;
	}

	void TextureSet::uploadBuffer()
	{
		uint pixelsize = PixelUtil::GetPixelSize(PF_RGBA8_UNORM);
		if (m_texture)
		{
			ERect rt(0, 0, float(m_width), float(m_height));
			m_texture->updateSubTex2D(0, rt, m_TextureData);
		}
		else
		{
			Buffer buffer(m_width*m_height*pixelsize, m_TextureData, false);
			m_texture = TextureManager::Instance()->createManual(s_TextureName + StringUtil::ToString(s_TextureSetID), Texture::TT_2D, PF_RGBA8_UNORM, Texture::TU_STATIC, m_width, m_height, 1, 0, buffer);
		}
	}

	// 插入纹理
	int TextureSet::Insert( int nodeIdx, Byte* data, PixelFormat fm, int width, int height)
	{
		if( nodeIdx==INVALID)
			return INVALID;

		// if we're not a leaf then
		if( !m_nodes[nodeIdx].IsLeaf())
		{
			// 优先插入面积较小的块
			int child0Idx = m_nodes[nodeIdx].m_child[0];
			int child1Idx = m_nodes[nodeIdx].m_child[1];
			if( child0Idx!=INVALID && child1Idx!=INVALID)
			{
				if( m_nodes[child0Idx].m_rc.getArea()>m_nodes[child1Idx].m_rc.getArea())
					std::swap( child0Idx, child1Idx);
			}

			// try inserting into first child
			int newIdx = Insert( child0Idx, data, fm, width, height);
			if( newIdx != INVALID)
				return newIdx;

			// no room, insert into second
			return Insert( child1Idx, data, fm, width, height);
		}
		else
		{
			// if there is already a lightmap here, return
			if( m_nodes[nodeIdx].m_id != INVALID)
				return INVALID;

			int texWidth  = static_cast<int>(width);
			int texHeight = static_cast<int>(height);

			// if we're too small, return
			if( m_nodes[nodeIdx].m_rc.width<texWidth || m_nodes[nodeIdx].m_rc.height<texHeight)
				return INVALID;

			// if we'rs just right accept, "no surplus space"
			if( m_nodes[nodeIdx].m_rc.width==texWidth && m_nodes[nodeIdx].m_rc.height==texHeight)
				return nodeIdx;

			// otherwise, gotta split this node and create some kids
			Node node0, node1;

			// deside split pattern
			int dw = m_nodes[nodeIdx].m_rc.width  - texWidth;
			int dh = m_nodes[nodeIdx].m_rc.height - texHeight;

			const IRect& nRect = m_nodes[nodeIdx].m_rc;
			if( dw>dh)
			{
				node0.m_rc = IRect( nRect.left+texWidth, nRect.top, nRect.width-texWidth, nRect.height);
				node1.m_rc = IRect( nRect.left, nRect.top+texHeight, texWidth, nRect.height-texHeight);
			}
			else
			{
				node0.m_rc = IRect( nRect.left+texWidth, nRect.top, nRect.width-texWidth, texHeight);
				node1.m_rc = IRect( nRect.left, nRect.top+texHeight, nRect.width, nRect.height-texHeight);
			}

			// 添加有省余空间的子结点
			if( node0.m_rc.getArea() > 0)
			{
				m_nodes[nodeIdx].m_child[0] = (int)m_nodes.size();
				m_nodes.push_back( node0);
			}
			if( node1.m_rc.getArea() > 0)
			{
				m_nodes[nodeIdx].m_child[1] = (int)m_nodes.size();
				m_nodes.push_back( node1);
			}

			m_nodes[nodeIdx].m_rc.width = texWidth;
			m_nodes[nodeIdx].m_rc.height= texHeight;

			return nodeIdx;
		}
	}
	// 加载
	void TextureSet::Load( const char* filePath)
	{
		String		  tePath(filePath);
		size_t index = tePath.find_last_of('.');
		String ddsName = tePath.substr(0, index) + ".bmp";

		m_texture = TextureManager::Instance()->createTexture(ddsName);
		m_texture->load();

		DataStream* pStream = ResourceGroupManager::Instance()->openResource(tePath);
		if( pStream)
		{
			// 保存上方向索引
			ui32 numNode = 0;
			pStream->read(&numNode, sizeof(numNode));

			// 保存数据大小
			m_nodes.resize( numNode);
			pStream->read( m_nodes.data(), sizeof( Node)*numNode);
			LordDelete pStream;
		}
	//	// 加载纹理
	//	string tePath;
	//	FileUtils::ReplaceExt( tePath, filePath, ".dds");

	//#ifdef ARES_EDITOR_MODE
 //       TexturePtr tePtr = AResSystem.SyncLoadDDS( tePath.c_str(), EAH_GPURead | EAH_CPUWrite);
	//	Init( tePtr);
	//#else
 //       TexturePtr tePtr = AResSystem.SyncLoadDDS( tePath.c_str(), EAH_GPURead | EAH_Immutable);
	//	Init( tePtr);
	//#endif

	//	// 加载结点信息
	//	FILE* fileHandle = fopen( filePath, "rb");
	//	if( fileHandle)
	//	{
	//		// 保存上方向索引
	//		size_t numNode = 0;
	//		fread( &numNode, sizeof(numNode), 1, fileHandle);

	//		// 保存数据大小
	//		m_nodes.resize( numNode);
	//		fread( m_nodes.data(), sizeof( Node)*numNode, 1, fileHandle);

	//		fflush( fileHandle);
	//		fclose( fileHandle);
	//	}
	}

	// 保存
	void TextureSet::Save( const char* filePath)
	{
		LordAssert(m_TextureData);
		//// 保存纹理
		String		  tePath(filePath);
		size_t index = tePath.find_last_of('.');
		String ddsName = tePath.substr(0, index) + ".bmp";
		Image img((Byte*)m_TextureData, m_width, m_height, 1, PF_RGBA8_UNORM);
		img.saveToFile(ddsName, IF_BMP);

		/*std::vector<wchar_t> s_wchar_buff;
		size_t lengthUnicode = MultiByteToWideChar(CP_ACP, 0, tePath.c_str(), 
			tePath.length(), NULL, 0);
		if(s_wchar_buff.size() < lengthUnicode + 1)
			s_wchar_buff.resize(lengthUnicode * 2);

		wchar_t* szUnicode = &s_wchar_buff[0];
		MultiByteToWideChar(CP_ACP, 0, tePath.c_str(), tePath.length(), szUnicode, lengthUnicode);
		szUnicode[lengthUnicode] = 0;*/

		std::fstream f;
		f.open(filePath, std::ios::out|std::ios::binary|std::ios::trunc);
		LORD::DataStream* filePtr(LordNew LORD::FileStreamDataStream(&f, false));
		if (filePtr)
		{
			ui32 numNode = (ui32)m_nodes.size();
			filePtr->write(&numNode, sizeof(numNode));

			//	// 保存数据大小
			filePtr->write( m_nodes.data(), sizeof(Node)*numNode);
		}
		//FileUtils::ReplaceExt( tePath, filePath, ".dds");
		//AResSystem.SyncSaveTexture( m_texture, tePath.c_str());

		//// 保存结点信息
		//FILE* fileHandle = fopen( filePath, "wb");
		//if( fileHandle)
		//{
		//	// 保存上方向索引
		//	size_t numNode = m_nodes.size();
		//	fwrite( &numNode, sizeof(numNode), 1, fileHandle);

		//	// 保存数据大小
		//	fwrite( m_nodes.data(), sizeof( Node)*numNode, 1, fileHandle);

		//	fflush( fileHandle);
		//	fclose( fileHandle);
		//}


	}
	void TextureSet::SaveTga(const char * filePath)
	{
		LordAssert(m_TextureData);
		//// 保存纹理
		String		  tePath(filePath);
		size_t index = tePath.find_last_of('.');
		String ddsName = tePath.substr(0, index) + ".tga";
		Image img((Byte*)m_TextureData, m_width, m_height, 1, PF_RGBA8_UNORM);
		img.saveToFile(ddsName, IF_TGA);

		/*std::vector<wchar_t> s_wchar_buff;
		size_t lengthUnicode = MultiByteToWideChar(CP_ACP, 0, tePath.c_str(),
		tePath.length(), NULL, 0);
		if(s_wchar_buff.size() < lengthUnicode + 1)
		s_wchar_buff.resize(lengthUnicode * 2);

		wchar_t* szUnicode = &s_wchar_buff[0];
		MultiByteToWideChar(CP_ACP, 0, tePath.c_str(), tePath.length(), szUnicode, lengthUnicode);
		szUnicode[lengthUnicode] = 0;*/

		std::fstream f;
		f.open(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
		LORD::DataStream* filePtr(LordNew LORD::FileStreamDataStream(&f, false));
		if (filePtr)
		{
			ui32 numNode = (ui32)m_nodes.size();
			filePtr->write(&numNode, sizeof(numNode));

			//	// 保存数据大小
			filePtr->write(m_nodes.data(), sizeof(Node)*numNode);
		}
	}
}