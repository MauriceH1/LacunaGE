#include "game_objects/Camera.h"
#include "system/InputSystem.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace lcn::object;

Camera::Camera()
	: Entity()
{
	m_ProjectionMatrix = glm::perspectiveFovRH<float>(glm::radians(80.f), 1280.f, 720.f, 0.001f, 1000.f);

	//m_Transform->
}

Camera::~Camera()
{

}

void Camera::Update()
{
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_A))
	{
		AddPosition(glm::vec3(-0.1f, 0.0f, 0.0f));
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_D))
	{
		AddPosition(glm::vec3(0.1f, 0.0f, 0.0f));
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_S))
	{
		AddPosition(glm::vec3(0.0f, 0.0f, 0.1f));
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_W))
	{
		AddPosition(glm::vec3(0.0f, 0.0f, -0.1f));
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_E))
	{
		AddPosition(glm::vec3(0.0f, 0.1f, 0.0f));
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_Q))
	{
		AddPosition(glm::vec3(0.0f, -0.1f, 0.0f));
	}

	if (lcn::InputSystem::IsKeyDown(LCN_KEY_J))
	{
		m_Transform->SetRotation(glm::mat4_cast(glm::rotate(m_Transform->GetRotation(), -0.1f, glm::vec3(0.0f, 1.0f, 0.0f))));
	}
	if (lcn::InputSystem::IsKeyDown(LCN_KEY_L))
	{
		m_Transform->SetRotation(glm::mat4_cast(glm::rotate(m_Transform->GetRotation(), 0.1f, glm::vec3(0.0f, 1.0f, 0.0f))));
	}
}

const glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(m_Transform->GetTranslation(), m_Transform->GetTranslation() + GetForward(), glm::vec3(0.0f, 1.0f, 0.0f));
}

const glm::mat4 Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}