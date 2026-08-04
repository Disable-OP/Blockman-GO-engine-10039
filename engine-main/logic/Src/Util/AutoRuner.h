#pragma once
#include <memory>
#include <functional>

struct AutoRunHelp
{
	AutoRunHelp(std::function<void(void)> fuc) { fuc(); }
};

template<typename T>
struct AutoRuner
{
	static AutoRunHelp obj;
};



#define  AUTO_RUN(T, F)                    \
         struct T{};                       \
         AutoRuner<T>;                     \
         AutoRunHelp AutoRuner<T>::obj(F);