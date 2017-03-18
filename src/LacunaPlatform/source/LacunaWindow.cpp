#include "LacunaWindow.h"
#include <iostream>

using namespace lcn::platform;

LacunaWindow::LacunaWindow(const LacunaWindowOptions& a_Options)
{
    m_Size = a_Options.size;
    m_Name = a_Options.name;
    m_Borderless = a_Options.borderless;
    m_Fullscreen = a_Options.fullscreen;
}

const glm::ivec2& LacunaWindow::GetSize() const
{
    return m_Size;
}