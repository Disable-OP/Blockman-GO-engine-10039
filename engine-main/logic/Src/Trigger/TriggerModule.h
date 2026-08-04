#ifndef __TRIGGER_MODULE_HEADER__
#define __TRIGGER_MODULE_HEADER__

#include "Core.h"

namespace BLOCKMAN
{
	class TriggerModule : public LORD::ObjectAlloc, public LORD::Singleton<TriggerModule>
	{
	public:
		virtual ~TriggerModule() = default;
		virtual void initialize() const = 0;
		virtual void uninitialize() const = 0;
	};
}

#endif // !__TRIGGER_MODULE_HEADER__
