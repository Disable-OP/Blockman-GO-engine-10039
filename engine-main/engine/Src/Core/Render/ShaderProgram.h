/********************************************************************
filename: 	ShaderProgram.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_SHADERPROGRAM_H__
#define __LORD_SHADERPROGRAM_H__

#include "Shader.h"

namespace LORD
{
	// 支持的着色器参数类型
	enum ShaderParamType
	{
		SPT_UNKNOWN,  
		SPT_INT, 
		SPT_FLOAT, 
		SPT_VEC2, 
		SPT_VEC3, 
		SPT_VEC4,
		SPT_MAT4,
		SPT_TEXTURE, 
	};

	/**
	 * ShaderProgram 2014-11-18
	 */
	class LORD_CORE_API ShaderProgram: public ObjectAlloc
	{
	public:
		// 参数描述
		struct Uniform : public ObjectAlloc
		{
			String			m_name;			// 名称
			ShaderParamType	m_type;			// 类型
			int				m_count;		// 大小
			int				m_sizeInBytes;	// 大小
			int				m_location;		// 位置
			Byte*			m_value;		// 当前值
			bool			m_isDirty;		// 值是否发生了变化

			// 构造函数
			Uniform()
				: m_name( "UnKnown")
				, m_type( SPT_UNKNOWN)
				, m_count( -1)
				, m_location( -1)
				, m_value( NULL)
				, m_isDirty( true)
			{}

			// 析构函数
			~Uniform()
			{
				LordSafeFree( m_value);
			}

			// 重置当前值
			void resetValue()
			{
				m_isDirty = true;
			}

			// 设置值
			void setValue( const void* value, int count)
			{
				LordAssert( value);

				// 分配堆空间
				if( !m_value)
				{
					m_value = (Byte*)LordMalloc( m_sizeInBytes);
				}

				// 值比较
				int buffsize = m_sizeInBytes / m_count * count;
				if (memcmp(this->m_value, value, buffsize) != 0)
				{
					m_isDirty = true;
					memcpy(m_value, value, buffsize);
				}
			}

		};
		typedef std::vector<Uniform*> UniformArray;

	public:
		ShaderProgram(ui32 uuid);
		virtual ~ShaderProgram();

		// 获取关连着色器
		Shader* getShader(Shader::ShaderType type) const { return m_pShaders[(uint)type]; }

		virtual bool				attachShader(Shader* pShader);
		virtual Shader*				detachShader(Shader::ShaderType type);
		virtual bool				linkShaders() = 0;

		inline bool isLinked() const { return m_bLinked; }

		// 根据参数名获取参数物理地址
		virtual int getParamPhysicsIndex(const String& paramName)=0;
		
		virtual void bind() = 0;
		virtual void unbind() = 0;
	public:
		// 根据变量名称设置值
		//void setUniform( const char* name, const void* value, ShaderParamType uniformType, ui32 count);

		// 根据物理索引设置变量值
		void setUniform( ui32 physicIdx, const void* value, ShaderParamType uniformType, ui32 count);

		// 应用变量
		virtual void bindUniforms()=0;

		ui32 getUUID() const { return m_UUID; }
	protected:
		bool				m_bLinked;
		ui32				m_UUID;
		Shader*				m_pShaders[Shader::ST_SHADERCOUNT];
		UniformArray		m_uniforms;							// 参数信息
	};
}
#endif
