#include "game_objects/Transform.h"
#include "game_objects/Entity.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>

using namespace lcn::object;

lcn::object::Transform::Transform(Entity* a_Owner)
	: Transform(glm::mat4(), a_Owner)
{}

lcn::object::Transform::Transform(glm::mat4 a_Matrix, Entity* a_Owner)
{
	m_Owner = a_Owner;

	m_LocalMatrix = a_Matrix;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(a_Matrix, m_Scale, m_Rotation, m_Translation, skew, perspective);
	m_Rotation = glm::conjugate(m_Rotation);

	m_Forward = glm::vec3(0.0f, 0.0f, -1.0f) * m_Rotation;
	m_Up = glm::vec3(0.0f, 1.0f, 0.0f) * m_Rotation;
	m_Right = glm::vec3(1.0f, 0.0f, 0.0f) * m_Rotation;
}

lcn::object::Transform::Transform(glm::vec3 a_Position, glm::quat a_Rotation, glm::vec3 a_Scale, Entity* a_Owner)
	: Transform(a_Owner)
{
	m_Owner = a_Owner;

	m_Translation = a_Position;
	m_Rotation = a_Rotation;
	m_Scale = a_Scale;

	m_Forward = glm::vec3(0.0f, 0.0f, -1.0f) * m_Rotation;
	m_Up = glm::vec3(0.0f, 1.0f, 0.0f) * m_Rotation;
	m_Right = glm::vec3(1.0f, 0.0f, 0.0f) * m_Rotation;

	m_LocalMatrix = glm::scale(glm::mat4(), m_Scale);
	m_LocalMatrix *= glm::mat4_cast(m_Rotation);
	m_LocalMatrix = glm::translate(m_LocalMatrix, m_Translation);
}

lcn::object::Transform::~Transform()
{
}

void lcn::object::Transform::Update()
{
	if (!m_Uptodate)
	{
		m_Uptodate = true;
		m_LocalMatrix = glm::scale(glm::mat4(), m_Scale);
		m_LocalMatrix *= glm::mat4_cast(m_Rotation);
		m_LocalMatrix = glm::translate(m_LocalMatrix, m_Translation);

		m_Forward = glm::vec3(0.0f, 0.0f, -1.0f) * m_Rotation;
		m_Up = glm::vec3(0.0f, 1.0f, 0.0f) * m_Rotation;
		m_Right = glm::vec3(1.0f, 0.0f, 0.0f) * m_Rotation;

		if (m_Owner && m_Owner->GetParent())
		{
			m_WorldMatrix = m_LocalMatrix * glm::inverse(m_Owner->GetParent()->GetWorldMatrix());
		}
		else
		{
			m_WorldMatrix = m_LocalMatrix;
		}
	}
}

void lcn::object::Transform::SetLocalMatrix(glm::mat4x4 a_LocalMatrix)
{
	m_Uptodate = false;
	m_LocalMatrix = a_LocalMatrix;
}

void lcn::object::Transform::SetTranslation(glm::vec3 a_Translation)
{
	m_Uptodate = false;
	m_Translation = a_Translation;
}

void lcn::object::Transform::SetRotation(glm::quat a_Rotation)
{
	m_Uptodate = false;
	
	m_Forward = glm::vec3(0.0f, 0.0f, -1.0f) * a_Rotation;
	m_Up = glm::vec3(0.0f, 1.0f, 0.0f) * a_Rotation;
	m_Right = glm::vec3(1.0f, 0.0f, 0.0f) * a_Rotation;

	m_Rotation = a_Rotation;
}

void lcn::object::Transform::SetScale(glm::vec3 a_Scale)
{
	m_Uptodate = false;
	m_Scale = a_Scale;
}

void lcn::object::Transform::AddTranslation(glm::vec3 a_DeltaTranslation)
{
	m_Uptodate = false;
	m_Translation += a_DeltaTranslation;
}

void lcn::object::Transform::AddRotation(glm::quat a_DeltaRotation)
{
	m_Uptodate = false;
	m_Rotation *= a_DeltaRotation;

	m_Forward = glm::vec3(0.0f, 0.0f, -1.0f) * m_Rotation;
	m_Up = glm::vec3(0.0f, 1.0f, 0.0f) * m_Rotation;
	m_Right = glm::vec3(1.0f, 0.0f, 0.0f) * m_Rotation;
}

void lcn::object::Transform::AddScale(glm::vec3 a_DeltaScale)
{
	m_Uptodate = false;
	m_Scale += a_DeltaScale;
}

glm::mat4 lcn::object::Transform::GetWorldMatrix()
{
	return m_WorldMatrix;
}

glm::mat4 lcn::object::Transform::GetLocalMatrix()
{
	return m_LocalMatrix;
}

glm::vec3 lcn::object::Transform::GetTranslation()
{
	return m_Translation;
}

glm::quat lcn::object::Transform::GetRotation()
{
	return m_Rotation;
}

glm::vec3 lcn::object::Transform::GetScale()
{
	return m_Scale;
}

glm::vec3 lcn::object::Transform::GetForward()
{
	return m_Forward;
}

glm::vec3 lcn::object::Transform::GetUp()
{
	return m_Up;
}

glm::vec3 lcn::object::Transform::GetRight()
{
	return m_Right;
}

void lcn::object::Transform::SetOutdated()
{
	m_Uptodate = false;

	const std::vector<lcn::object::Entity*>* children = m_Owner->GetChildren();
	for (size_t i = 0; i < m_Owner->GetChildren()->size(); i++)
	{
		children->at(i)->SetTransformOutdated();
	}
}