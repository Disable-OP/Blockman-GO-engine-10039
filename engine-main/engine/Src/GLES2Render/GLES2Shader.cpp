#include "GLES2RenderStd.h"
#include "GLES2Shader.h"
#include "Object/Exception.h"

namespace LORD
{
	GLES2Shader::GLES2Shader(ShaderType type, const ShaderDesc& desc, const String& filename, String& errMsg)
		: Shader(type, desc, filename, errMsg)
		, m_hShader(0)
		, m_hProgram(0)
	{
		create(errMsg, filename);
	}
	
	GLES2Shader::~GLES2Shader()
	{
		if(m_hShader)
		{
			glDeleteShader(m_hShader);
			m_hShader = 0;
		}
	}

	void GLES2Shader::setProgramHandle(uint hProgram)
	{
		m_hProgram = hProgram;
	}

	GLuint GLES2Shader::getShaderHandle() const
	{
		return m_hShader;
	}

	void GLES2Shader::create(String& errMsg, const String& filename)
	{
		if (!filename.empty())
		{
			DataStream* pShaderStream = ResourceGroupManager::Instance()->openResource(filename);
			if (!pShaderStream)
			{
				LordException("Shader file isn't exist. [%s]", filename.c_str());
			}

			m_shaderSize = (ui32)pShaderStream->size() + 1;
			m_pSrcData = (Byte*)(LordMalloc(m_shaderSize));
			m_pSrcData[m_shaderSize - 1] = '\0';

			pShaderStream->read(m_pSrcData, m_shaderSize);

			LordDelete pShaderStream;
		}
		else
		{
			LordException("GLES2Shader::create: Create Shader failed. filename is empty!");
		}
		switch(m_shaderType)
		{
			case ST_VERTEXSHADER:
			{
				m_hShader = glCreateShader(GL_VERTEX_SHADER);
				if(!m_hShader)
				{
					LordFree(m_pSrcData);

					if(filename.empty())
						LordException("Create vertex Shader failed.");
					else
						LordException("Create vertex Shader [%s] failed.", filename.c_str());
				}
			} break;
			case ST_PIXELSHADER:
			{
				m_hShader = glCreateShader(GL_FRAGMENT_SHADER);
				if(!m_hShader)
				{
					LordFree(m_pSrcData);

					if(filename.empty())
						LordException("Create pixel Shader failed.");
					else
						LordException("Create pixel Shader [%s] failed.", filename.c_str());
				}
			} break;
			default:
			{
				if(filename.empty())
					LordException("Unknown shader type, create Shader failed.");
				else
					LordException("Unknown shader type, create Shader [%s] failed.", filename.c_str());
			}break;
		}

		// bind shader source code to the shader object
		glShaderSource(m_hShader, 1, (const GLchar**)&m_pSrcData, NULL);
		LordSafeFree(m_pSrcData);

		// compile shader.
		glCompileShader(m_hShader);

		// check to see if the vertex shader compiled successfully.
		GLint status;
		glGetShaderiv(m_hShader, GL_COMPILE_STATUS, &status);
		if(status != 1)
		{
			// get the size of the string containing the information log for the failed shader compilation message.
			GLint logSize = 0;
			glGetShaderiv(m_hShader, GL_INFO_LOG_LENGTH, &logSize);

			// increment the size by one to handle also the null terminator.
			++logSize;

			// create a char buffer to hold the info log.
			char* szLogInfo = (char*)LordMalloc(logSize);

			// retrieve the info log.
			GLint realsize;
			glGetShaderInfoLog(m_hShader, logSize, &realsize, szLogInfo);

			errMsg = szLogInfo;
			LordSafeFree(szLogInfo);

			if(filename.empty())
				LordException("Compile Shader Error: \n%s", errMsg.c_str());
			else
				LordException("Compile Shader [%s] Error: \n%s", filename.c_str(), errMsg.c_str());
		}
	}

}
