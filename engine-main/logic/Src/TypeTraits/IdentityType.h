#ifndef __IDENTITY_TYPE_HEADER__
#define __IDENTITY_TYPE_HEADER__

namespace BLOCKMAN
{
	template<typename T>
	struct identity_type
	{
		using type = T;
	};

	template<typename T>
	using identity_type_t = typename identity_type<T>::type;
}

#endif
