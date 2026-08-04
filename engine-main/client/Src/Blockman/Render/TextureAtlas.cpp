#include "TextureAtlas.h"
#include "Common.h"

#include "Resource/ResourceManager.h"
#include "Resource/ResourceGroupManager.h"

#include "External/cjson.h"

namespace BLOCKMAN
{

AtlasSprite::AtlasSprite(SPRITE_TYPE _type)
	: type(_type)
	, uv_begin(Vector2::ZERO)
	, uv_end(Vector2::ZERO)
	, frame_pos(Vector2i::ZERO)
	, frame_size(Vector2i::ONE)
	, source_pos(Vector2i::ZERO)
	, source_size(Vector2i::ONE)
	, texture(NULL)
{}

AtlasSprite::AtlasSprite(const AtlasSprite& rhs)
	: name(rhs.name)
	, type(rhs.type)
	, uv_begin(rhs.uv_begin)
	, uv_end(rhs.uv_end)
	, frame_pos(rhs.frame_pos)
	, frame_size(rhs.frame_size)
	, source_pos(rhs.source_pos)
	, source_size(rhs.source_size)
	, texture(rhs.texture)
{}

AtlasSpriteFlipped::AtlasSpriteFlipped(const AtlasSprite& rhs, bool flipU, bool flipV)
	: AtlasSprite(rhs)
	, m_flipU(flipU)
	, m_flipV(flipV)
{}

int AtlasSprite::parseFromJson(void* pJson, Texture* tex, const String& nameSpace)
{
	int nResult = false;
	cJSON* pSprite = (cJSON*)(pJson);
	do 
	{
		this->nameSpace = nameSpace;
		name = pSprite->string;

		cJSON* pSpriteFrame = cJSON_GetObjectItem(pSprite, "frame");
		ERROR_BREAK(pSpriteFrame);
		cJSON* pSpriteOffset = cJSON_GetObjectItem(pSprite, "offset");
		ERROR_BREAK(pSpriteOffset);
		cJSON* pSpriteSource = cJSON_GetObjectItem(pSprite, "sourceSize");
		ERROR_BREAK(pSpriteSource);

		cJSON* jsInt = cJSON_GetObjectItem(pSpriteFrame, "x");
		ERROR_BREAK(jsInt);
		frame_pos.x = jsInt->valueint;
		jsInt = cJSON_GetObjectItem(pSpriteFrame, "y");
		ERROR_BREAK(jsInt);
		frame_pos.y = jsInt->valueint;
		jsInt = cJSON_GetObjectItem(pSpriteFrame, "w");
		ERROR_BREAK(jsInt);
		frame_size.x = jsInt->valueint;
		jsInt = cJSON_GetObjectItem(pSpriteFrame, "h");
		ERROR_BREAK(jsInt);
		frame_size.y = jsInt->valueint;

		jsInt = cJSON_GetObjectItem(pSpriteOffset, "x");
		ERROR_BREAK(jsInt);
		source_pos.x = jsInt->valueint;
		jsInt = cJSON_GetObjectItem(pSpriteOffset, "y");
		ERROR_BREAK(jsInt);
		source_pos.y = jsInt->valueint;

		jsInt = cJSON_GetObjectItem(pSpriteSource, "w");
		ERROR_BREAK(jsInt);
		source_size.x = jsInt->valueint;
		jsInt = cJSON_GetObjectItem(pSpriteSource, "h");
		ERROR_BREAK(jsInt);
		source_size.y = jsInt->valueint;
		
		texture = tex;

		caculate();

		nResult = true;
	} while (0);

	return nResult;
}

void AtlasSprite::setManual(const String& _name, const Vector2i& pos, const Vector2i& size, Texture* tex)
{
	name = _name;
	frame_pos = pos;
	frame_size = size;
	texture = tex;
	source_size.x = tex->getWidth();
	source_size.y = tex->getHeight();
	caculate();
}

Vector4 AtlasSprite::getAtlasVec4() const
{
	Vector4 ret;
	ret.x = uv_begin.x;
	ret.y = uv_begin.y;
	ret.w = uv_end.x - uv_begin.x;
	ret.z = uv_end.y - uv_begin.y;
	return ret;
}

void AtlasSprite::caculate()
{
	LordAssert(texture);

	int texWidth = texture->getWidth();
	int texHeight = texture->getHeight();
	uv_begin.x = float(frame_pos.x) / texWidth;
	uv_begin.y = float(frame_pos.y) / texHeight;
	uv_end.x = float(frame_pos.x + frame_size.x) / texWidth;
	uv_end.y = float(frame_pos.y + frame_size.y) / texHeight;
}

TextureAtlas::TextureAtlas()
	: m_TexturePtr(NULL)
	, m_type(SPRITE_TYPE_INVALID)
{}

MemTextureAtlas::MemTextureAtlas()
	: TextureAtlas()
{}

TextureAtlas::TextureAtlas(const String& atlasFile)
	: m_TexturePtr(NULL)
{
	loadFromJson(atlasFile);
}

MemTextureAtlas::MemTextureAtlas(const String& atlasFile)
	: TextureAtlas()
{
	m_name = atlasFile;
}

TextureAtlas::~TextureAtlas()
{
	release();
}

void TextureAtlas::release()
{
	if (m_TexturePtr)
	{
		TextureManager::Instance()->releaseResource(m_TexturePtr);
		m_TexturePtr = NULL;
	}
	m_AtlasSpriteMap.clear();
}

int TextureAtlas::loadFromJson(const String& atlasFile, const String& nameSpace)
{
	int nResult = false;
	int nRetCode = false;
	DataStream* pData = NULL;
	cJSON* pJsonRoot = NULL;
	char* pMemory = NULL;

	do 
	{
		pData = ResourceGroupManager::Instance()->openResource(atlasFile);
		ERROR_BREAK(pData);

		m_name = atlasFile;
		m_namespace = nameSpace;

		pMemory = (char*)LordMalloc(sizeof(char) * pData->size());
		pData->read(pMemory, pData->size());

		pJsonRoot = cJSON_Parse(pMemory);
		ERROR_BREAK(pJsonRoot);
		
		cJSON* jsonMeta = cJSON_GetObjectItem(pJsonRoot, "meta");
		ERROR_BREAK(jsonMeta);

		cJSON* jsImage = cJSON_GetObjectItem(jsonMeta, "image");
		ERROR_BREAK(jsImage);

		String textureName = ResourceGroupManager::Instance()->FindFileArchive(atlasFile)->getFileName(jsImage->valuestring);

		cJSON* jsType = cJSON_GetObjectItem(jsonMeta, "type");
		ERROR_BREAK(jsType);
		m_type = getTypeFromString(jsType->valuestring);

		m_TexturePtr = TextureManager::Instance()->createTexture(textureName);
		ERROR_BREAK(m_TexturePtr);
		// io and load the texture in single-thread!
		m_TexturePtr->prepare();
		m_TexturePtr->load();

		if (m_type == SPRITE_TYPE_FRAME)
		{
			cJSON* jsSprites = cJSON_GetObjectItem(pJsonRoot, "frames");
			ERROR_BREAK(jsSprites);

			nRetCode = parseJsonSprites(jsSprites);
			ERROR_BREAK(nRetCode);
		}
		else
		{
			cJSON* jsName = cJSON_GetObjectItem(jsonMeta, "name");
			ERROR_BREAK(jsName);
			String name = jsName->valuestring;
			AtlasSprite atlas(SPRITE_TYPE_ANIMATION);
			int width = m_TexturePtr->getWidth();
			atlas.setManual(name, Vector2i::ZERO, Vector2i(width, width), m_TexturePtr);
			m_AtlasSpriteMap.insert(std::make_pair(name, 0));
			m_AtlasSpriteArr.push_back(atlas);
		}

		LordSafeDelete(pData);
		LordSafeFree(pMemory);
		cJSON_Delete(pJsonRoot);
		nResult = true;
	} while (0);

	if (!nResult)
	{
		if (pData)
		{
			LordSafeDelete(pData);
		}

		if (pJsonRoot)
		{
			cJSON_Delete(pJsonRoot);
		}

		if (pMemory)
		{
			LordSafeFree(pMemory);
		}
	}

	return nResult;
}


SPRITE_TYPE TextureAtlas::getTypeFromString(const String& typeStr)
{
	SPRITE_TYPE result = SPRITE_TYPE_INVALID;
	if (typeStr == "frame")
		result = SPRITE_TYPE_FRAME;
	else if (typeStr == "animation")
		result = SPRITE_TYPE_ANIMATION;
	return result;
}

int TextureAtlas::parseJsonSprites(void* pJsonSprites)
{
	int nResult = true;
	int nRetCode = false;
	cJSON* pJson = (cJSON*)pJsonSprites;

	if (!pJson)
		return false;

	int arraySize = cJSON_GetArraySize(pJson);
	m_AtlasSpriteArr.resize(arraySize);

	for (int idx = 0; idx < arraySize; ++idx)
	{
		ERROR_BREAK_CODE(m_TexturePtr, nResult);
		cJSON* pSprite = cJSON_GetArrayItem(pJson, idx);
		ERROR_BREAK_CODE(pSprite, nResult);

		String spriteName = pSprite->string;
		
		AtlasSprite atlas(SPRITE_TYPE_FRAME);
		nRetCode = atlas.parseFromJson(pSprite, m_TexturePtr);
		ERROR_BREAK_CODE(nRetCode, nResult);
				
		m_AtlasSpriteMap.insert(std::make_pair(spriteName, idx));
		m_AtlasSpriteArr[idx] = atlas;
	}
	
	return nResult;
}

AtlasSprite* TextureAtlas::getAtlasScript(const String& spriteName)
{
	int idx = getAltasScriptIdx(spriteName);
	return getAtlasScript(idx);
}

AtlasSprite* TextureAtlas::getAtlasScript(int idx)
{
	if (idx < 0)
		return NULL;
	if (idx >= int(m_AtlasSpriteArr.size()))
		return NULL;

	return &m_AtlasSpriteArr[idx];
}

int	TextureAtlas::getAltasScriptIdx(const String& spriteName)
{
	int nResult = ERROR_INDEX;

	AtlasSpriteMap::iterator it = m_AtlasSpriteMap.find(spriteName);
	if (it != m_AtlasSpriteMap.end())
		nResult = it->second;

	return nResult;
}

void TextureAtlas::getAtlasSpriteArr(GUIImageset::ImageSetUnitArr& outImages)
{
	outImages.clear();
	for (auto it : m_AtlasSpriteArr)
	{
		GUIImageset::ImageSetUnit img;
		img.name = it.getSpriteName();
		img.pos = it.getFramePos();
		img.size = it.getFrameSize();
		outImages.push_back(img);
	}
}

MemTextureAtlas::~MemTextureAtlas()
{
	release();
	LordSafeFree(m_textureBuffer);
	for (int i = 0; i < maxMip; ++i)
	{
		LordSafeFree(m_mip[i]);
		TextureManager::Instance()->releaseResource(m_mipTex[i]);
		m_mipTex[i] = nullptr;
		m_mipAtlasMap[i].clear();
		m_mipAtlasArr[i].clear();
	}
}

int MemTextureAtlas::load(const String& jsonName)
{
	int nResult = false;
	int nRetCode = false;
	DataStream* pData = NULL;
	cJSON* pJsonRoot = NULL;
	char* pMemory = NULL;

	do
	{
		pData = ResourceGroupManager::Instance()->openResource(jsonName);
		ERROR_BREAK(pData);
		
		pMemory = (char*)LordMalloc(sizeof(char) * pData->size());
		pData->read(pMemory, pData->size());

		pJsonRoot = cJSON_Parse(pMemory);
		ERROR_BREAK(pJsonRoot);

		cJSON* jsonMeta = cJSON_GetObjectItem(pJsonRoot, "meta");
		ERROR_BREAK(jsonMeta);

		cJSON* jsImage = cJSON_GetObjectItem(jsonMeta, "image");
		ERROR_BREAK(jsImage);

		String textureName = jsImage->valuestring;

		cJSON* jsType = cJSON_GetObjectItem(jsonMeta, "type");
		ERROR_BREAK(jsType);
		m_type = getTypeFromString(jsType->valuestring);
		
		cJSON* jsSize = cJSON_GetObjectItem(jsonMeta, "size");
		ERROR_BREAK(jsSize);

		cJSON* jsw = cJSON_GetObjectItem(jsSize, "w");
		ERROR_BREAK(jsw);
		m_width = jsw->valueint;

		cJSON* jsh = cJSON_GetObjectItem(jsSize, "h");
		ERROR_BREAK(jsh);
		m_height = jsh->valueint;

		int nBufferSize = m_width * m_height * sizeof(int);
		m_textureBuffer = (int*)LordMalloc(nBufferSize);
		memset(m_textureBuffer, 0, nBufferSize);
		Buffer buff(nBufferSize, m_textureBuffer);

		m_TexturePtr = TextureManager::Instance()->createManual(textureName, Texture::TT_2D, PF_RGBA8_UNORM,
			Texture::TU_STATIC, m_width, m_height, 1, 1, buff);
	

		if (m_type == SPRITE_TYPE_FRAME)
		{
			cJSON* jsSprites = cJSON_GetObjectItem(pJsonRoot, "frames");
			ERROR_BREAK(jsSprites);

			nRetCode = parseJsonSprites(jsSprites);
			ERROR_BREAK(nRetCode);
		}
		
		// update texture.
		ERect rt(0, 0, float(m_width), float(m_height));
		m_TexturePtr->updateSubTex2D(0, rt, m_textureBuffer);

		loadMip(textureName);

		LordSafeFree(m_textureBuffer);  // give memory back.
		LordSafeDelete(pData);
		LordSafeFree(pMemory);
		cJSON_Delete(pJsonRoot);
		nResult = true;
	} while (0);

	if (!nResult)
	{
		if (pData)
		{
			LordSafeDelete(pData);
		}

		if (pJsonRoot)
		{
			cJSON_Delete(pJsonRoot);
		}

		if (pMemory)
		{
			LordSafeFree(pMemory);
		}
	}

	return nResult;
}

void MemTextureAtlas::loadMip(const String& texName)
{
	LordAssert(m_textureBuffer);
	int w = m_width;
	int h = m_height;
	int s = 1;
	for (int mip = 0; mip < maxMip; ++mip)
	{
		w /= 2;
		h /= 2;
		s *= 2;

		m_mip[mip] = (ui32*)LordMalloc(w * h * sizeof(ui32));
		// copy pixel to mip, use point fillter.
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				int srcIdx = y * s * m_width + x * s;  // point fillter
				int dstIdx = y * w + x;
				m_mip[mip][dstIdx] = m_textureBuffer[srcIdx];
			}
		}
		
