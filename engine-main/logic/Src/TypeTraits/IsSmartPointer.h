#ifndef __IS_SMART_POINTER_HEADER__
#define  __IS_SMART_POINTER_HEADER__

#include <memory>
#include "Std/type_traits.h"

namespace BLOCKMAN
{
	template<typename T>
	struct is_smart_pointer : std::false_type {};

	template<typename T>
	struct is_smart_pointer<std::shared_ptr<T>> : std::true_type {};
	template<typename T>
	struct is_smart_pointer<const std::shared_ptr<T>> : std::true_type {};
	template<typename T>
	struct is_smart_pointer<volatile std::shared_ptr<T>> : std::true_type {};
	template<typename T>
	struct is_smart_pointer<const volatile std::shared_ptr<T>> : std::true_type {};

	template<typename T>
	struct is_smart_pointer<std::unique_ptr<T>> : std::true_type {};
	template<typename T>
	struct is_smart_pointer<const std::unique_ptr<T>> : std::true_type {};
	template<typename T>
	struct is_smart_pointer<volatile std::unique_ptr<T>> : std::true_type {};
	template<typename T>
	struct is_smart_pointer<const volatile std::unique_ptr<T>> : std::true_type {};

}

#endif // __IS_SMART_POINTER_HEADER__
