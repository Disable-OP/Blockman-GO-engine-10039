#include "CustomBlockClient.h"
#include "Render/TextureAtlas.h"
#include "World/IBlockAccess.h"

using namespace LORD;

namespace BLOCKMAN
{
	CustomBlockClient::CustomBlockClient(int id, const String & nameSpace, const String & name, const BlockConfig & config)
		: CustomBlock(id, nameSpace, name, config)
		, cBlock("")
	{
		auto components = StringUtil::Split(config.texture.top, ":");
		switch (components.size())
		{
		case 1: // "my_texture"
			m_atlasName = "plugin/" + m_namespace + "/block/" + name + "/texture.json";
			break;
		case 3: // "block:my_block:my_texture"
			m_atlasName = "plugin/" + m_namespace + "/block/" + components[1] + "/texture.json";
			break;
		case 5: // "plugin:my_plugin:block:my_block:my_texture"
			m_atlasName = "plugin/" + components[1] + "/block/" + components[3] + "/texture.json";
			break;
		}

		setTextures(config);
		m_transparent = config.transparent || config.state == BlockState::GAS;
		m_color = config.color;
	}

	void CustomBlockClient::setTextures(const BlockConfig & config)
	{
		m_texture.top = getSprite(config.texture.top);
		m_texture.bottom = getSprite(config.texture.bottom);
		m_texture.left = getSprite(config.texture.left);
		m_texture.right = getSprite(config.texture.right);
		m_texture.front = getSprite(config.texture.front);
		m_texture.back = getSprite(config.texture.back);
		m_sprite = m_texture.top;
	}

	AtlasSprite * CustomBlockClient::getIcon(int side, int meta)
	{
		switch (side)
		{
		case BM_FACE_DOWN:
			return m_texture.bottom;
		case BM_FACE_UP:
			return m_texture.top;
		case BM_FACE_SOUTH:
			return m_texture.front;
		case BM_FACE_NORTH:
			return m_texture.back;
		case BM_FACE_EAST:
			return m_texture.right;
		case BM_FACE_WEST:
			return m_texture.left;
		default:
			LordLogError("icon of custom block of invalid side is queried, side: %d", side);
			return nullptr;
		}
	}

	AtlasSprite* CustomBlockClient::getSprite(const String& textureString)
	{
		if (StringUtil::StartWith(textureString, "blockymods:"))
		{
			return TextureAtlasRegister::Instance()->getAtlasSprite(textureString.substr(11));
		}

		auto components = StringUtil::Split(textureString, ":");
		switch (components.size())
		{
		case 1:
			return TextureAtlasRegister::Instance()->getAtlasSprite(
				"plugin:" + m_namespace + ":block:" + m_blockName + ":" + textureString);
		case 3:
			return TextureAtlasRegister::Instance()->getAtlasSprite("plugin:" + m_namespace + ":" + textureString);
		case 5:
			return TextureAtlasRegister::Instance()->getAtlasSprite(textureString);
		default:
			LordLogError("BlockConfig used invalid texture name: %s", textureString.c_str());
			return nullptr;
		}
	}

	int CustomBlockClient::getRenderBlockPass()
	{
		if (m_transparent || m_state == BlockState::GAS)
		{
			return TRP_TRANSPARENT_2D;
		}
		else
		{
			return TRP_SOLID;
		}
	}

	bool CustomBlockClient::shouldSideBeRendered(IBlockAccess * pBlockAccess, const BlockPos & pos, int side)
	{
		if (m_state == BlockState::GAS)
		{
			return pBlockAccess->getBlockId(pos) != m_blockID && Block::shouldSideBeRendered(pBlockAccess, pos, side);
		}
		else
		{
			return Block::shouldSideBeRendered(pBlockAccess, pos, side);
		}
	}

	ui32 CustomBlockClient::colorMultiplier(IBlockAccess * piBlockAccess, const BlockPos & pos)
	{
		return m_color.getABGR();
	}
}