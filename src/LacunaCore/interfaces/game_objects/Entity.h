#pragma once

#include "game_objects\Component.h"
#include "game_objects\Transform.h"

#include <vector>

namespace lcn::object
{
	class Entity
	{
	public:
		Entity();
		~Entity();

		virtual void Update();

		void SetParent(Entity* a_NewParent);

		void AddComponent(Component* a_Component);
		void AddChild(Entity* a_NewChild);

		const glm::mat4 GetModelMatrix() const;
		const glm::mat4 GetWorldMatrix() const;
		const Entity* GetParent() const;
		const std::vector<Entity*> GetChildren() const;
		const std::vector<Component*> GetComponents() const;
		template<typename T>
		const std::vector<T*> GetComponentsByType() const
		{
			std::vector<T*> comps;

			for (size_t i = 0; i < m_Components.size(); i++)
			{
				Component* comp = m_Components.at(i);
				if (typeid(*comp) == typeid(T))
				{
					comps.push_back(static_cast<T*>(comp));
				}
			}

			return comps;
		}

	protected:
		Transform* m_Transform = nullptr;
		Entity* m_Parent = nullptr;
		std::vector<Entity*> m_Children;
		std::vector<Component*> m_Components;
	};
}; // namespace lcn::objects