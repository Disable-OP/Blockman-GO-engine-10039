#include "GLES2RenderStd.h"
#include "GLES2Renderer.h"
#include "GLES2ShaderProgram.h"
#include "GLES2Mapping.h"
#include "Render/ShaderProgramManager.h"
#include "Object/Exception.h"
#include "Object/Root.h"

namespace LORD
{
	GLES2ShaderProgram::GLES2ShaderProgram(ui32 uuid)
		: ShaderProgram(uuid)
	{
		m_hProgram = glCreateProgram();
		if(!m_hProgram)
		{
			LordException("Create GLES2ShaderProgram failed.");
		}
	}

	GLES2ShaderProgram::~GLES2ShaderProgram()
	{
		detachShader(Shader::ST_VERTEXSHADER);
		detachShader(Shader::ST_PIXELSHADER);

		glDeleteProgram(m_hProgram);
		m_hProgram = 0;
	}
	
	bool GLES2ShaderProgram::attachShader(Shader* pShader)
	{
		bool bRet = ShaderProgram::attachShader(pShader);
		if(!bRet)
			return false;

		GLES2Shader* pGLES2Shader = (GLES2Shader*)pShader;
		LordAssert(pGLES2Shader);
		uint hShader = pGLES2Shader->getShaderHandle();
		glAttachShader(m_hProgram, hShader);

		return true;
	}
	
	Shader* GLES2ShaderProgram::detachShader(Shader::ShaderType type)
	{
		Shader* pShader = ShaderProgram::detachShader(type);
		if(pShader)
		{
			GLES2Shader* pGLES2Shader = (GLES2Shader*)pShader;
			LordAssert(pGLES2Shader);
			uint hShader = pGLES2Shader->getShaderHandle();
			glDetachShader(m_hProgram, hShader);
		}

		return pShader;
	}

	// 根据变量类型获取其对应ByteSize
	int GLES2ShaderProgram::getUniformByteSizeByUniformType( ShaderParamType uniformType)
	{
		switch( uniformType)
		{
		case SPT_UNKNOWN:			return 0;  
		case SPT_INT:				return 4;  
		case SPT_FLOAT:				return 4;  
		case SPT_VEC2:				return 8;  
		case SPT_VEC3:				return 12;  
		case SPT_VEC4:				return 16;  
		case SPT_MAT4:				return 64;
		case SPT_TEXTURE:			return 4;
		default:					return 0;
		}
	}

	bool GLES2ShaderProgram::linkShaders()
	{
		glLinkProgram(m_hProgram);

		// Check the status of the link.
		GLint status;
		glGetProgramiv(m_hProgram, GL_LINK_STATUS, &status);
		if(status != 1)
		{
			// get the size of the string containing the information log for the failed shader compilation message.
			GLint logSize = 0;
			glGetProgramiv(m_hProgram, GL_INFO_LOG_LENGTH, &logSize);

			// increment the size by one to handle also the null terminator.
			++logSize;

			// create a char buffer to hold the info log.
			char* szLogInfo = (char*)LordMalloc(logSize);

			// retrieve the info log.
			glGetProgramInfoLog(m_hProgram, logSize, NULL, szLogInfo);

			String errMsg = szLogInfo;
			LordSafeFree(szLogInfo);

			LordLogError("Link Shaders Error: \n%s", errMsg.c_str());
			return false;
		}

		m_bLinked = true;
		LordLogDebug("Link shaders successd.");
	
		// 记录常量信息
		GLint activeUniformLength;
		glGetProgramiv( m_hProgram, GL_ACTIVE_UNIFORMS, &activeUniformLength);

		// 最大物理地址
		int maxUniformLocation = 0;
		// 记录常量信息
		UniformArray uniformArray;
		for( GLint i=0; i<activeUniformLength; i++)
		{
			char    unifromName[512];			
			GLint   uniformSize;
			GLenum  uniformType;
			GLsizei uniformLength;
			glGetActiveUniform( m_hProgram, i, 512, &uniformLength, &uniformSize, &uniformType, unifromName);

			Uniform* desc = LordNew Uniform;
			desc->m_name		   = StringUtil::Replace( unifromName, "[0]", "").c_str();
			desc->m_type = GLES2Mapping::MapUniformType(uniformType);
			desc->m_count = uniformSize;
			desc->m_sizeInBytes = desc->m_count * getUniformByteSizeByUniformType(desc->m_type);
			desc->m_location = glGetUniformLocation(m_hProgram, desc->m_name.c_str());
			uniformArray.push_back( desc);

			maxUniformLocation = std::max<int>(maxUniformLocation, desc->m_location);
		}

		m_uniforms.resize( maxUniformLocation+1);

		if( m_uniforms.size() > 512)
		{
			LordLogError( "what's wrong with the uniform location!");
		}

		// 重新整理变量信息
		for( size_t i=0; i<uniformArray.size(); i++)
		{
			m_uniforms[uniformArray[i]->m_location] = uniformArray[i];
		}

		return true;
	}
	
