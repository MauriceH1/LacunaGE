#include "game_objects\Transform.h"
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
}

lcn::object::Transform::Transform(glm::vec3 a_Position, glm::quat a_Rotation, glm::vec3 a_Scale, Entity* a_Owner)
	: Transform(a_Owner)
{
	m_Translation = a_Position;
	m_Rotation = a_Rotation;
	m_Scale = a_Scale;

	m_LocalMatrix = glm::scale(m_LocalMatrix, m_Scale);
	m_LocalMatrix = glm::mat4_cast(m_Rotation);
	m_LocalMatrix = glm::translate(glm::mat4(), m_Translation);
}

lcn::object::Transform::~Transform()
{
}

void lcn::object::Transform::Update()
{

}

void lcn::object::Transform::SetTranslation(glm::vec3 a_Translation)
{
	m_Translation = a_Translation;
}

void lcn::object::Transform::SetRotation(glm::quat a_Rotation)
{
	m_Rotation = a_Rotation;
}

void lcn::object::Transform::SetScale(glm::vec3 a_Scale)
{
	m_Scale = a_Scale;
}

void lcn::object::Transform::AddTranslation(glm::vec3 a_DeltaTranslation)
{
	m_Translation += a_DeltaTranslation;
}

void lcn::object::Transform::AddRotation(glm::quat a_DeltaRotation)
{
	m_Rotation += a_DeltaRotation;
}

void lcn::object::Transform::AddScale(glm::vec3 a_DeltaScale)
{
	m_Scale += a_DeltaScale;
}

glm::mat4 lcn::object::Transform::GetWorldMatrix()
{
	return m_LocalMatrix;
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