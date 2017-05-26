#pragma once

namespace lcn::object
{
	class Entity;
	class Camera;
}; // namespace lcn::object

namespace lcn::resources
{
	class SceneResource;
}; // namespace lcn::resources

namespace lcn::EntityFactory
{
	void AddEntityToScene(lcn::object::Entity* a_Entity);
	void AddEntityToScene(lcn::object::Entity* a_Entity, lcn::resources::SceneResource* a_Scene);

	lcn::object::Entity* CreateEntity();
	lcn::object::Entity* CreateEntity(lcn::resources::SceneResource* a_Scene);
	lcn::object::Camera* CreateCamera();
	lcn::object::Camera* GetMainCamera();
}; // namespace lcn::EntityFactory