#pragma once

namespace lcn::object
{
	class Entity;
	class Camera;
}

namespace lcn::EntityFactory
{
	lcn::object::Entity* CreateEntity();
	lcn::object::Camera* CreateCamera();

	lcn::object::Camera* GetMainCamera();
}; // namespace lcn::EntityFactory