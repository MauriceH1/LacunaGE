#pragma once

#include "resources\Device.h"
#include "game_objects/Entity.h"
#include "resources/SceneResource.h"

#include <glm\vec3.hpp>
#include <glm\vec4.hpp>

#include <string>

namespace lcn::platform::specifics
{
	struct PlatformHandles;
}; // namespace lcn::platform::specifics

namespace lcn::graphics
{
	struct TVertex
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	class Renderer
	{
	public:
		Renderer() {};
		virtual ~Renderer() {};

		virtual bool Initialize(const lcn::platform::specifics::PlatformHandles* a_Handles) = 0;
		virtual void Render(lcn::resources::SceneResource* a_Scene) = 0;
		virtual void Cleanup() = 0;

		virtual const lcn::resources::Device* GetDevice() const = 0;
	};
}; // namespace lcn::graphics