/********************************************************************
filename: 	ShaderProgramManager.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_SHADERPROGRAME_MGR_H__
#define __LORD_SHADERPROGRAME_MGR_H__

#include "ShaderProgram.h"

namespace LORD
{
	class Renderer;
	class LORD_CORE_API ShaderProgramManager : public ObjectAlloc, public Singleton<ShaderProgramManager>
	{
	public:
		inline static ShaderProgramManager* Instance()
		{
			return ms_pSingleton;
		}
	public:
		typedef map<String, ShaderProgram*>::type ShaderProgramMap;
		typedef map<String, Shader*>::type ShaderHandleMap;
	public:
		ShaderProgramManager();
		virtual ~ShaderProgramManager();

		void initialize();
	public:
		// Free all shaderprogram
		void free();
		void loadFromFile(const String& filename);
		// load and parse by file
		bool loadShaderFrom(void* pNode);
		ShaderProgram* createShaderProgram(const String& vsFileName, const String& psFileName, ui32 uuid);
		ShaderProgram* getShaderProgram(const String& name);

		virtual bool bindShaderProgram(ui32 handle) = 0;
	private:
		ShaderProgramMap m_ShaderProgramMap;
		ShaderHandleMap m_Shaders;
	};
}

#endif
