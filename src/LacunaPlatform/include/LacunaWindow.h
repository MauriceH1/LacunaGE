#pragma once

#include <glm/vec2.hpp>
#include <string>

namespace lcn { namespace platform
{
	namespace specifics
	{
		struct PlatformHandles;
	}; // namespace specifics

	struct LacunaWindowOptions
	{
		glm::ivec2 size = glm::ivec2(1280, 720);
		std::string name = std::string("Lacuna Window");
		bool borderless = false;
		bool fullscreen = false;

		void(*KeyUpFunction)(__int64 a_KeyCode) = nullptr;
		void(*KeyDownFunction)(__int64 a_KeyCode) = nullptr;

	};

	class LacunaWindow
	{
	public:
		LacunaWindow(const LacunaWindowOptions& a_Options);

		const glm::ivec2& GetSize() const;

		virtual const int32_t HandleMessages() = 0;

		virtual const specifics::PlatformHandles* GetPlatformHandles() const = 0;

	protected:
		virtual const int32_t Initialize() = 0;
		void(*KeyUpFunc)(__int64 a_KeyCode);
		void(*KeyDownFunc)(__int64 a_KeyCode);

		bool m_Borderless;
		bool m_Fullscreen;

		std::string m_Name;
		glm::ivec2 m_Size;
	};
};}; // namespace lcn::platform