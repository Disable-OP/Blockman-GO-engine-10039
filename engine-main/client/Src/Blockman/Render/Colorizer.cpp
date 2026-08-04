#include "Colorizer.h"
#include "Common.h"

#include "Image/Image.h"

namespace BLOCKMAN
{

Colorizer::Colorizer()
{
	for (int i = 0; i < COLORIZER_TYPE_COUNT; ++i)
	{
		m_ColorBuffer[i] = NULL;
		m_arraySize[i] = Vector2i::ZERO;
	}
}

Colorizer::~Colorizer()
{
	for (int i = 0; i < COLORIZER_TYPE_COUNT; ++i)
		LordSafeFree(m_ColorBuffer[i]);
}

int Colorizer::initColorBuffer(COLORIZER_TYPE t, const String& textureName)
{
	int nResult = false;

	DataStream* pStream = NULL;
	Byte* pData = NULL;
	Image* pImage = NULL;
	int nSize = 0;
	
	do 
	{
		ERROR_BREAK(!m_ColorBuffer[t]);

		pStream = ResourceGroupManager::Instance()->openResource(textureName);
		ERROR_BREAK(pStream);

		pData = (Byte*)(LordMalloc(pStream->size()));
		ERROR_BREAK(pData);

		nSize = pStream->size();
		pStream->read(pData, nSize);
		LordSafeDelete(pStream);

		Buffer imageBuffer(nSize, pData);
		Image* pImage = Image::CreateFromMemory(imageBuffer, Image::GetImageFormat(textureName));
		ERROR_BREAK(pImage);

		m_arraySize[t].x = pImage->getWidth();
		m_arraySize[t].y = pImage->getHeight();
		m_ColorBuffer[t] = (ui32*)LordMalloc(sizeof(ui32) * m_arraySize[t].x * m_arraySize[t].y);
		memcpy(m_ColorBuffer[t], pImage->getData(), sizeof(ui32) * m_arraySize[t].x * m_arraySize[t].y);
		for (int i = 0; i < m_arraySize[t].x; ++i)
		{
			for (int j = 0; j < m_arraySize[t].y; ++j)
			{
				int c = m_ColorBuffer[t][j*m_arraySize[t].y + i];
				int r = c & 0xFF0000;
				int g = c & 0x00FF00;
				int b = c & 0x0000FF;
				m_ColorBuffer[t][j*m_arraySize[t].y + i] = 0xFF000000 | b << 16 | g | r >> 16;
			}
		}
		LordSafeDelete(pImage);
		LordSafeFree(pData);

		nResult = true;
	} while (0);

	if (!nResult)
	{
		if (pStream)
			LordSafeDelete(pStream);
		if (pData)
			LordSafeFree(pData);
		if (pImage)
			LordSafeDelete(pImage);
	}

	return nResult;
}

int Colorizer::getColor(COLORIZER_TYPE color_type, float x, float y)
{
	if (!m_ColorBuffer[color_type])
		return 0xFFFFFFFF;

	y *= x;
	int ix = (int)((1.0f - x) * (m_arraySize[color_type].x - 1));
	int iy = (int)((1.0f - y) * (m_arraySize[color_type].x - 1));

	return m_ColorBuffer[color_type][iy*m_arraySize[color_type].x + ix];
}

}