		// create texture from this mip
		LordAssert(m_mipTex[mip] == nullptr);
		String mipTexName = StringUtil::Format("%s_mip_%d", texName.c_str(), mip + 1);
		Buffer buff(w * h * sizeof(ui32), m_mip[mip]);
		m_mipTex[mip] = TextureManager::Instance()->createManual(mipTexName, Texture::TT_2D, PF_RGBA8_UNORM,
			Texture::TU_STATIC, w, h, 1, 1, buff);

		// update texture.
		ERect rt(0, 0, float(w), float(h));
		m_mipTex[mip]->updateSubTex2D(0, rt, m_mip[mip]);

		// create json from this mip
		LordAssert(m_mipAtlasArr[mip].empty());
		LordAssert(m_mipAtlasMap[mip].empty());
		int ssize = m_AtlasSpriteArr.size();
		m_mipAtlasArr[mip].resize(ssize);
		for (int i = 0; i < ssize; ++i)
		{
			AtlasSprite atlas(SPRITE_TYPE_FRAME);
			const AtlasSprite& rhs = m_AtlasSpriteArr[i];
			atlas.name = rhs.name;
			atlas.frame_pos = rhs.frame_pos / s;
			atlas.frame_size = rhs.frame_size / s;
			atlas.source_pos = rhs.source_pos / s;
			atlas.source_size = rhs.source_size / s;
			atlas.texture = m_mipTex[mip];
			atlas.caculate();

			m_mipAtlasArr[mip][i] = atlas;
			m_mipAtlasMap[mip].insert({atlas.name, i});
		}
	}
}

