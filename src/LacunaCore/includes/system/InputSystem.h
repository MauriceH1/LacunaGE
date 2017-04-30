#pragma once

#include "InputHelpers.h"

namespace lcn::InputSystem
{
	bool IsKeyDown(size_t a_KeyCode);

	void KeyUp(__int64 a_KeyCode);
	void KeyDown(__int64 a_KeyCode);
}; // namespace lcn::InputSystem