#ifndef __TRIGGER_MODULE_SERVER__
#define __TRIGGER_MODULE_SERVER__

#include "Trigger/TriggerModule.h"

namespace BLOCKMAN
{
	class TriggerModuleServer : public TriggerModule
	{
	public:
		virtual void initialize() const override;
		virtual void uninitialize() const override;
	};
}

#endif // !__TRIGGER_MODULE_CLIENT__
