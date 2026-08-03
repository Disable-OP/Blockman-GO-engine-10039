#ifndef __HEAD_TEXT_RENDERER_HEADER__
#define __HEAD_TEXT_RENDERER_HEADER__

#include "Core.h"

namespace BLOCKMAN
{
	class WallTextRenderer
	{
	public:
		static void renderWallText(const LORD::String& txt, LORD::Vector3 pos, bool canBeOccluded, float scale = 1.0f, float yaw = 0.0f, float pitch = 0.0f, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
	};
}

#endif // !__HEAD_TEXT_RENDERER_HEADER__