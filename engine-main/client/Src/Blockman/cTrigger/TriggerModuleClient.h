#ifndef __TRIGGER_MODULE_CLIENT__
#define __TRIGGER_MODULE_CLIENT__

#include "Trigger/TriggerModule.h"

namespace BLOCKMAN
{
	class TriggerModuleClient : public TriggerModule
	{
	public:
		virtual void initialize() const override;
		virtual void uninitialize() const override;
	};
}

#endif // !__TRIGGER_MODULE_CLIENT__
