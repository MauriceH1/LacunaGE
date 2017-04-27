#pragma once

#include "game_objects/Component.h"

namespace lcn::object
{
	class AudioSourceComponent : public Component
	{
	public:
		AudioSourceComponent() {}
		virtual ~AudioSourceComponent() {}

		virtual void Update() override {};
	};
}; // namespace lcn::object