/********************************************************************
filename: 	TextureAtlas.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-28
*********************************************************************/
#ifndef __TEXTURE_ATLAS_HEADER__
#define __TEXTURE_ATLAS_HEADER__

#include "Core.h"
#include "UI/GUIImageset.h"

using namespace LORD;

namespace BLOCKMAN
{

enum SPRITE_TYPE
{
	SPRITE_TYPE_INVALID = -1,
	
	SPRITE_TYPE_FRAME,
	SPRITE_TYPE_ANIMATION,

	SPRITE_TYPE_COUNT,
};

class AtlasSprite : public ObjectAlloc
{
	friend class MemTextureAtlas;
public:
	AtlasSprite(SPRITE_TYPE type = SPRITE_TYPE_INVALID);
	AtlasSprite(const AtlasSprite& rhs);

	virtual float getInterpolatedU(float fu) { return uv_begin.x + (uv_end.x - uv_begin.x) * fu; }
	virtual float getInterpolatedV(float fv) { return uv_begin.y + (uv_end.y - uv_begin.y) * fv; }
	virtual float getMinU() const { return uv_begin.x; }
	virtual float getMinV() const { return uv_begin.y; }
	virtual float getMaxU() const { return uv_end.x; }
	virtual float getMaxV() const { return uv_end.y; }

	const Vector2i& getFramePos() const { return frame_pos; }
	const Vector2i& getFrameSize() const { return frame_size; }
	int getOriginX() const { return source_size.x; }
	int getOriginY() const { return source_size.y; }
	Texture* getTexture() { return texture; }
	const String& getSpriteName() const { return name; }
	const String getSpriteFullName() const 
	{ 
		if (nameSpace.empty())
		{
			return name;
		}
		else
		{
			return nameSpace + ":" + name;
		}
	}
	SPRITE_TYPE getType() const { return type; }
	Vector4 getAtlasVec4() const;

	int parseFromJson(void* pJson, Texture* tex, const String& nameSpace = "");
	void setManual(const String& name, const Vector2i& pos, const Vector2i& size, Texture* tex);

protected:
	void caculate();

protected:
	String name;
	String nameSpace;
	SPRITE_TYPE type = SPRITE_TYPE_INVALID;
	Vector2 uv_begin;
	Vector2 uv_end;
	Vector2i frame_pos;
	Vector2i frame_size;
	Vector2i source_pos;
	Vector2i source_size;
	Texture* texture = nullptr;
};

class AtlasSpriteFlipped : public AtlasSprite
{
public:
	AtlasSpriteFlipped(const AtlasSprite& rhs, bool flipU, bool flipV);
	
	virtual float getInterpolatedU(float fu) { return getMinU() + (getMaxU() - getMinU()) * fu; }
	virtual float getInterpolatedV(float fv) { return getMinV() + (getMaxV() - getMinV()) * fv; }
	virtual float getMinU() const { return m_flipU ? uv_end.x : uv_begin.x; }
	virtual float getMinV() const { return m_flipV ? uv_end.y : uv_begin.y; }
	virtual float getMaxU() const { return m_flipU ? uv_begin.x : uv_end.x; }
	virtual float getMaxV() const { return m_flipV ? uv_begin.y : uv_end.y; }

private:
	bool m_flipU = false;
	bool m_flipV = false;
};

typedef map<String, int>::type AtlasSpriteMap;
typedef vector<AtlasSprite>::type AtlasSpriteArr;

class TextureAtlas : public ObjectAlloc
{
friend class TextureAtlasRegister;

public:
	int loadFromJson(const String& atlasFile, const String& nameSpace = "");

	AtlasSprite* getAtlasScript(const String& spriteName);
	AtlasSprite* getAtlasScript(int idx);
	int	getAltasScriptIdx(const String& spriteName);
	const String& getName() const { return m_name; }
	const String& getNamespace() const{ return m_namespace; }
	Texture* getTexture() { return m_TexturePtr; }
	void getAtlasSpriteArr(GUIImageset::ImageSetUnitArr& outImages);

	virtual void updateTextureAtlas() {}


	TextureAtlas();
	TextureAtlas(const String& atlasFile);

	virtual ~TextureAtlas();

protected:

	void release();
	virtual int parseJsonSprites(void* pJsonSprites);
	static SPRITE_TYPE getTypeFromString(const String& typeStr);

protected:
	String			m_name;
	String			m_namespace;
	AtlasSpriteMap	m_AtlasSpriteMap;
	AtlasSpriteArr	m_AtlasSpriteArr;
	Texture*		m_TexturePtr = nullptr;
	SPRITE_TYPE		m_type = SPRITE_TYPE_INVALID;
};

typedef vector<TextureAtlas*>::type TextureAtlasArr;


class MemTextureAtlas : public TextureAtlas
{
	friend class TextureAtlasRegister;

public:
	int getMipHeight(int mip);
	int getMipWidth(int mip);
	ui32* getPixelMemMip(int mip);
	Texture* getMipTexture(int mip);
	AtlasSprite* getMipAtlasScript(int mip, const String& spriteName);
	AtlasSprite* getMipAtlasScript(int mip, int idx);
	void getMipAtlasSpriteArr(int mip, GUIImageset::ImageSetUnitArr& outImages);

protected:
	MemTextureAtlas();
	MemTextureAtlas(const String& atlasFile);
	
	virtual ~MemTextureAtlas();

	int load(const String& jsonName);
	void loadMip(const String& texName);
	virtual void updateTextureAtlas() {}
	virtual int parseJsonSprites(void* pJsonSprites);
	int copyImageToTexture(const String& name, void* json, AtlasSprite& atlas);

	struct FrameAtlasSprite
	{
		int m_memIndex;
		int m_texIndex;
		Texture* tex;
	};
	typedef vector<FrameAtlasSprite>::type FrameAtlasSpriteArr;


protected:
	int m_width = 0;
	int m_height = 0;
	int m_next64Index = 0;
	int m_next32Index = 0;
	int m_32Index = -1;
	int m_next16Index = 0;
	int m_16Index = -1;

	static const int maxMip = 2; // 32 16
	ui32* m_mip[maxMip] = { nullptr, nullptr };
	Texture* m_mipTex[maxMip]{ nullptr, nullptr };
	AtlasSpriteMap m_mipAtlasMap[maxMip];
	AtlasSpriteArr m_mipAtlasArr[maxMip];

	String m_resourceFolder;
	FrameAtlasSpriteArr FrameAtlasArr;
	int* m_textureBuffer = nullptr;
};

class TextureAtlasRegister : public Singleton<TextureAtlasRegister>, public ObjectAlloc
{
public:
	TextureAtlasRegister();
	~TextureAtlasRegister();

	int addTextureAtlas(const String& atlasname, const String& prefix = ""); 
	int addMemTextureAtlas(const String& atlasname, const String& folder);

	TextureAtlas* getTextureAtlas(const String& atlasname);
	Texture*	getTexture(const String& texturename);
	AtlasSprite* getAtlasSprite(const String& spritename);

	//add by maxicheng
	AtlasSprite* getAtlasSprite(const String& atlasName, const String& spriteName);

protected:
	TextureAtlasArr m_textureAtlasArr;
};

}

#endif