#pragma once

#include <cstdint>

#include "game_objects/Component.h"

namespace lcn::object
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent();
		~MeshComponent();

		virtual void Update() override;

		void SetMeshGUID(uint32_t a_GUID);
		uint32_t GetMeshGUID();

	private:
		uint32_t m_MeshGUID = 0;
	};
}; // namespace lcn::object