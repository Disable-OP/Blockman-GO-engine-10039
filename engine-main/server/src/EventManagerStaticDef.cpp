// EventManagerStaticDef.cpp — definition of EventManager::eventInfos
// static member that's missing from the link.
//
// The declaration is in logic/Src/Util/Event.h (static member), and the
// definition is in logic/Src/Util/Event.cpp. But when linking the server
// (libGameServer.so), the linker reports 'undefined reference to
// BLOCKMAN::EVENT::EventManager::eventInfos'. This suggests the symbol
// isn't being exported from libLogic.a properly (possibly a name mangling
// or visibility issue with gnustl_static).
//
// As a fallback, we provide the definition here so the linker can find it.

#include "Core.h"
#include "Util/Event.h"

namespace BLOCKMAN { namespace EVENT {

vector<EventManager::EventInfo>::type EventManager::eventInfos;

}} // namespace BLOCKMAN::EVENT
