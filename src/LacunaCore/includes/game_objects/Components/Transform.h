#include <glm\mat4x4.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\vec3.hpp>

namespace lcn { namespace object
{
	class Entity;
	class Transform
	{
	public:
		Transform(Entity* a_Owner = nullptr);
		Transform(glm::mat4 a_Matrix, Entity* a_Owner = nullptr);
		Transform(glm::vec3 a_Position, glm::quat a_Rotation, glm::vec3 a_Scale, Entity* a_Owner = nullptr);
		~Transform();
	
		glm::mat4x4 GetWorldMatrix();
		glm::mat4x4 GetLocalMatrix();
		glm::vec3 GetTranslation();
		glm::quat GetRotation();
		glm::vec3 GetScale();
	
	private:
		Entity* m_Owner = nullptr;
		Transform* m_Parent = nullptr;

		glm::vec3 m_Scale;
		glm::quat m_Rotation;
		glm::vec3 m_Translation;
		glm::mat4 m_LocalMatrix;
	};
};};