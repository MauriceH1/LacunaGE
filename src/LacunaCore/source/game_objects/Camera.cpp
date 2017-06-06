#include "game_objects/Camera.h"
#include "system/InputSystem.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
using namespace lcn::object;

Camera::Camera()
	: Entity()
{
	m_ProjectionMatrix = glm::perspectiveFov(glm::radians(80.f), 1280.f, 720.f, 0.001f, 1000.f);
}

Camera::~Camera()
{

}

void Camera::Update()
{
	glm::vec3 deltaP;
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_A))
	{
		deltaP += -GetRight();
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_D))
	{
		deltaP += GetRight();
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_S))
	{
		deltaP += -GetForward();
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_W))
	{
		deltaP += GetForward();
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_E))
	{
		deltaP += GetUp();
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_Q))
	{
		deltaP += -GetUp();
	}

	if (deltaP.x * deltaP.x + deltaP.y * deltaP.y + deltaP.z * deltaP.z > 0.004f)
	{
		AddPosition(deltaP * 0.5f);
	}

	glm::quat deltaR;
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_J))
	{
		deltaR *= glm::quat_cast(glm::rotate(glm::mat4(), -0.04f, GetUp()));
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_L))
	{
		deltaR *= glm::quat_cast(glm::rotate(glm::mat4(), 0.04f, GetUp()));
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_I))
	{
		deltaR *= glm::quat_cast(glm::rotate(glm::mat4(), 0.04f, GetRight()));
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_K))
	{
		deltaR *= glm::quat_cast(glm::rotate(glm::mat4(), -0.04f, GetRight()));
	}

	if (glm::abs(deltaR.x + deltaR.y + deltaR.z) > 0.001f)
	{
		AddRotation(glm::normalize(deltaR));
	}
}

const glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(m_Transform->GetTranslation(), m_Transform->GetTranslation() + GetForward(), GetUp());
}

const glm::mat4 Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}