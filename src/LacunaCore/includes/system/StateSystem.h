#pragma once

namespace lcn::resources
{
	class SceneResource;
}; // namespace lcn::resources

namespace lcn::StateSystem
{
	lcn::resources::SceneResource* GetScene();
	void SetActiveScene(lcn::resources::SceneResource* a_Scene);
}; // namespace lcn::StateSystem