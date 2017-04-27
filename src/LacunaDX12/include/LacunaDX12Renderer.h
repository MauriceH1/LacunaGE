#pragma once

#include "graphics\Renderer.h"

namespace lcn::graphics
{
	namespace helpers { struct DX12Data; }; // namespace helpers
	class LacunaDX12Renderer : public Renderer
	{
	public:
		LacunaDX12Renderer();
		virtual ~LacunaDX12Renderer() override final;

		virtual bool Initialize(const lcn::platform::specifics::PlatformHandles* a_Handles) override final;
		virtual void Render(lcn::object::Entity* a_RootEntity) override final;
		void OldRender();
		virtual void Cleanup() override final;

		virtual const lcn::resources::Device* GetDevice() const;

	private:
		void WaitForPreviousFrame();
		void PopulateCommandList();
		void PrepareData();

		struct helpers::DX12Data* m_Data;

		// Test Functionality
		void CreateObjects();

		std::wstring m_Path;
	};
}; // namespace lcn::graphics