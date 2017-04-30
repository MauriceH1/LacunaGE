#include "game_objects/Entity.h"

using namespace lcn::object;

Entity::Entity()
{
	m_Transform = new Transform(glm::vec3(0.0f), glm::quat(), glm::vec3(1));
}

Entity::~Entity()
{
	delete m_Transform;
	m_Transform = nullptr;
}

void Entity::Update()
{
}

const glm::vec3 Entity::GetPosition() const
{
	return m_Transform->GetTranslation();
}

const glm::quat Entity::GetRotation() const
{
	return m_Transform->GetRotation();
}

const glm::vec3 Entity::GetScale() const
{
	return m_Transform->GetScale();
}

void Entity::SetPosition(glm::vec3 a_Position)
{
	m_Transform->SetTranslation(a_Position);
}

void Entity::SetRotation(glm::vec3 a_Rotation)
{
	m_Transform->SetRotation(a_Rotation);
}

void Entity::SetScale(glm::vec3 a_Scale)
{
	m_Transform->SetScale(a_Scale);
}

void Entity::AddPosition(glm::vec3 a_Position)
{
	m_Transform->AddTranslation(a_Position);
}

void Entity::AddRotation(glm::quat a_Rotation)
{
	m_Transform->AddRotation(a_Rotation);
}

void Entity::AddScale(glm::vec3 a_Scale)
{
	m_Transform->AddScale(a_Scale);
}

const glm::mat4 Entity::GetModelMatrix() const
{
	return m_Transform->GetLocalMatrix();
}

const glm::mat4 Entity::GetWorldMatrix() const
{
	return m_Transform->GetWorldMatrix();
}

const glm::vec3 Entity::GetForward() const
{
	return m_Transform->GetForward();
}

void Entity::SetParent(Entity* a_NewParent)
{
	m_Parent = a_NewParent;
}
const Entity* Entity::GetParent() const
{
	return m_Parent;
}

void Entity::AddChild(Entity* a_NewChild)
{
	a_NewChild->SetParent(this);
	m_Children.push_back(a_NewChild);
}

const std::vector<Entity*> Entity::GetChildren() const
{
	return m_Children;
}

void Entity::AddComponent(Component *a_Component)
{
	a_Component->SetParent(this);
	m_Components.push_back(a_Component);
}

const std::vector<Component*> Entity::GetComponents() const
{
	return m_Components;
}