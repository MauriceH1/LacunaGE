#include "graphics\Renderer.h"

namespace lcn {
	namespace graphics
	{
		namespace helpers
		{
			struct VkData;
		};

		class LacunaVRenderer : public Renderer
		{
		public:
			LacunaVRenderer();
			virtual ~LacunaVRenderer() override final;

			virtual bool Initialize(const lcn::platform::specifics::PlatformHandles* a_Handles) override final;
			virtual void Render() override final;
			virtual void Cleanup() override final;

		private:
			helpers::VkData* m_Data = nullptr;
		};
	};
}; // namespace lcn::graphics