#pragma once

#include "game_objects/Component.h"

namespace lcn::object
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent();
		~MeshComponent();

		virtual void Update() override;
	};
}; // namespace lcn::object