#include "system/EntityFactory.h"
#include "game_objects/Entity.h"
#include "game_objects/Camera.h"
#include "resources/SceneResource.h"
#include "system/StateSystem.h"

#include <list>

using namespace lcn;
using namespace lcn::object;

namespace lcn::EntityFactory
{
	namespace // Data Members
	{
		std::list<Entity*> m_Entities;
		std::list<Camera*> m_Cameras;
	}

	void AddEntityToScene(lcn::object::Entity* a_Entity)
	{
		lcn::StateSystem::GetScene()->AddEntity(a_Entity);
	}

	void AddEntityToScene(lcn::object::Entity* a_Entity, lcn::resources::SceneResource* a_Scene)
	{
		a_Scene->AddEntity(a_Entity);
	}
	

	lcn::object::Entity* CreateEntity()
	{
		Entity* entity = new Entity();
		m_Entities.push_back(entity);
		AddEntityToScene(entity);
		return entity;
	}

	lcn::object::Entity* CreateEntity(lcn::resources::SceneResource* a_Scene)
	{
		Entity* entity = new Entity();
		m_Entities.push_back(entity);
		AddEntityToScene(entity, a_Scene);
		return entity;
	}

	lcn::object::Camera* CreateCamera()
	{
		Camera* camera = new Camera();
		m_Entities.push_back(camera);
		m_Cameras.push_back(camera);
		AddEntityToScene(camera);
		return camera;
	}

	lcn::object::Camera* GetMainCamera()
	{
		return m_Cameras.front();
	}
}; // namespace lcn::EntityFactory