/********************************************************************
filename: 	GLES2RenderInput.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2RENDERINPUT_H__
#define __LORD_GLES2RENDERINPUT_H__

#include "Render/RenderInput.h"

namespace LORD
{
	class LORD_GLES2RENDER_API GLES2RenderInput: public RenderInput
	{
	public:
		GLES2RenderInput(const VertexElementList& vertElms, ShaderProgram* pProgram);
		~GLES2RenderInput();

	public:
		void			bind();
		void			unbind();

	private:
		void			buildVertexAttributes();
	
	};
}

#endif
