#include "game_objects/Entity.h"

using namespace lcn::object;

Entity::Entity()
{
	m_Transform = new Transform(glm::vec3(2.0f, 5.0f, -10.0f), glm::quat(), glm::vec3(1));
}

Entity::~Entity()
{
	delete m_Transform;
	m_Transform = nullptr;
}

void Entity::Update()
{
}

void Entity::SetParent(Entity* a_NewParent)
{
	m_Parent = a_NewParent;
}

void Entity::AddComponent(Component *a_Component)
{
	a_Component->SetParent(this);
	m_Components.push_back(a_Component);
}

void Entity::AddChild(Entity* a_NewChild)
{
	a_NewChild->SetParent(this);
	m_Children.push_back(a_NewChild);
}

const glm::mat4 Entity::GetModelMatrix() const
{
	return m_Transform->GetLocalMatrix();
}

const glm::mat4 Entity::GetWorldMatrix() const
{
	return m_Transform->GetWorldMatrix();
}

const Entity* Entity::GetParent() const
{
	return m_Parent;
}

const std::vector<Entity*> Entity::GetChildren() const
{
	return m_Children;
}

const std::vector<Component*> Entity::GetComponents() const
{
	return m_Components;
}