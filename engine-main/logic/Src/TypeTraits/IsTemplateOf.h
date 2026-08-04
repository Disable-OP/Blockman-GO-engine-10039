#ifndef __IS_TEMPLATE_OF_HEADER__
#define __IS_TEMPLATE_OF_HEADER__

#include "Std/type_traits.h"

namespace BLOCKMAN
{
	template<template<typename... Args> class Template, typename Class>
	struct is_template_of : std::false_type {};

	template<template<typename... Args> class Template, typename... Args>
	struct is_template_of<Template, Template<Args...>> : std::true_type {};
}

#endif
