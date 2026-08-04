#ifndef CPR_DEFINES_H
#define CPR_DEFINES_H

#include <utility>

#if LORD_PLATFORM == LORD_PLATFORM_ANDROID

#include <sstream>

namespace std {	
	template <typename T>
	std::string to_string(T value)
	{
		std::ostringstream os ;
		os << value ;
		return os.str() ;
	}
}
#endif

#define CPR_FWD(...) ::std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

#endif
