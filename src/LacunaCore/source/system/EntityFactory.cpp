#include "system/EntityFactory.h"

#include <list>

using namespace lcn;
using namespace lcn::object;

namespace lcn::EntityFactory
{
	namespace // Data Members
	{
		std::list<std::shared_ptr<Entity>> m_Entities;
	}

	std::shared_ptr<lcn::object::Entity> CreateEntity()
	{
		std::shared_ptr<Entity> entity = std::make_shared<Entity>();
		m_Entities.push_back(entity);
		return entity;
	}
}; // namespace