	// 设置采样状态
	void GLES2ShaderProgram::setSampler( int physicsLocation, uint stage )
	{
		glUniform1i(physicsLocation, stage);
	}

	// 应用变量
	void GLES2ShaderProgram::bindUniforms()
	{
		for( size_t i=0; i<m_uniforms.size(); i++)
		{
			Uniform* uniform = m_uniforms[i];
			if (uniform)
			{
				if (uniform->m_isDirty && uniform->m_type != SPT_UNKNOWN)
				{
					switch (uniform->m_type)
					{
					case SPT_VEC4:		glUniform4fv(uniform->m_location, uniform->m_count, (GLfloat*)uniform->m_value);					break;
					case SPT_MAT4:		glUniformMatrix4fv(uniform->m_location, uniform->m_count, false, (GLfloat*)uniform->m_value);		break;
					case SPT_INT:		glUniform1iv(uniform->m_location, uniform->m_count, (GLint*)uniform->m_value);					break;
					case SPT_FLOAT:		glUniform1fv(uniform->m_location, uniform->m_count, (GLfloat*)uniform->m_value);					break;
					case SPT_VEC2:		glUniform2fv(uniform->m_location, uniform->m_count, (GLfloat*)uniform->m_value);					break;
					case SPT_VEC3:		glUniform3fv(uniform->m_location, uniform->m_count, (GLfloat*)uniform->m_value);					break;
					case SPT_TEXTURE:	setSampler(uniform->m_location, *(uint*)uniform->m_value);										break;
					default:			LordAssertX(0, "unknow shader param format!");													break;
					}

					// 修改状态
					uniform->m_isDirty = false;
				}
			}
		}
	}
	
	// 绑定着色器
	void GLES2ShaderProgram::bind()
	{
		LordAssertX(m_bLinked, "The shader program has not been linked.");
	
		// Install the shader program as part of the current rendering state.
		if (ShaderProgramManager::Instance()->bindShaderProgram(m_hProgram))
		{
			// 所有变量设置脏标记
			for (UniformArray::iterator it = m_uniforms.begin(); it != m_uniforms.end(); it++)
			{
				Uniform* uniform = *it;
				if (uniform)
				{
					uniform->m_isDirty = true;
				}
			}
		}		
	}
	
	// 解绑着色器
	void GLES2ShaderProgram::unbind()
	{
		//glUseProgram(0);
	}
	
	// 获取参数物理地址
	int GLES2ShaderProgram::getParamPhysicsIndex( const String& paramName)
	{
		for( UniformArray::iterator it=m_uniforms.begin(); it!=m_uniforms.end(); it++)
		{
			Uniform* uniform = *it;
			if (uniform && uniform->m_name == paramName)
				return (*it)->m_location;
		}

		LordLogError( "%s is not param of shader program", paramName.c_str());
		LordAssert( false);

		return 0;
	}
}
