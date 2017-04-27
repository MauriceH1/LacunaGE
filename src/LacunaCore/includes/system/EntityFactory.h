#pragma once

#include <memory>
#include "game_objects/Entity.h"


namespace lcn::EntityFactory
{
	std::shared_ptr<lcn::object::Entity> CreateEntity();
}