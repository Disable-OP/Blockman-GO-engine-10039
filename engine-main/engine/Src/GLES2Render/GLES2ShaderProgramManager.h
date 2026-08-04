/********************************************************************
filename: 	GLES2ShaderProgramManager.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2SHADERPROGRAM_MGR_H__
#define __LORD_GLES2SHADERPROGRAM_MGR_H__

#include "GLES2Shader.h"
#include "Render/ShaderProgramManager.h"
#include "Render/Renderable.h"

namespace LORD
{
	class LORD_GLES2RENDER_API GLES2ShaderProgramManager : public ShaderProgramManager
	{
	public:
		GLES2ShaderProgramManager();
		virtual	~GLES2ShaderProgramManager();

	public:
		bool bindShaderProgram(ui32 handle);
	private:
		GLuint m_hProgram;
	};
}

#endif
