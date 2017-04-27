//#include "game_objects/Components/Transform.h"
//#include <glm/gtx/matrix_decompose.hpp>
//
//using namespace lcn::object;
//
//Transform::Transform(Entity* a_Owner)
//	: Transform(glm::mat4(), a_Owner)
//{}
//
//Transform::Transform(glm::mat4 a_Matrix, Entity* a_Owner)
//{
//	m_Owner = a_Owner;
//
//	m_LocalMatrix = a_Matrix;
//	glm::vec3 skew;
//	glm::vec4 perspective;
//	glm::decompose(a_Matrix, m_Scale, m_Rotation, m_Translation, skew, perspective);
//	m_Rotation = glm::conjugate(m_Rotation);
//}
//
//Transform::Transform(glm::vec3 a_Position, glm::quat a_Rotation, glm::vec3 a_Scale, Entity* a_Owner)
//	: Transform(a_Owner)
//{
//	m_Translation = a_Position;
//}
//
//glm::mat4 Transform::GetWorldMatrix()
//{
//	return m_LocalMatrix;
//}
//
//glm::mat4 Transform::GetLocalMatrix()
//{
//	return m_LocalMatrix;
//}
//
//glm::vec3 Transform::GetTranslation()
//{
//	return m_Translation;
//}
//
//glm::quat Transform::GetRotation()
//{
//	return m_Rotation;
//}
//
//glm::vec3 Transform::GetScale()
//{
//	return m_Scale;
//}
//
//Transform::~Transform()
//{
//
//}