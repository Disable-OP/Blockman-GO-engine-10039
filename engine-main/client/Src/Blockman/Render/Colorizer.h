/********************************************************************
filename: 	Colorizer.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-12-07
*********************************************************************/
#ifndef __COLORIZER_HEADER__
#define __COLORIZER_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

enum COLORIZER_TYPE
{
	COLORIZER_TYPE_INVALID,
	COLORIZER_TYPE_GRASS,
	COLORIZER_TYPE_FOLIAGE,
	COLORIZER_TYPE_COUNT,
};

class Colorizer : public Singleton<Colorizer>, public ObjectAlloc
{
public:
	Colorizer();
	~Colorizer();

	int initColorBuffer(COLORIZER_TYPE color_type, const String& textureName);
	int getColor(COLORIZER_TYPE color_type, float x, float y);

protected:
	ui32* m_ColorBuffer[COLORIZER_TYPE_COUNT];
	Vector2i m_arraySize[COLORIZER_TYPE_COUNT];
};

}

#endif