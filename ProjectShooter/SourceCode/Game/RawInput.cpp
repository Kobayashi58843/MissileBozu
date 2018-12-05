#include "RawInput.h"

RawInput::RawInput()
	: m_bLButtonDown(false)
	, m_bLButtonUp(false)
	, m_bLButtonHoldDown(false)
	, m_bRButtonDown(false)
	, m_bRButtonUp(false)
	, m_bRButtonHoldDown(false)
	, m_iWheelFraction(0)
	, m_bWheelForward(false)
	, m_bWheelBackward(false)
{
	CrearVECTOR2(m_vMouseMovingDistance);
}

RawInput::~RawInput()
{
}

//RawInput初期化.
void RawInput::InitRawInput(const HWND hWnd)
{
	//WM_INPUTで受け取れるように登録.
	RAWINPUTDEVICE device[1];
	device[0].usUsagePage = 0x01;
	device[0].usUsage = 0x02;
	device[0].dwFlags = RIDEV_INPUTSINK;
	device[0].hwndTarget = hWnd;

	if (RegisterRawInputDevices(device, 1, sizeof(device[0])) == 0)
	{
		ERR_MSG("Main::InitRawInput() : デバイスの登録失敗", "error");
	}
}

//メッセージ毎の処理.
void RawInput::MsgProc(const UINT uMsg, const LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		m_iWheelFraction = NULL;

		break;
	case WM_INPUT:
	{
		UINT dwSize = 40;
		BYTE lpb[40];

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

		RAWINPUT* RawInput = (RAWINPUT*)lpb;

		if (RawInput->header.dwType == RIM_TYPEMOUSE)
		{
			//マウスの移動量.
			m_vMouseMovingDistance.x = static_cast<float>(RawInput->data.mouse.lLastX);
			m_vMouseMovingDistance.y = static_cast<float>(RawInput->data.mouse.lLastY);

			//現在の押されているボタンの確認.
			ConfirmationPushedButton(RawInput);
		}
	}
		break;
	}
}

//現在の押されているボタンの確認.
void RawInput::ConfirmationPushedButton(RAWINPUT* RawInput)
{
	assert(RawInput != nullptr);
	USHORT usButtonFlags = RawInput->data.mouse.usButtonFlags;

	switch (usButtonFlags)
	{
	case RI_MOUSE_LEFT_BUTTON_DOWN:
		m_bLButtonDown = true;
		m_bLButtonHoldDown = true;

		break;
	case RI_MOUSE_LEFT_BUTTON_UP:
		m_bLButtonUp = true;
		m_bLButtonHoldDown = false;

		break;
	case RI_MOUSE_RIGHT_BUTTON_DOWN:
		m_bRButtonDown = true;
		m_bRButtonHoldDown = true;

		break;
	case RI_MOUSE_RIGHT_BUTTON_UP:
		m_bRButtonUp = true;
		m_bRButtonHoldDown = false;

		break;
	case RI_MOUSE_WHEEL:
		{
			//回転量.
			short sDelta = static_cast<short>(RawInput->data.mouse.usButtonData);

			//前回の端数を追加.
			sDelta += m_iWheelFraction;

			//ノッチ数を求める.
			int iNotch = sDelta / WHEEL_DELTA;

			//端数を保存する.
			m_iWheelFraction = sDelta % WHEEL_DELTA;

			if (iNotch > 0)//前に回転.
			{
				m_bWheelForward = true;
				m_bWheelBackward = false;
			}
			else if (iNotch < 0)//後ろに回転.
			{
				m_bWheelForward = false;
				m_bWheelBackward = true;
			}
		}

		break;
	default:

		break;
	}
}