int MemTextureAtlas::getMipHeight(int mip)
{
	LordAssert(mip > 0 && mip <= maxMip);
	int result = m_height;
	for (int i = 0; i < mip; ++i)
		result /= 2;
	return result;
}

int MemTextureAtlas::getMipWidth(int mip)
{
	LordAssert(mip > 0 && mip <= maxMip);
	int result = m_width;
	for (int i = 0; i < mip; ++i)
		result /= 2;
	return result;
}

ui32* MemTextureAtlas::getPixelMemMip(int mip)
{
	LordAssert(mip > 0 && mip <= maxMip);
	return m_mip[mip - 1];
}

Texture* MemTextureAtlas::getMipTexture(int mip)
{
	LordAssert(mip > 0 && mip <= maxMip);
	return m_mipTex[mip - 1];
}

void MemTextureAtlas::getMipAtlasSpriteArr(int mip, GUIImageset::ImageSetUnitArr& outImages)
{
	LordAssert(mip > 0 && mip <= maxMip);
	outImages.clear();
	for (auto it : m_mipAtlasArr[mip - 1])
	{
		GUIImageset::ImageSetUnit img;
		img.name = it.name;
		img.pos = it.frame_pos;
		img.size = it.frame_size;
		outImages.push_back(img);
	}
}

