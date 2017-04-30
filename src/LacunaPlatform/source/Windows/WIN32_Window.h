#include "LacunaWindow.h"

namespace lcn::platform::specifics
{
    class WIN32_Window : public LacunaWindow
    {
    public:
        WIN32_Window(const LacunaWindowOptions& a_Options);

        virtual const int32_t HandleMessages() override final;
        virtual const specifics::PlatformHandles* GetPlatformHandles() const override final { return m_PlatformHandles; };

    protected:
        virtual const int32_t Initialize() override final;
        specifics::PlatformHandles* m_PlatformHandles = nullptr;

    };
}; // namespace lcn::platform::specifics