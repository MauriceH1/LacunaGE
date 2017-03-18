#pragma once

#include "graphics\Renderer.h"

namespace lcn { namespace graphics
{
	namespace helpers { struct DX12Data; }; // namespace helpers
	class LacunaDX12Renderer : public Renderer
	{
	public:
		LacunaDX12Renderer();
		LacunaDX12Renderer(std::wstring path);
		virtual ~LacunaDX12Renderer() override final;

		virtual bool Initialize(const lcn::platform::specifics::PlatformHandles* a_Handles) override final;
		virtual void Render() override final;
		virtual void Cleanup() override final;

		virtual const lcn::resources::Device* GetDevice() const;

	private:
		void WaitForPreviousFrame();
		void PopulateCommandList();

		struct helpers::DX12Data* m_Data;

		// Test Functionality
		void UploadCube();

		std::wstring m_Path;
	};
};}; // namespace lcn::graphics