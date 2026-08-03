/********************************************************************
filename: 	GLES2ShaderProgram.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2SHADERPROGRAM_H__
#define __LORD_GLES2SHADERPROGRAM_H__

#include "GLES2Shader.h"
#include "Render/ShaderProgram.h"
#include "Render/Renderable.h"

namespace LORD
{
	/**
	 * 着色器程序
	 */
	class LORD_GLES2RENDER_API GLES2ShaderProgram: public ShaderProgram
	{
	public:
		GLES2ShaderProgram(ui32 uuid);
		~GLES2ShaderProgram();

	public:
		bool		attachShader(Shader* pShader);
		Shader*		detachShader(Shader::ShaderType type);
		bool		linkShaders();

		// 应用变量
		virtual void bindUniforms();

		void		bind();
		void		unbind();

		// 根据参数名获取参数物理地址
		int	getParamPhysicsIndex(const String& paramName);
		
		// 获取句柄
		inline GLuint getProgramHandle() const { return m_hProgram; }
	
	private:
		bool setShaderParam(ShaderParamType type, const String& name, uint size, Byte* pData);

		// 根据变量类型获取其对应ByteSize
		int getUniformByteSizeByUniformType( ShaderParamType uniformType);

		// 设置采样纹理
		void setSampler(int physicsLocation, uint stage);
	
	private:
		GLuint			m_hProgram;
	};

}

#endif
