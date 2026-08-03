/********************************************************************
filename: 	Shader.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_SHADER_H__
#define __LORD_SHADER_H__

#include "Core.h"
#include "Object/Buffer.h"

namespace LORD
{
	class LORD_CORE_API Shader: public ObjectAlloc
	{
	public:
		enum ShaderType
		{
			ST_VERTEXSHADER, 
			ST_PIXELSHADER, 
			ST_SHADERCOUNT, 
		};

		enum ShaderLang
		{
			SL_HLSL,
			SL_GLSL,
			SL_CG,
		};

		struct ShaderDesc
		{
			String		func;
			String		profile;
		};

		Shader(ShaderType type, const ShaderDesc& desc, const String& filename, String& errMsg);
		virtual ~Shader();


	public:
		virtual ShaderType		getShaderType() const;
		static String			GetShaderTypeDesc(ShaderType type);

	protected:
		ShaderType			m_shaderType;
		String				m_filename;
		Byte*				m_pSrcData;
		uint				m_shaderSize;
		String				m_profile;
		String				m_func;
	};
}

#endif