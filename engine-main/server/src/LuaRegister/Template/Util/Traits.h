#pragma once
template <typename T> struct TypeDispatch {};

// 编译期间的if，如果C是true就是A类型，false就是B类型
template<bool C, typename A, typename B> struct if_ {};
template<typename A, typename B>        struct if_<true, A, B> { typedef A type; };
template<typename A, typename B>        struct if_<false, A, B> { typedef B type; };

// 获取基本类型 指针以及引用
template<typename A>
struct PureType { typedef A type; };
template<typename A>
struct PureType<A*> { typedef A type; };
template<typename A>
struct PureType<A&> { typedef A type; };

//获取 移除了引用跟const 的类型 用作 paramtranslate的类型判断

template<typename A>
struct ParamType { typedef typename std::remove_const<typename std::remove_reference<A>::type>::type type; };

template<typename T>
struct CastVar
{
	template<typename ...Args>
	static T CreateVar(Args ... args)
	{
		return T(std::forward<Args>(args)...);
	}
	static T GetVar(T t)
	{
		return t;
	}
};

template<typename T>
struct CastVar<T*>
{
	template<typename ...Args>
	static T* CreateVar(Args ... args)
	{
		return new T(std::forward<Args>(args)...);
	}
	static T GetVar(T* t)
	{
		return *t;
	}
};

template<typename T>
struct CastVar<T&>
{
	template<typename ...Args>
	static T& CreateVar(Args ... args)
	{
		return T(std::forward<Args>(args)...);// 这怎么处理？
	}
	static T GetVar(T& t)
	{
		return t;
	}
};

template<typename T>
struct CastVar<std::shared_ptr<T>>
{
	template<typename ...Args>
	static std::shared_ptr<T> CreateVar(Args ... args)
	{
		return new T(std::forward<Args>(args)...);// 这怎么处理？
	}
	static T GetVar(std::shared_ptr<T> t)
	{
		return *(t.get());
	}
};

