#include "EntityTextureAtlas.h"

namespace BLOCKMAN
{

EntityTextureAtlas::EntityTextureAtlas(const String& name, int w, int h)
	: width(w)
	, height(h)
{
	m_name = name;
}

EntityTextureAtlas::~EntityTextureAtlas()
{
	m_pakSprites.clear();
}

bool EntityTextureAtlas::addSprite(const String& name, Image* pImg, int x, int y, int w, int h)
{
	if (m_pakSprites.find(name) != m_pakSprites.end())
		return false;
	if (!pImg)
		return false;

	PakSprite sprite;
	sprite.name = name;
	sprite.width = w;
	sprite.height = h;
	sprite.x = 0;
	sprite.y = 0;
	sprite.xOffset = x;
	sprite.yOffset = y;
	sprite.pImage = pImg;
	// copy texture pixels to image.
	
	m_pakSprites.insert(std::make_pair(name, sprite));
	return true;
}

bool EntityTextureAtlas::delSprite(const String& name)
{
	PakSpriteMap::iterator it = m_pakSprites.find(name);
	if (it == m_pakSprites.end())
		return false;

	m_pakSprites.erase(it);
	return true;
}

bool EntityTextureAtlas::texturePack()
{
	int dataSize = sizeof(ui32) * width * height;
	ui8* pImageData = (ui8*)LordMalloc(dataSize);
	memset(pImageData, 0, dataSize);
	Image* pImage = LordNew Image(pImageData, width, height, 1, PF_RGBA8_UNORM);

	// todo.
	return true;
}

PakResults EntityTextureAtlas::computeResult()
{
	PakResults pak;
	// todo.
	return pak;
}

PakResults EntityTextureAtlas::computeResult(int modes)
{
	PakResults pak;
	// todo.
	return pak;
}


}