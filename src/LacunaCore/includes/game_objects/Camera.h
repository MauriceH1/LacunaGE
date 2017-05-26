#include "game_objects/Entity.h"

namespace lcn::object
{
	class Camera : public Entity
	{
	public:
		Camera();
		~Camera();

		virtual void Update();

		const glm::mat4 GetViewMatrix() const;
		const glm::mat4 GetProjectionMatrix() const;
	
	private:

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
	};
}; // namespace lcn::object