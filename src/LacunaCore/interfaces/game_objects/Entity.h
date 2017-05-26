#pragma once

#include "game_objects/Component.h"
#include "game_objects/Transform.h"

#include <vector>

namespace lcn::object
{
	class Entity
	{
	public:
		Entity();
		~Entity();

		virtual void Update();
		void UpdateTransforms();

		// Transform Functions
		const glm::vec3 GetPosition() const;
		const glm::quat GetRotation() const;
		const glm::vec3 GetScale() const;
		void SetLocalMatrix(glm::mat4x4 a_LocalMatrix);
		void SetPosition(glm::vec3 a_Position);
		void SetRotation(glm::vec3 a_Rotation);
		void SetScale(glm::vec3 a_Scale);
		void AddPosition(glm::vec3 a_Position);
		void AddRotation(glm::quat a_Rotation);
		void AddScale(glm::vec3 a_Scale);
		const glm::mat4 GetModelMatrix() const;
		const glm::mat4 GetWorldMatrix() const;

		const Entity* GetParent() const;
		void SetParent(Entity* a_NewParent);
		void Entity::AddChild(Entity* a_NewChild);
		const std::vector<Entity*>* GetChildren() const;

		void AddComponent(Component* a_Component);
		const std::vector<Component*> GetComponents() const;

		template<typename T> const std::vector<T*> GetComponentsByType() const
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

		const glm::vec3 GetForward() const;
		const glm::vec3 GetUp() const;
		const glm::vec3 GetRight() const;

		void SetTransformOutdated();

	protected:
		Transform* m_Transform = nullptr;
		Entity* m_Parent = nullptr;
		std::vector<Entity*> m_Children;
		std::vector<Component*> m_Components;
	};
}; // namespace lcn::objects