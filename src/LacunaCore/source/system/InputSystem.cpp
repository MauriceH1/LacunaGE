#include "system/InputSystem.h"
#include "LacunaWincodes.h"
#include <iostream>

namespace lcn::InputSystem
{
	namespace
	{
		bool m_Keys[LCN_KEY_MAX];
	};

	/* ########## FUNCTION PROTOTYPES ########## */
	size_t TranslateKey(__int64 a_KeyCode);

	/* ########## DECLARATIONS ########## */
	bool IsKeyDown(size_t a_KeyCode)
	{
		return m_Keys[a_KeyCode];
	}

	void KeyUp(__int64 a_KeyCode)
	{
		m_Keys[TranslateKey(a_KeyCode)] = false;
	}

	void KeyDown(__int64 a_KeyCode)
	{
		m_Keys[TranslateKey(a_KeyCode)] = true;
	}

	size_t TranslateKey(__int64 a_KeyCode)
	{
		switch (a_KeyCode)
		{
			case LCN_PLATFORM_CODE_0: {
				return LCN_KEY_0;
			} break;
			case LCN_PLATFORM_CODE_1: {
				return LCN_KEY_1;
			} break;
			case LCN_PLATFORM_CODE_2: {
				return LCN_KEY_2;
			} break;
			case LCN_PLATFORM_CODE_3: {
				return LCN_KEY_3;
			} break;
			case LCN_PLATFORM_CODE_4: {
				return LCN_KEY_4;
			} break;
			case LCN_PLATFORM_CODE_5: {
				return LCN_KEY_5;
			} break;
			case LCN_PLATFORM_CODE_6: {
				return LCN_KEY_6;
			} break;
			case LCN_PLATFORM_CODE_7: {
				return LCN_KEY_7;
			} break;
			case LCN_PLATFORM_CODE_8: {
				return LCN_KEY_8;
			} break;
			case LCN_PLATFORM_CODE_9: {
				return LCN_KEY_9;
			} break;

			case LCN_PLATFORM_CODE_A: {
				return LCN_KEY_A;
			} break;
			case LCN_PLATFORM_CODE_B: {
				return LCN_KEY_B;
			} break;
			case LCN_PLATFORM_CODE_C: {
				return LCN_KEY_C;
			} break;
			case LCN_PLATFORM_CODE_D: {
				return LCN_KEY_D;
			} break;
			case LCN_PLATFORM_CODE_E: {
				return LCN_KEY_E;
			} break;
			case LCN_PLATFORM_CODE_F: {
				return LCN_KEY_F;
			} break;
			case LCN_PLATFORM_CODE_G: {
				return LCN_KEY_G;
			} break;
			case LCN_PLATFORM_CODE_H: {
				return LCN_KEY_H;
			} break;
			case LCN_PLATFORM_CODE_I: {
				return LCN_KEY_I;
			} break;
			case LCN_PLATFORM_CODE_J: {
				return LCN_KEY_J;
			} break;
			case LCN_PLATFORM_CODE_K: {
				return LCN_KEY_K;
			} break;
			case LCN_PLATFORM_CODE_L: {
				return LCN_KEY_L;
			} break;
			case LCN_PLATFORM_CODE_M: {
				return LCN_KEY_M;
			} break;
			case LCN_PLATFORM_CODE_N: {
				return LCN_KEY_N;
			} break;
			case LCN_PLATFORM_CODE_O: {
				return LCN_KEY_O;
			} break;
			case LCN_PLATFORM_CODE_P: {
				return LCN_KEY_P;
			} break;
			case LCN_PLATFORM_CODE_Q: {
				return LCN_KEY_Q;
			} break;
			case LCN_PLATFORM_CODE_R: {
				return LCN_KEY_R;
			} break;
			case LCN_PLATFORM_CODE_S: {
				return LCN_KEY_S;
			} break;
			case LCN_PLATFORM_CODE_T: {
				return LCN_KEY_T;
			} break;
			case LCN_PLATFORM_CODE_U: {
				return LCN_KEY_U;
			} break;
			case LCN_PLATFORM_CODE_V: {
				return LCN_KEY_V;
			} break;
			case LCN_PLATFORM_CODE_W: {
				return LCN_KEY_W;
			} break;
			case LCN_PLATFORM_CODE_X: {
				return LCN_KEY_X;
			} break;
			case LCN_PLATFORM_CODE_Y: {
				return LCN_KEY_Y;
			} break;
			case LCN_PLATFORM_CODE_Z: {
				return LCN_KEY_Z;
			} break;
			default: {
				return LCN_KEY_UNKNOWN;
			} break;
		}
	}
}; // namespace lcn::InputSystem