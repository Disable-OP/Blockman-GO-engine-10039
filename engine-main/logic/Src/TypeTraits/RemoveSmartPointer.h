#ifndef __REMOVE_SMART_POINTER_HEADER__
#define __REMOVE_SMART_POINTER_HEADER__

#include <memory>
#include "TypeTraits/IdentityType.h"

namespace BLOCKMAN
{
	template<typename T>
	struct remove_smart_pointer : identity_type<T> {};

	template<typename T>
	struct remove_smart_pointer<std::shared_ptr<T>> : identity_type<T> {};
	template<typename T>
	struct remove_smart_pointer<const std::shared_ptr<T>> : identity_type<T> {};
	template<typename T>
	struct remove_smart_pointer<volatile std::shared_ptr<T>> : identity_type<T> {};
	template<typename T>
	struct remove_smart_pointer<const volatile std::shared_ptr<T>> : identity_type<T> {};

	template<typename T>
	struct remove_smart_pointer<std::unique_ptr<T>> : identity_type<T> {};
	template<typename T>
	struct remove_smart_pointer<const std::unique_ptr<T>> : identity_type<T> {};
	template<typename T>
	struct remove_smart_pointer<volatile std::unique_ptr<T>> : identity_type<T> {};
	template<typename T>
	struct remove_smart_pointer<const volatile std::unique_ptr<T>> : identity_type<T> {};

	template<typename T>
	using remove_smart_pointer_t = typename remove_smart_pointer<T>::type;
}

#endif