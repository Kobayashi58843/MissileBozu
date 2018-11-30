#pragma once

#include "..\\Global.h"

class RawInput
{
public:
	RawInput();
	~RawInput();

	//RawInput初期化.
	void InitRawInput(const HWND hWnd);

	//メッセージ毎の処理.
	void MsgProc(const UINT uMsg, const LPARAM lParam);

	void ClearPerFrame()
	{
		CrearVECTOR2(m_vMouseMovingDistance);

		m_bLButtonDown = false;
		m_bLButtonUp = false;
		m_bRButtonDown = false;
		m_bRButtonUp = false;

		m_bWheelForward = false;
		m_bWheelBackward = false;
	}

	//マウスの移動量.
	D3DXVECTOR2 GetMouseMovingDistance() const
	{
		return m_vMouseMovingDistance;
	}

	//左クリックされた瞬間.
	bool IsLButtonDown() const
	{
		return m_bLButtonDown;
	}

	//左クリックが離された瞬間.
	bool IsLButtonUp() const
	{
		return m_bLButtonUp;
	}

	//左クリックされているか.
	bool IsLButtonHoldDown() const
	{
		return m_bLButtonHoldDown;
	}

	//右クリックされた瞬間.
	bool IsRButtonDown() const
	{
		return m_bRButtonDown;
	}

	//右クリックが離された瞬間.
	bool IsRButtonUp() const
	{
		return m_bRButtonUp;
	}

	//右クリックされているか.
	bool IsRButtonHoldDown() const
	{
		return m_bRButtonHoldDown;
	}

	bool IsWheelForward() const
	{
		return m_bWheelForward;
	}

	bool IsWheelBackward() const
	{
		return m_bWheelBackward;
	}


private:
	//マウスの移動量.
	D3DXVECTOR2 m_vMouseMovingDistance;

	//左クリック.
	bool m_bLButtonDown;
	bool m_bLButtonUp;
	bool m_bLButtonHoldDown;

	//右クリック.
	bool m_bRButtonDown;
	bool m_bRButtonUp;
	bool m_bRButtonHoldDown;

	//ホイールの端数.
	int m_iWheelFraction;
	bool m_bWheelForward;
	bool m_bWheelBackward;

	//現在の押されているボタンの確認.
	void ConfirmationPushedButton(RAWINPUT* RawInput);
};