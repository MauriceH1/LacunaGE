#include "system/EntityFactory.h"
#include "game_objects/Entity.h"
#include "game_objects/Camera.h"

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

	lcn::object::Entity* CreateEntity()
	{
		Entity* entity = new Entity();
		m_Entities.push_back(entity);
		return entity;
	}

	lcn::object::Camera* CreateCamera()
	{
		Camera* camera = new Camera();
		m_Entities.push_back(camera);
		m_Cameras.push_back(camera);
		return camera;
	}

	lcn::object::Camera* GetMainCamera()
	{
		return m_Cameras.front();
	}
}; // namespace lcn::EntityFactory