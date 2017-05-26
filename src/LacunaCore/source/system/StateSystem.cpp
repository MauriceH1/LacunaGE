#include "system/StateSystem.h"
#include "resources/SceneResource.h"

namespace lcn::StateSystem
{
	namespace // data separation
	{
		lcn::resources::SceneResource* m_ActiveScene = nullptr;
	}; // data separation

	lcn::resources::SceneResource* GetScene()
	{
		return m_ActiveScene;
	}

	void SetActiveScene(lcn::resources::SceneResource* a_Scene)
	{
		m_ActiveScene = a_Scene;
	}

}; // namespace lcn::StateSystem