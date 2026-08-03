#include "GLES2RenderStd.h"
#include "GLES2Renderer.h"
#include "GLES2ShaderProgram.h"
#include "GLES2ShaderProgramManager.h"
#include "Object/Exception.h"
#include "Object/Root.h"

namespace LORD
{
	GLES2ShaderProgramManager::GLES2ShaderProgramManager()
		: ShaderProgramManager()
		, m_hProgram(0)
	{
	}

	GLES2ShaderProgramManager::~GLES2ShaderProgramManager()
	{
	}
	
	bool GLES2ShaderProgramManager::bindShaderProgram(ui32 handle)
	{
		GLuint hProgram = (GLuint)handle;
		if (m_hProgram != hProgram)
		{
			m_hProgram = hProgram;
			glUseProgram(m_hProgram);
			return true;
		}

		return false;
	}
	
}
