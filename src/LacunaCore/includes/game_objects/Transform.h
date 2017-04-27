#include <glm\mat4x4.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\vec3.hpp>

namespace lcn::object
{
	class Entity;
	class Transform
	{
	public:
		Transform(Entity* a_Owner = nullptr);
		Transform(glm::mat4 a_Matrix, Entity* a_Owner = nullptr);
		Transform(glm::vec3 a_Position, glm::quat a_Rotation, glm::vec3 a_Scale, Entity* a_Owner = nullptr);
		~Transform();
	
		void Update();

		void SetTranslation(glm::vec3 a_Translation);
		void SetRotation(glm::quat a_Rotation);
		void SetScale(glm::vec3 a_Scale);

		void AddTranslation(glm::vec3 a_DeltaTranslation);
		void AddRotation(glm::quat a_DeltaRotation);
		void AddScale(glm::vec3 a_DeltaScale);

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
}; // namespace lcn::object