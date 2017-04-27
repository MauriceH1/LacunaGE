#pragma once

namespace lcn::object
{
	class Entity;
	class Component
	{
	public:
		Component() {}
		virtual ~Component() {}
		
		virtual void Update() = 0;
		
		void SetParent(Entity* a_Parent) { m_Parent = a_Parent; };

	protected:
		Entity* m_Parent = 0;
	};
}; // namespace lcn::object