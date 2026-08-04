#ifndef __HEAD_TEXT_RENDERER_HEADER__
#define __HEAD_TEXT_RENDERER_HEADER__

#include "Core.h"

namespace BLOCKMAN
{
	class HeadTextRenderer
	{
	public:
		static void renderHeadText(const LORD::String& txt, LORD::Vector3 pos, bool canBeOccluded);
		static void renderHeadText(const LORD::String& txt, LORD::Vector3 pos, bool canBeOccluded, float scale);
	};
}

#endif // !__HEAD_TEXT_RENDERER_HEADER__