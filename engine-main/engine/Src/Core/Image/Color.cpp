#include "Core.h"
#include "Color.h"

namespace LORD
{
	// predefined specific vectors
	const Color Color::INVALID(Math::MAX_REAL, Math::MAX_REAL, Math::MAX_REAL, Math::MAX_REAL);
	const Color Color::TRANS(0, 0, 0, 0);
	const Color Color::RED(1, 0, 0, 1);
	const Color Color::GREEN(0, 1, 0, 1);
	const Color Color::BLUE(0, 0, 1, 1);
	const Color Color::BLACK(0, 0, 0, 1);
	const Color Color::WHITE(1, 1, 1, 1);
	const Color Color::YELLOW(1, 1, 0, 1);
	const Color Color::CYAN(0, 1, 1, 1);
	const Color Color::PURPLE(1, 0, 1, 1);
	const Color Color::GRAY(1, 0.62745F, 0.62745F, 0.62745F);
}