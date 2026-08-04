/********************************************************************
filename: 	GLES2Shader.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2SHADER_H__
#define __LORD_GLES2SHADER_H__

#include "Render/Shader.h"

namespace LORD
{
	class LORD_GLES2RENDER_API GLES2Shader: public Shader
	{
	public:
		GLES2Shader(ShaderType type, const ShaderDesc& desc, const String& filename, String& errMsg);
		~GLES2Shader();

	public:
		void			setProgramHandle(uint hProgram);
		GLuint			getShaderHandle() const;

	private:
		void			create(String& errMsg, const String& filename);

	private:
		GLuint			m_hShader;
		GLuint			m_hProgram;
	};
}

#endif
