#include "Core.h"
#include "Texture.h"
#include "Renderer.h"
#include "Object/Exception.h"
#include "TextureManager.h"
#include "Image/PixelFormat.h"
#if (LORD_PLATFORM == LORD_PLATFORM_ANDROID || LORD_PLATFORM == LORD_PLATFORM_LINUX)
#include <sys/stat.h>
#include <sys/errno.h>
#endif

namespace LORD
{

	Texture::Texture(const String& name, ResourceType rt)
		: Resource(name, rt)
		, m_texType(TT_2D)
		, m_pixFmt(PF_UNKNOWN)
		, m_usage(TU_GPU_READ | TU_CPU_WRITE)
		, m_width(0)
		, m_height(0)
		, m_depth(1)
		, m_numMipmaps(1)
		, m_size(0)
		, m_pPreparedData(NULL)
		, m_compressType(CompressType_Unknown)
		, m_bCompressed( false )
		, m_faceNum( 1 )
		, m_endian(0)
		, m_xDimension( 0 )
		, m_yDimension( 0 )
		, m_zDimension( 0 )
		, m_bitsPerPixel( 0 )
		, m_blockSize( 0 )
		, m_headerSize( 0 )
		, m_uploadedSize( 0 )
		, m_pSamplerState( 0 )
	{
	}

	Texture::Texture(TexType texType, PixelFormat pixFmt, Dword usage, uint width, uint height, uint depth, 
		uint numMipmaps, const Buffer& buff, bool bBak)
		: m_texType(texType)
		, m_pixFmt(pixFmt)
		, m_usage(usage)
		, m_width(width)
		, m_height(height)
		, m_depth(depth)
		, m_numMipmaps(numMipmaps)
		, m_size(0)
		, m_pPreparedData(NULL)
		, m_compressType(CompressType_Unknown)
		, m_bCompressed( false )
		, m_faceNum(1)
		, m_endian(0)
		, m_xDimension( 0 )
		, m_yDimension( 0 )
		, m_zDimension( 0 )
		, m_bitsPerPixel( 0 )
		, m_blockSize( 0 )
		, m_headerSize( 0 )
		, m_uploadedSize( 0 )
        , m_pSamplerState( 0 )
		, m_userData(0)
	{
		if(numMipmaps > MAX_MINMAPS)
		{
			m_numMipmaps = MAX_MINMAPS;
			LordLogWarning("Over the max support mipmaps, using the max mipmaps num.");
		}
		else
		{
			m_numMipmaps = (numMipmaps > 0? numMipmaps : 1);
		}
	}

	Texture::~Texture()
	{
		LordSafeFree(m_pPreparedData);
	}

	bool Texture::create2D(PixelFormat pixFmt, Dword usage, uint width, uint height, uint numMipmaps, const Buffer& buff)
	{
		return false;
	}

	bool Texture::reCreate2D( PixelFormat pixFmt, Dword usage, uint width, uint height, uint numMipmaps, const Buffer& buff )
	{
		unload();
		create2D(pixFmt, usage, width, height, numMipmaps, buff);
		m_size = PixelUtil::CalcSurfaceSize(width, height, m_depth, numMipmaps, pixFmt);
		return true;
	}

	size_t Texture::calculateSize() const
	{
		// need repaird
		return (size_t)PixelUtil::CalcSurfaceSize(m_width, m_height, m_depth, m_numMipmaps, m_pixFmt);
	}

	bool Texture::getData(Byte* data)
	{
		return false;
	}

	bool Texture::prepareImpl()
	{
		if (mResourceType == RESOURCE_TYPE_NORMAL)
		{
			if (!_data_io())
			{
				LordLogError("Texture[%s] load Into System Memory Failed !", getName().c_str());

				return false;
			}
		}
		else if (mResourceType == RESOURCE_TYPE_OUTSIDE)
		{
			if (!_data_io_outside())
			{
				LordLogError("Texture[%s] load Into System Memory Failed !", getName().c_str());

				return false;
			}
		}

		return true;
	}

	void Texture::unprepareImpl()
	{
		LordSafeFree(m_pPreparedData);
	}

	bool Texture::loadImpl()
	{
		return false;// implementation it in GLES2Texture::loadImpl
	}

	void Texture::unloadImpl()
	{	
	}

	bool Texture::_data_io()
	{
		LordAssert(!m_pPreparedData);

		DataStream* pStream = ResourceGroupManager::Instance()->openResource(getName());

		if(!pStream) return false;

		m_pPreparedData = static_cast<Byte*>(LordMalloc(pStream->size()));

		if( !m_pPreparedData )
		{
			return false;
		}

		pStream->read( m_pPreparedData, pStream->size() );

		m_size = (ui32)pStream->size();

		LordDelete pStream;
		return true;
	}

	bool Texture::_data_io_outside()
	{
		LordAssert(!m_pPreparedData);
		LordAssert(mResourceType == RESOURCE_TYPE_OUTSIDE);

		struct stat tagStat;
		int ret = stat(mName.c_str(), &tagStat);
		if (ret == -1)
		{
			LordLogError("FileSystemArchive error  code %d    msg %s  fullPath %s", errno, strerror(errno), mName.c_str());
			return false;
		}
		assert(ret == 0);
		std::ifstream* origStream = LordNewT(std::ifstream);
		origStream->open(mName.c_str(), std::ios::in | std::ios::binary);

		if (origStream->fail())
		{
			//std::cerr << "Error: " << std::strerror(errno);
			LordDeleteT(origStream, basic_ifstream);
			LordException("Error: Cannot open file: %s in FileSystemArchive::open", mName.c_str());
			return false;
		}
		FileStreamDataStream* pStream = LordNew FileStreamDataStream(mName.c_str(),
			origStream, tagStat.st_size, true);

		if (!pStream) 
			return false;

		m_pPreparedData = static_cast<Byte*>(LordMalloc(pStream->size()));

		if (!m_pPreparedData)
		{
			return false;
		}

		pStream->read(m_pPreparedData, pStream->size());

		m_size = (ui32)pStream->size();

		LordDelete pStream;
		return true;
	}

}