AtlasSprite* MemTextureAtlas::getMipAtlasScript(int mip, const String& spriteName)
{
	LordAssert(mip > 0 && mip <= maxMip);
	int idx = ERROR_INDEX;

	AtlasSpriteMap::iterator it = m_mipAtlasMap[mip-1].find(spriteName);
	if (it != m_mipAtlasMap[mip-1].end())
		idx = it->second;

	if (idx == ERROR_INDEX)
		return nullptr;

	return getMipAtlasScript(mip, idx);
}

AtlasSprite* MemTextureAtlas::getMipAtlasScript(int mip, int idx)
{
	LordAssert(mip > 0 && mip <= maxMip);
	if (idx < 0)
		return NULL;
	if (idx >= int(m_mipAtlasArr[mip-1].size()))
		return NULL;

	return &m_mipAtlasArr[mip-1][idx];
}

int MemTextureAtlas::parseJsonSprites(void* pJsonSprites)
{
	int nResult = true;
	int nRetCode = false;
	cJSON* pJson = (cJSON*)pJsonSprites;

	if (!pJson)
		return false;

	int arraySize = cJSON_GetArraySize(pJson);
	m_AtlasSpriteArr.resize(arraySize);

	for (int idx = 0; idx < arraySize; ++idx)
	{
		ERROR_BREAK_CODE(m_TexturePtr, nResult);
		cJSON* pSprite = cJSON_GetArrayItem(pJson, idx);
		ERROR_BREAK_CODE(pSprite, nResult);

		String spriteName = pSprite->string;
		
		AtlasSprite atlas(SPRITE_TYPE_FRAME);
		nRetCode = copyImageToTexture(spriteName, pSprite, atlas);
		ERROR_BREAK_CODE(nRetCode, nResult);
		// add texture to texture.
		

		m_AtlasSpriteMap.insert(std::make_pair(spriteName, idx));
		m_AtlasSpriteArr[idx] = atlas;
	}

	return nResult;
}

