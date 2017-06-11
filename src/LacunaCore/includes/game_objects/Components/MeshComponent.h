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
		const uint32_t GetMeshGUID();

		void SetMeshIndexCount(uint32_t a_Count);
		const uint32_t GetMeshIndexCount();

	private:
		uint32_t m_MeshGUID = 0;
		uint32_t m_IndexCount = 0;
	};
}; // namespace lcn::object