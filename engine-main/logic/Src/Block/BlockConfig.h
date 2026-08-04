#ifndef __BLOCK_CONFIG_HEADER__
#define __BLOCK_CONFIG_HEADER__

#include "Core.h"

namespace BLOCKMAN
{
	enum class BlockState
	{
		SOLID,
		GAS
	};

	class Trigger;
	using TriggerPtr = std::shared_ptr<Trigger>;
	using TriggerList = LORD::vector<TriggerPtr>::type;

	struct BlockConfig
	{
		struct TextureConfig
		{
			LORD::String top;
			LORD::String bottom;
			LORD::String left;
			LORD::String right;
			LORD::String front;
			LORD::String back;
		} texture;
		LORD::Color color = LORD::Color::WHITE;
		BlockState state = BlockState::SOLID;
		bool transparent = false;
		LORD::Box collisionBox = { {0,0,0},{1,1,1} };
		int resistance = 15;
		bool flammable = false;
		TriggerList triggers;

		static bool readJson(BlockConfig& output, const LORD::String& configPath);

	private:
		static bool validateTextureConfig(const TextureConfig& config);
	};
}

#endif // !__BLOCK_CONFIG_HEADER__
