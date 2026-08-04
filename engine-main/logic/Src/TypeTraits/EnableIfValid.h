#ifndef __ENABLE_IF_VALID_HEADER__
#define __ENABLE_IF_VALID_HEADER__

#include "Std/type_traits.h"

#define ENABLE_IF_VALID(expr, type) std::enable_if_t<!std::is_void<decltype(expr)>::value, type>

#endif // !__EXPRESSION_VALID_HEADER__

