#ifndef __TRIGGER_ACTIVATOR
#define __TRIGGER_ACTIVATOR

#include "Core.h"
#include <typeindex>

#define DEFINE_MEMBER(type_, name_)              \
public:                                          \
    TriggerActivator(type_ value##name_)         \
        : m_type(typeid(type_))                  \
        , m_##name_(value##name_)                \
    {}                                           \
    bool is##name_() const noexcept              \
    {                                            \
        return m_type == typeid(type_);          \
    }                                            \
    type_ to##name_() const noexcept             \
    {                                            \
        return m_##name_;                        \
    }                                            \
private:                                         \
    type_ m_##name_;

namespace BLOCKMAN
{
	class Entity;
	using BlockPos = LORD::Vector3i;

	class TriggerActivator : public LORD::ObjectAlloc
	{
	private:
		std::type_index m_type;

	public:
		TriggerActivator()
			: m_type(typeid(void*))
		{}
		TriggerActivator& operator=(const TriggerActivator& that) = default;

		DEFINE_MEMBER(Entity*, Entity);
		DEFINE_MEMBER(LORD::Vector3i, BlockPos);
	};

	using TriggerActivatorList = LORD::vector<TriggerActivator>::type;
}

#undef DEFINE_MEMBER

#endif // !__TRIGGER_ACTIVATOR
