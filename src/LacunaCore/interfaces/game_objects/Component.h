#include "game_objects/Entity.h"

namespace lcn { namespace object
{
	class Component
	{
	public:
		Component() {}
		virtual ~Component() {}
		
		virtual void Update() = 0;
		
	protected:
		Entity* m_Parent = 0;
	};
};}; // namespace lcn::object