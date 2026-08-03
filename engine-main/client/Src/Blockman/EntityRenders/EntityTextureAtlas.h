/********************************************************************
filename: 	EntityTextureAtlas.h
file path:	dev\client\Src\Blockman\EntityRender

version:	1
author:		ajohn
company:	supernano
date:		2017-4-20
*********************************************************************/
#ifndef __ENTITY_TEXTURE_ATLAS_HEADER__
#define __ENTITY_TEXTURE_ATLAS_HEADER__

#include "Render/TextureAtlas.h"

namespace BLOCKMAN
{
	
struct PakSprite
{
	String name;
	Image* pImage = nullptr;

	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int xOffset = 0;
	int yOffset = 0;
};

typedef vector<PakSprite>::type PakSprites;
typedef map<String, PakSprite>::type PakSpriteMap;

struct PakResult
{
	int width = 0;
	int height = 0;
	PakSprites sprites;
};
typedef vector<PakResult*>::type PakResults;

class EntityTextureAtlas : public TextureAtlas
{
	friend class TextureAtlasRegister;

public:
	EntityTextureAtlas(const String& name, int w, int h);
	virtual ~EntityTextureAtlas();

	bool addSprite(const String& name, Image* pImg, int x, int y, int w, int h);
	bool delSprite(const String& name);
	bool texturePack();
	
protected:
	PakResults computeResult();
	PakResults computeResult(int modes);

protected:
	int width = 0;
	int height = 0;
	PakSpriteMap m_pakSprites;
};

}

#endif
