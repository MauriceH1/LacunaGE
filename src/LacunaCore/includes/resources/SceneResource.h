#pragma once

#include "system/EntityFactory.h"
#include "game_objects/Entity.h"
#include <vector>

namespace lcn::resources
{
	class SceneResource
	{
	public:
		SceneResource();
		~SceneResource();

		void Update();

		const std::vector<lcn::object::Entity*>* GetEntities() const;
		
		void AddChild(lcn::object::Entity* a_Entity);
		std::vector<lcn::object::Entity*>* GetChildren();

	private:
		void AddEntity(lcn::object::Entity* a_Entity);
		friend void lcn::EntityFactory::AddEntityToScene(lcn::object::Entity* a_Entity);
		friend void lcn::EntityFactory::AddEntityToScene(lcn::object::Entity* a_Entity, lcn::resources::SceneResource* a_Scene);

		std::vector<lcn::object::Entity*> m_Entities;
		std::vector<lcn::object::Entity*> m_Children;
	};
}; // namespace lcn::resources