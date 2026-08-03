
#ifndef __NETWORK_DEFINE_H__
#define __NETWORK_DEFINE_H__

#include "TypeDef.h"
#include "Math/IntMath.h"
#include "Network/protocol/PacketDefine.h"

using namespace LORD;

namespace NETWORK_DEFINE {
	class S2CPacketUpdateBlockInfo{
	public:
		i16 x, y, z;
		ui16 blockMetaAndId;

		S2CPacketUpdateBlockInfo() {}
		S2CPacketUpdateBlockInfo(const Vector3i& pos, int blockId, int blockMeta = 0)
			: x((i16)pos.x)
			, y((i16)pos.y)
			, z((i16)pos.z)
			, blockMetaAndId(blockMeta << 12 | blockId & 0xFFF)
		{
		}
		DEFINE_NETWORK_SERIALIZER(x, y, z, blockMetaAndId);
		ui16 getBlockId() const { return blockMetaAndId & 0xFFF; }
		ui8 getBlockMeta() const { return (blockMetaAndId >> 12) & 15; }
	};

	enum PacketPlayerActionType : ui8 {
		START_SNEAK,
		STOP_SNEAK,
		START_PULL_BOW,
		STOP_PULL_BOW,
		START_SPRINTING,
		STOP_SPRINTING,
		START_ON_FIRE,
		STOP_ON_FIRE
	};

	enum C2SPacketUseItemUseType : ui8 {
		ITEM_USE_TYPE_EASTABLE,
		ITEM_USE_TYPE_BUCKET,
		ITEM_USE_TYPE_COMMON,
	};

	enum PacketAnimateType : ui8 {
		ANIMATE_TYPE_ATTACK,
		ANIMATE_TYPE_BASE_ACTION,
		ANIMATE_TYPE_UPPER_ACTION
	};

	enum class PacketLoginResult
	{
		emSucc = 0,
		emErrTokenWrong = 1,
		emErrBeKicked = 2,
		emErrGetUserAttrFail = 3,
		emErrWaitRoleAttrFail = 4
	};
}

namespace C2SPACKET_DETAIL
{
	using Vector3 = std::array<real32, 3>;
	using Vector3i = std::array<i32, 3>;
}

#endif