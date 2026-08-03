#include "Core.h"
#include "Render/ShaderProgram.h"
#include "Object/Exception.h"
#include "Util/PathUtil.h"
#include "Render/Renderer.h"

namespace LORD
{
	// 构造函数
	ShaderProgram::ShaderProgram(ui32 uuid)
		: m_bLinked(false)
		, m_UUID(uuid)
	{
		for(uint i = 0; i < Shader::ST_SHADERCOUNT; ++i)
		{
			m_pShaders[i] = NULL;
		}
	}

	// 析构函数
	ShaderProgram::~ShaderProgram()
	{
		for (uint i = 0; i < Shader::ST_SHADERCOUNT; ++i)
		{
			m_pShaders[i] = NULL;
		}
		for (uint i = 0; i < m_uniforms.size(); ++i)
		{
			LordSafeDelete(m_uniforms[i]);
		}
		m_uniforms.clear();
	}

	bool ShaderProgram::attachShader(Shader* pShader)
	{
		if(!pShader)
			return false;

		Shader::ShaderType type = pShader->getShaderType();
		if(m_pShaders[(uint)type])
		{
			LordLogError("The shader [%s] has been already attached.", Shader::GetShaderTypeDesc(type).c_str());
			return false;
		}

		m_pShaders[(uint)type] = pShader;
		m_bLinked = false;

		return true;
	}

	Shader* ShaderProgram::detachShader(Shader::ShaderType type)
	{
		Shader* pShader = m_pShaders[(uint)type];
		m_pShaders[(uint)type] = NULL;

		m_bLinked = false;

		return pShader;
	}

	// 根据变量名称设置值
	/*void ShaderProgram::setUniform( const char* name, const void* value, ShaderParamType uniformType, ui32 count)
	{
	setUniform( getParamPhysicsIndex( name), value, uniformType, count);
	}*/

	// 根据物理索引设置变量值
	void ShaderProgram::setUniform( ui32 physicIdx, const void* value, ShaderParamType uniformType, ui32 count)
	{
		if( physicIdx<m_uniforms.size())
		{
			Uniform* uniform = m_uniforms[physicIdx];
			if (uniform)
			{
				uniform->setValue(value, count);

				LordAssert(uniform->m_type == uniformType);
				LordAssert(uniform->m_count >= (int)count);
			}
		}
		else
		{
			LordLogError( "ShaderProgram uniform [%d] not exist!", physicIdx);
		}
	}
}