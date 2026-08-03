#include "GLES2RenderStd.h"
#include "GLES2Renderer.h"
#include "GLES2RenderInput.h"
#include "GLES2ShaderProgram.h"
#include "GLES2Mapping.h"
#include "GLES2Loader.h"
#include "Util/AssertX.h"
#include "Object/Exception.h"
#include "Image/PixelFormat.h"

namespace LORD
{
	GLES2RenderInput::GLES2RenderInput(const VertexElementList& vertElms, ShaderProgram* pProgram)
		: RenderInput(vertElms, pProgram)
	{
		uint numVertElms = (uint)vertElms.size();
		if(numVertElms == 0)
			LordException("Vertex elements size error, create GLES2RenderInput failed.");

		GLES2ShaderProgram* pGLES2Program = (GLES2ShaderProgram*)pProgram;
		GLuint hProgram = pGLES2Program->getProgramHandle();

		m_vertDeclaration.reserve(numVertElms);
		m_vertDeclaration.resize(numVertElms);

		uint elmOffset = 0;
		for(size_t i = 0; i < numVertElms; ++i)
		{
			String strName = GLES2Mapping::MapVertexSemanticString(vertElms[i].semantic);
			GLint loc = glGetAttribLocation(hProgram, strName.c_str());
			LordAssert(loc>=0);
			m_vertDeclaration[i].index = loc;
			m_vertDeclaration[i].count = PixelUtil::GetChannelCount(vertElms[i].pixFmt);
			m_vertDeclaration[i].type = GLES2Mapping::MapDataType(vertElms[i].pixFmt);
			m_vertDeclaration[i].bNormalize = PixelUtil::IsNormalized(vertElms[i].pixFmt);
			m_vertDeclaration[i].elementOffset = elmOffset;
			elmOffset += PixelUtil::GetPixelSize(vertElms[i].pixFmt);
		}
		m_vertStride = elmOffset;

	}
	
	GLES2RenderInput::~GLES2RenderInput()
	{
	}
		
	void GLES2RenderInput::bind()
	{
		GLuint hVBO = ((GLES2GPUBuffer*)m_pVertBuff)->getVBOHandle();
		glBindBuffer(GL_ARRAY_BUFFER, hVBO);
		
		size_t declarationSize = m_vertDeclaration.size();
		for(size_t i = 0; i < declarationSize; ++i)
		{
			VertexDeclaration& declaration = m_vertDeclaration[i];
			
			// Enable the vertex array attributes.
			glEnableVertexAttribArray(declaration.index);
			glVertexAttribPointer(declaration.index,
				declaration.count,
				declaration.type,
				declaration.bNormalize,
				m_vertStride,
				(GLvoid*)declaration.elementOffset);
		}
		
		if(m_pIdxBuff)
		{
			// Bind the index buffer and load the index data into it.
			GLuint hIdxVBO = ((GLES2GPUBuffer*)m_pIdxBuff)->getVBOHandle();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIdxVBO);
		}
	}
	
	void GLES2RenderInput::unbind()
	{
		size_t declarationSize = m_vertDeclaration.size();
		for(size_t i = 0; i < declarationSize; ++i)
		{
			glDisableVertexAttribArray(m_vertDeclaration[i].index);
		}

		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		if(m_pIdxBuff)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
	/*
		if(m_hVAO)
		{
	#if defined(LORD_LOAD_GLES_EXT)
			GLES2Loader::Instance()->bindVertexArrayOES(0);
	#else
			glBindVertexArrayOES(0);
	#endif
		}
	*/
	}

}
