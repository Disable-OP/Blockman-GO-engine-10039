/********************************************************************
filename: 	GLES2Mapping.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2MAPPING_H__
#define __LORD_GLES2MAPPING_H__

#include "GLES2RenderState.h"
#include "Render/RenderInput.h"
#include "Render/Texture.h"

namespace LORD
{
	class LORD_GLES2RENDER_API GLES2Mapping
	{
	public:
		/*static GLES2Error		MapErrorCode(GLenum err);
		static GLenum			MapFrontFace(bool bFrontFaceCCW);
		static GLenum			MapGPUBufferUsage(Dword usage);
		static GLenum			MapDataType(PixelFormat pixFmt);
		static GLenum			MapInternalFormat(PixelFormat pixFmt);
		static GLenum			MapFormat(PixelFormat pixFmt);
		static GLenum			MapPrimitiveTopology(RenderInput::TopologyType type);
		static String			MapVertexSemanticString(RenderInput::VertexSemantic semantic);
		static GLenum			MapComparisonFunc(RenderState::ComparisonFunc func);
		static GLenum			MapTextureType(Texture::TexType type);*/
		static GLenum			MapStencilOperation(DepthStencilState::StencilOperation op);
		static GLenum			MapBlendOperation(BlendState::BlendOperation op);
		static GLenum			MapBlendFactor(BlendState::BlendFactor factor);
		static GLint			MapAddressMode(SamplerState::AddressMode mode);

		static inline GLES2Error MapErrorCode(GLenum err)
		{
			switch (err)
			{
			case GL_NO_ERROR:			return GLES2ERR_NO_ERROR;
			case GL_INVALID_ENUM:		return GLES2ERR_INVALID_ENUM;
			case GL_INVALID_VALUE:		return GLES2ERR_INVALID_VALUE;
			case GL_INVALID_OPERATION:	return GLES2ERR_INVALID_OPERATION;
			case GL_OUT_OF_MEMORY:		return GLES2ERR_OUT_OF_MEMORY;
			default:
				{
					LordAssert(false);
					return GLES2ERR_NO_ERROR;
				}
			}
		}

		static inline GLenum MapGPUBufferUsage(Dword usage)
		{
			if(	(usage & GPUBuffer::GBU_GPU_READ) &&
				!(usage & GPUBuffer::GBU_CPU_READ) && !(usage & GPUBuffer::GBU_CPU_WRITE))
				return GL_STATIC_DRAW;
			else
				return GL_DYNAMIC_DRAW;
		}

		static inline GLenum MapDataType(PixelFormat pixFmt)
		{
			switch(pixFmt)
			{
			case PF_R8_UNORM:			return GL_UNSIGNED_BYTE;
			case PF_R8_SNORM:			return GL_BYTE;
			case PF_R8_UINT:			return GL_UNSIGNED_BYTE;
			case PF_R8_SINT:			return GL_BYTE;

			case PF_A8_UNORM:			return GL_UNSIGNED_BYTE;

			case PF_RG8_UNORM:			return GL_UNSIGNED_BYTE;
			case PF_RG8_SNORM:			return GL_BYTE;
			case PF_RG8_UINT:			return GL_UNSIGNED_BYTE;
			case PF_RG8_SINT:			return GL_BYTE;

			case PF_RGB8_UNORM:			return GL_UNSIGNED_BYTE;
			case PF_RGB8_SNORM:			return GL_BYTE;
			case PF_RGB8_UINT:			return GL_UNSIGNED_BYTE;
			case PF_RGB8_SINT:			return GL_BYTE;

			case PF_BGR8_UNORM:			return GL_UNSIGNED_BYTE;

			case PF_RGBA8_UNORM:		return GL_UNSIGNED_BYTE;
			case PF_RGBA8_SNORM:		return GL_BYTE;
			case PF_RGBA8_UINT:			return GL_UNSIGNED_BYTE;
			case PF_RGBA8_SINT:			return GL_BYTE;

			case PF_BGRA8_UNORM:		return GL_UNSIGNED_BYTE;

			case PF_R16_UNORM:			return GL_UNSIGNED_SHORT;
			case PF_R16_SNORM:			return GL_SHORT;
			case PF_R16_UINT:			return GL_UNSIGNED_SHORT;
			case PF_R16_SINT:			return GL_SHORT;
			case PF_R16_FLOAT:			return GL_HALF_FLOAT_OES;

			case PF_RG16_UNORM:			return GL_UNSIGNED_SHORT;
			case PF_RG16_SNORM:			return GL_SHORT;
			case PF_RG16_UINT:			return GL_UNSIGNED_SHORT;
			case PF_RG16_SINT:			return GL_SHORT;
			case PF_RG16_FLOAT:			return GL_HALF_FLOAT_OES;

			case PF_RGB16_UNORM:		return GL_UNSIGNED_SHORT;
			case PF_RGB16_SNORM:		return GL_SHORT;
			case PF_RGB16_UINT:			return GL_UNSIGNED_SHORT;
			case PF_RGB16_SINT:			return GL_SHORT;
			case PF_RGB16_FLOAT:		return GL_HALF_FLOAT_OES;

			case PF_RGBA16_UNORM:		return GL_UNSIGNED_SHORT;
			case PF_RGBA16_SNORM:		return GL_SHORT;
			case PF_RGBA16_UINT:		return GL_UNSIGNED_SHORT;
			case PF_RGBA16_SINT:		return GL_SHORT;
			case PF_RGBA16_FLOAT:		return GL_HALF_FLOAT_OES;

			case PF_R32_UNORM:			return GL_UNSIGNED_INT;
			case PF_R32_SNORM:			return GL_INT;
			case PF_R32_UINT:			return GL_UNSIGNED_INT;
			case PF_R32_SINT:			return GL_INT;
			case PF_R32_FLOAT:			return GL_FLOAT;

			case PF_RG32_UNORM:			return GL_UNSIGNED_INT;
			case PF_RG32_SNORM:			return GL_INT;
			case PF_RG32_UINT:			return GL_UNSIGNED_INT;
			case PF_RG32_SINT:			return GL_INT;
			case PF_RG32_FLOAT:			return GL_FLOAT;

			case PF_RGB32_UNORM:		return GL_UNSIGNED_INT;
			case PF_RGB32_SNORM:		return GL_INT;
			case PF_RGB32_UINT:			return GL_UNSIGNED_INT;
			case PF_RGB32_SINT:			return GL_INT;
			case PF_RGB32_FLOAT:		return GL_FLOAT;

			case PF_RGBA32_UNORM:		return GL_UNSIGNED_INT;
			case PF_RGBA32_SNORM:		return GL_INT;
			case PF_RGBA32_UINT:		return GL_UNSIGNED_INT;
			case PF_RGBA32_SINT:		return GL_INT;
			case PF_RGBA32_FLOAT:		return GL_FLOAT;

			case PF_D16_UNORM:			return GL_UNSIGNED_SHORT;
			case PF_D24_UNORM_S8_UINT:	return GL_UNSIGNED_INT_24_8_OES;
			case PF_D32_FLOAT:			return GL_FLOAT;
			default:
				{
					LordAssertX("Unsupported pixel format [%s].", PixelUtil::GetPixelFormatName(pixFmt).c_str());
					return GL_BYTE;
				}
			}
		}

		static inline GLenum MapInternalFormat(PixelFormat pixFmt)
		{
			switch(pixFmt)
			{
			case PF_R8_UNORM:				return GL_LUMINANCE;
			case PF_R8_SNORM:				return GL_LUMINANCE;
			case PF_R8_UINT:				return GL_LUMINANCE;
			case PF_R8_SINT:				return GL_LUMINANCE;

			case PF_A8_UNORM:				return GL_ALPHA;

			case PF_RG8_UNORM:				return GL_LUMINANCE_ALPHA;
			case PF_RG8_SNORM:				return GL_LUMINANCE_ALPHA;
			case PF_RG8_UINT:				return GL_LUMINANCE_ALPHA;
			case PF_RG8_SINT:				return GL_LUMINANCE_ALPHA;

			case PF_RGB8_UNORM:				return GL_RGB;
			case PF_RGB8_SNORM:				return GL_RGB;
			case PF_RGB8_UINT:				return GL_RGB;
			case PF_RGB8_SINT:				return GL_RGB;

			case PF_BGR8_UNORM:				return GL_RGB;

			case PF_RGBA8_UNORM:			return GL_RGBA;
			case PF_RGBA8_SNORM:			return GL_RGBA;
			case PF_RGBA8_UINT:				return GL_RGBA;
			case PF_RGBA8_SINT:				return GL_RGBA;

			case PF_BGRA8_UNORM:			return GL_BGRA_EXT;

				//case PF_R16_UNORM:
				//case PF_R16_SNORM:
				//case PF_R16_UINT:
				//case PF_R16_SINT:
			case PF_R16_FLOAT:				return GL_LUMINANCE;

				//case PF_RG16_UNORM:
				//case PF_RG16_SNORM:
				//case PF_RG16_UINT:
				//case PF_RG16_SINT:
			case PF_RG16_FLOAT:				return GL_LUMINANCE_ALPHA;

				//case PF_RGB16_UNORM:
				//case PF_RGB16_SNORM:
				//case PF_RGB16_UINT:
				//case PF_RGB16_SINT:
			case PF_RGB16_FLOAT:			return GL_RGB;

				//case PF_RGBA16_UNORM:
				//case PF_RGBA16_SNORM:
				//case PF_RGBA16_UINT:
				//case PF_RGBA16_SINT:
			case PF_RGBA16_FLOAT:			return GL_RGBA;

				//case PF_R32_UNORM:
				//case PF_R32_SNORM:
				//case PF_R32_UINT:
				//case PF_R32_SINT:
			case PF_R32_FLOAT:				return GL_LUMINANCE;

				//case PF_RG32_UNORM:
				//case PF_RG32_SNORM:
				//case PF_RG32_UINT:
				//case PF_RG32_SINT:
			case PF_RG32_FLOAT:				return GL_LUMINANCE_ALPHA;

				//case PF_RGB32_UNORM:
				//case PF_RGB32_SNORM:
				//case PF_RGB32_UINT:
				//case PF_RGB32_SINT:
			case PF_RGB32_FLOAT:			return GL_RGB;

				//case PF_RGBA32_UNORM:
				//case PF_RGBA32_SNORM:
				//case PF_RGBA32_UINT:
				//case PF_RGBA32_SINT:
			case PF_RGBA32_FLOAT:			return GL_RGBA;

			case PF_D16_UNORM:				return GL_DEPTH_COMPONENT16;
			case PF_D24_UNORM_S8_UINT:		return GL_DEPTH24_STENCIL8_OES;
			case PF_D32_FLOAT:				return GL_DEPTH_COMPONENT32_OES;
			default:
				{
					LordAssertX("Unsupported pixel format [%s].", PixelUtil::GetPixelFormatName(pixFmt).c_str());
					return GL_BYTE;
				}
			}
		}

		static inline GLenum MapFormat(PixelFormat pixFmt)
		{
			switch(pixFmt)
			{
			case PF_R8_UNORM:				return GL_LUMINANCE;
			case PF_R8_SNORM:				return GL_LUMINANCE;
			case PF_R8_UINT:				return GL_LUMINANCE;
			case PF_R8_SINT:				return GL_LUMINANCE;

			case PF_A8_UNORM:				return GL_ALPHA;

			case PF_RG8_UNORM:				return GL_LUMINANCE_ALPHA;
			case PF_RG8_SNORM:				return GL_LUMINANCE_ALPHA;
			case PF_RG8_UINT:				return GL_LUMINANCE_ALPHA;
			case PF_RG8_SINT:				return GL_LUMINANCE_ALPHA;

			case PF_RGB8_UNORM:				return GL_RGB;
			case PF_RGB8_SNORM:				return GL_RGB;
			case PF_RGB8_UINT:				return GL_RGB;
			case PF_RGB8_SINT:				return GL_RGB;

			case PF_BGR8_UNORM:				return GL_RGB;

			case PF_RGBA8_UNORM:			return GL_RGBA;
			case PF_RGBA8_SNORM:			return GL_RGBA;
			case PF_RGBA8_UINT:				return GL_RGBA;
			case PF_RGBA8_SINT:				return GL_RGBA;

			case PF_BGRA8_UNORM:			return GL_BGRA_EXT;

				//case PF_R16_UNORM:
				//case PF_R16_SNORM:
				//case PF_R16_UINT:
				//case PF_R16_SINT:
			case PF_R16_FLOAT:				return GL_LUMINANCE;

				//case PF_RG16_UNORM:
				//case PF_RG16_SNORM:
				//case PF_RG16_UINT:
				//case PF_RG16_SINT:
			case PF_RG16_FLOAT:				return GL_LUMINANCE_ALPHA;

				//case PF_RGB16_UNORM:
				//case PF_RGB16_SNORM:
				//case PF_RGB16_UINT:
				//case PF_RGB16_SINT:
			case PF_RGB16_FLOAT:			return GL_RGB;

				//case PF_RGBA16_UNORM:
				//case PF_RGBA16_SNORM:
				//case PF_RGBA16_UINT:
				//case PF_RGBA16_SINT:
			case PF_RGBA16_FLOAT:			return GL_RGBA;

				//case PF_R32_UNORM:
				//case PF_R32_SNORM:
				//case PF_R32_UINT:
				//case PF_R32_SINT:
			case PF_R32_FLOAT:				return GL_LUMINANCE;

				//case PF_RG32_UNORM:
				//case PF_RG32_SNORM:
				//case PF_RG32_UINT:
				//case PF_RG32_SINT:
			case PF_RG32_FLOAT:				return GL_LUMINANCE_ALPHA;

				//case PF_RGB32_UNORM:
				//case PF_RGB32_SNORM:
				//case PF_RGB32_UINT:
				//case PF_RGB32_SINT:
			case PF_RGB32_FLOAT:			return GL_RGB;

				//case PF_RGBA32_UNORM:
				//case PF_RGBA32_SNORM:
				//case PF_RGBA32_UINT:
				//case PF_RGBA32_SINT:
			case PF_RGBA32_FLOAT:			return GL_RGBA;

			case PF_D16_UNORM:				return GL_DEPTH_COMPONENT;
			case PF_D24_UNORM_S8_UINT:		return GL_DEPTH_STENCIL_OES;
			case PF_D32_FLOAT:				return GL_DEPTH_COMPONENT;
			default:
				{
					LordAssertX("Unsupported pixel format [%s].", PixelUtil::GetPixelFormatName(pixFmt).c_str());
					return GL_BYTE;
				}
			}
		}

		static inline GLenum MapPrimitiveTopology(RenderInput::TopologyType type)
		{
			switch(type)
			{
			case RenderInput::TT_POINTLIST:		return GL_POINTS;
			case RenderInput::TT_LINELIST:		return GL_LINES;
			case RenderInput::TT_LINESTRIP:		return GL_LINE_STRIP;
			case RenderInput::TT_TRIANGLELIST:	return GL_TRIANGLES;
			case RenderInput::TT_TRIANGLESTRIP:	return GL_TRIANGLE_STRIP;
			default:							return GL_TRIANGLES;
			}
		}

		static inline GLenum MapComparisonFunc(RenderState::ComparisonFunc func)
		{
			switch(func)
			{
			case RenderState::CF_NEVER:			return GL_NEVER;
			case RenderState::CF_ALWAYS:		return GL_ALWAYS;
			case RenderState::CF_LESS:			return GL_LESS;
			case RenderState::CF_LESS_EQUAL:	return GL_LEQUAL;
			case RenderState::CF_EQUAL:			return GL_EQUAL;
			case RenderState::CF_NOT_EQUAL:		return GL_NOTEQUAL;
			case RenderState::CF_GREATER_EQUAL:	return GL_GEQUAL;
			case RenderState::CF_GREATER:		return GL_GREATER;
			default:
				{
					LordAssert(false)
						return GL_EQUAL;
				}
			}
		}

		static inline GLenum MapTextureType(Texture::TexType type)
		{
			switch(type)
			{
			case Texture::TT_2D:			return GL_TEXTURE_2D;
			case Texture::TT_CUBE:			return GL_TEXTURE_CUBE_MAP;
			default:
				{
                    LordAssert(false);
					return GL_TEXTURE_2D;
				}
			}
		}

		static inline String MapVertexSemanticString(RenderInput::VertexSemantic semantic)
		{
			String ret;
			switch(semantic)
			{
			case RenderInput::VS_POSITION:		ret = "inPosition";			break;
			case RenderInput::VS_BLENDINDICES:	ret = "inBlendIndices";		break;
			case RenderInput::VS_BLENDWEIGHTS:	ret = "inBlendWeights";		break;
			case RenderInput::VS_COLOR:			ret = "inColor";			break;
			case RenderInput::VS_NORMAL:		ret = "inNormal";			break;
			case RenderInput::VS_TEXCOORD:		ret = "inTexCoord";			break;
			case RenderInput::VS_TEXCOORD1:		ret = "inTexCoord1";			break;
			case RenderInput::VS_TANGENT:		ret = "inTangent";			break;
			case RenderInput::VS_BINORMAL:		ret = "inBinormal";			break;
			}
			return ret;
		}

		static inline ShaderParamType MapUniformType(GLenum uniformType)
		{
			switch (uniformType)
			{
			case GL_FLOAT: return SPT_FLOAT;
			case GL_FLOAT_VEC2: return SPT_VEC2;
			case GL_FLOAT_VEC3: return SPT_VEC3;
			case GL_FLOAT_VEC4: return SPT_VEC4;
			case GL_INT:		return SPT_INT;
			case GL_FLOAT_MAT4:	return SPT_MAT4;
			case GL_SAMPLER_2D: return SPT_TEXTURE;
			}

			return SPT_UNKNOWN;
		}
	};
}

#endif
