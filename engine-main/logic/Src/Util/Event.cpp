#include "Event.h"

namespace BLOCKMAN
{
#ifdef TRACK_EVENT_SUBSCRIPTION
	vector<EventManager::EventInfo>::type EventManager::eventInfos;
#endif
}