#include "LacunaPlatform.h"
#include "Windows\WIN32_Window.h"

using namespace lcn::platform;

LacunaWindow* lcn::platform::CreateWindow(const LacunaWindowOptions& a_Options)
{
	return static_cast<LacunaWindow*>(new specifics::WIN32_Window(a_Options));
}