int MemTextureAtlas::copyImageToTexture(const String& name, void* json, AtlasSprite& atlas)
{
	int nResult = false;

	DataStream* pStream = NULL;
	Byte* pData = NULL;
	Image* pImage = NULL;
	int nSize = 0;

	do
	{
		String imgname = name + ".png";
		pStream = ResourceGroupManager::Instance()->openResource(imgname);
		ERROR_BREAK(pStream);

		pData = (Byte*)(LordMalloc(pStream->size()));
		ERROR_BREAK(pData);

		nSize = pStream->size();
		pStream->read(pData, nSize);
		LordSafeDelete(pStream);

		Buffer imageBuffer(nSize, pData);
		Image* pImage = Image::CreateFromMemory(imageBuffer, Image::GetImageFormat(imgname));
		ERROR_BREAK(pImage);

		int w = pImage->getWidth();
		int h = pImage->getHeight();
		LordAssert(w == h);

		int batch = m_width / 64;

		atlas.name = name;
		if (w == 16)
		{
			if (m_16Index == -1)
			{
				m_next16Index = m_next64Index;
				m_next64Index++;
				m_16Index = 0;
			}
			atlas.frame_pos.x = (m_next16Index % batch) * 64 + (m_16Index % 4) * 16;
			atlas.frame_pos.y = (m_next16Index / batch) * 64 + (m_16Index / 4) * 16;
			m_16Index++;
			if (m_16Index == 16)
				m_16Index = -1;
		}
		else if (w == 32)
		{
			if (m_32Index == -1)
			{
				m_next32Index = m_next64Index;
				m_next64Index++;
				m_32Index = 0;
			}
			atlas.frame_pos.x = (m_next32Index % batch) * 64 + (m_32Index % 2) * 32;
			atlas.frame_pos.y = (m_next32Index / batch) * 64 + (m_32Index / 2) * 32;
			m_32Index++;
			if (m_32Index == 4)
				m_32Index = -1;
		}
		else if (w == 64)
		{
			atlas.frame_pos.x = (m_next64Index % batch) * 64;
			atlas.frame_pos.y = (m_next64Index / batch) * 64;
			m_next64Index++;
		}
		atlas.source_pos = Vector2i(0, 0);
		atlas.source_size = Vector2i(w, h);
		atlas.frame_size = Vector2i(w, h);
		atlas.texture = m_TexturePtr;
		atlas.caculate();

		int* srouceData = (int*)pImage->getData();
		Byte* sourcebData = pImage->getData();
		PixelFormat fm = pImage->getPixelFormat();
		bool hasAlpha = PixelUtil::HasAlpha(fm);
		// copy data to m_texture
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				int didx = (atlas.frame_pos.y + y) * m_width + atlas.frame_pos.x + x;
				int sidx = y * w + x;
				if (hasAlpha)
				{
					PixelUtil::BulkPixelConversion(srouceData + sidx, fm, m_textureBuffer + didx, PF_RGBA8_UNORM, 1);
				}
				else
				{
					PixelUtil::BulkPixelConversion(sourcebData + sidx * 3, fm, m_textureBuffer+didx, PF_RGBA8_UNORM, 1);
				}
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

		LordLogError("can not found the image file [%s]", name.c_str());
	}

	return nResult;
}

