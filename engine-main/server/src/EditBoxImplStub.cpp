// EditBoxImplStub.cpp — stub implementations for platform-specific EditBox
// factories. The server (libGameServer.so) doesn't use any UI, but
// GUIEditBox.cpp (part of LordCore) references these symbols. On Win32
// they're in EditBoxImplWin.cpp; on Android we stub them out.
//
// This file is compiled only when LORD_PLATFORM_ANDROID is defined.

#include "Core.h"

#if LORD_PLATFORM == LORD_PLATFORM_ANDROID

#include "UI/GUIEditBoxImpl.h"

namespace LORD
{
// Stub: create a no-op EditBoxImpl. The server never calls this (it has no
// UI), but the symbol must exist to satisfy the linker.
GUIEditBoxImpl* __createSystemEditBox(GUIEditBox* /*pEditBox*/)
{
	return nullptr;
}

// Stub: destroy the no-op EditBoxImpl.
void __destroySystemEditBox(GUIEditBoxImpl* /*pEditBoxImpl*/)
{
	// no-op
}

} // namespace LORD

#endif // LORD_PLATFORM_ANDROID
