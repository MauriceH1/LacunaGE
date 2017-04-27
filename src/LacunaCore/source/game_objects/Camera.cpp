#include "game_objects\Camera.h"
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
}

const glm::mat4 Camera::GetViewMatrix() const
{
	return glm::inverse(m_Transform->GetWorldMatrix());
}