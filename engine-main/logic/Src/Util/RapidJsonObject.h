#ifndef __RAPID_JSON_OBJECT_HEADER__
#define __RAPID_JSON_OBJECT_HEADER__

#include <utility>
#include "rapidjson/document.h"

#define WRAP_METHOD(_type)                                                     \
bool Is##_type(const char* name) const                                         \
{                                                                              \
    return m_object.HasMember(name) && m_object[name].Is##_type();             \
}                                                                              \
auto Get##_type(const char* name) const                                        \
    -> decltype(m_object[name].Get##_type())                                   \
{                                                                              \
    return m_object[name].Get##_type();                                        \
}                                                                              \
auto Get##_type(const char* name,                                              \
    decltype(m_object[name].Get##_type()) defaultValue) const                  \
    -> decltype(m_object[name].Get##_type())                                   \
{                                                                              \
    if (!Is##_type(name))                                                      \
    {                                                                          \
        return defaultValue;                                                   \
    }                                                                          \
    return m_object[name].Get##_type();                                        \
}

namespace BLOCKMAN
{
	class RapidJsonObject
	{
	private:
		const rapidjson::Value::ConstObject& m_object;

	public:
		RapidJsonObject(const rapidjson::Value::ConstObject& object)
			: m_object(object)
		{}

		WRAP_METHOD(Int);
		WRAP_METHOD(Float);
		WRAP_METHOD(String);
		WRAP_METHOD(Array);
	};
}

#undef WRAP_METHOD

#endif // !__RAPID_JSON_OBJECT_HEADER__