TextureAtlasRegister::TextureAtlasRegister()
{}

TextureAtlasRegister::~TextureAtlasRegister()
{
	for (TextureAtlasArr::iterator it = m_textureAtlasArr.begin(); it != m_textureAtlasArr.end(); ++it)
		LordSafeDelete(*it);
	m_textureAtlasArr.clear();
}

int TextureAtlasRegister::addTextureAtlas(const String& atlasname, const String& prefix)
{
	int nResult = false;
 	TextureAtlas* pAtlas = LordNew TextureAtlas();
	if (nResult = pAtlas->loadFromJson(atlasname, prefix))
	{
		m_textureAtlasArr.push_back(pAtlas);
	}
	else
		LordSafeDelete(pAtlas);
	return nResult;
}

int TextureAtlasRegister::addMemTextureAtlas(const String& atlasname, const String& folder)
{
	int nResult = false;
	MemTextureAtlas* pAtlas = LordNew MemTextureAtlas(atlasname);
	nResult = pAtlas->load(folder);
	if (nResult)
		m_textureAtlasArr.push_back(pAtlas);
	else
		LordSafeDelete(pAtlas);
	return nResult;
}

TextureAtlas* TextureAtlasRegister::getTextureAtlas(const String& atlasname)
{
	for (TextureAtlasArr::iterator it = m_textureAtlasArr.begin(); it != m_textureAtlasArr.end(); ++it)
	{
		if ((*it)->getName() == atlasname)
			return *it;
	}
	return NULL;
}

Texture* TextureAtlasRegister::getTexture(const String& texturename)
{
	for (TextureAtlasArr::iterator it = m_textureAtlasArr.begin(); it != m_textureAtlasArr.end(); ++it)
	{
		Texture* tex = (*it)->getTexture();
		if (tex && tex->getName() == texturename)
			return tex;
	}
	return NULL;
}

AtlasSprite* TextureAtlasRegister::getAtlasSprite(const String& spritename)
{
	String spritePureName = spritename;
	String spriteNamespace;
	auto pos = spritename.find_last_of(':');
	if (pos != String::npos)
	{
		spritePureName = spritename.substr(pos + 1);
		spriteNamespace = spritename.substr(0, pos);
	}
	AtlasSprite* pResult = NULL;
	for (TextureAtlasArr::iterator it = m_textureAtlasArr.begin(); it != m_textureAtlasArr.end(); ++it)
	{
		if (!spriteNamespace.empty() && (*it)->getNamespace() != spriteNamespace)
		{
			continue;
		}
		pResult = (*it)->getAtlasScript(spritePureName);
		if (pResult)
			return pResult;
	}
	return NULL;
}

AtlasSprite* TextureAtlasRegister::getAtlasSprite(const String& atlasName, const String& spriteName)
{
	AtlasSprite* pResult = NULL;
	for (TextureAtlasArr::iterator it = m_textureAtlasArr.begin(); it != m_textureAtlasArr.end(); ++it)
	{
		if ((*it)->getName() == atlasName)
		{
			pResult = (*it)->getAtlasScript(spriteName);
			if(pResult)
				return pResult;
			LordLogWarning("TextureAtlasRegister can not find the atlas location [%s]. use the missing atlas replace!", atlasName.c_str());
			pResult = (*it)->getAtlasScript("missing");
			return pResult;
		}
	}
	return NULL;
}

}
