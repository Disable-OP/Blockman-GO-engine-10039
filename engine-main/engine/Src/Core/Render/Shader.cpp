#include "Core.h"
#include "Shader.h"
#include "Object/Exception.h"
#include "Util/PathUtil.h"
#include "Renderer.h"
#include "Resource/DataStream.h"
#include "Resource/ResourceGroupManager.h"
#include "Image/Image.h"

namespace LORD
{
	Shader::Shader(ShaderType type, const ShaderDesc &desc, const String &filename, String &errMsg)
		: m_filename(filename)
		, m_pSrcData(NULL)
		, m_shaderType(type)
		, m_func(desc.func)
		, m_profile(desc.profile)
	{
	}

	Shader::~Shader()
	{
		LordSafeFree(m_pSrcData);
	}

	Shader::ShaderType Shader::getShaderType() const
	{
		return m_shaderType;
	}

	String Shader::GetShaderTypeDesc(ShaderType type)
	{
		switch(type)
		{
		case ST_VERTEXSHADER:	return "ST_VERTEXSHADER";
		case ST_PIXELSHADER:	return "ST_PIXELSHADER";
		default:				return "UNKNOWN";
		}
	}
}
