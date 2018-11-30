#pragma once

#include "..\\Global.h"

class Direct3D
{
public:
	Direct3D();
	~Direct3D();

	ID3D11Device* GetDevice() const
	{
		return m_pDevice;
	}
	void SetDevice(ID3D11Device* const pDevice)
	{
		m_pDevice = pDevice;
	}

	ID3D11DeviceContext* GetDeviceContext() const
	{
		return m_pDeviceContext;
	}
	void SetDeviceContext(ID3D11DeviceContext* const pDeviceContext)
	{
		m_pDeviceContext = pDeviceContext;
	}

	ID3D11RenderTargetView* GetRenderTargetView() const
	{
		return m_pBackBuffer_RTV;
	}

	ID3D11Texture2D* GetTexture2D() const
	{
		return m_pBackBuffer_Tex;
	}
	void SetTexture2D(ID3D11Texture2D* const pTexture)
	{
		m_pBackBuffer_Tex = pTexture;
	}

	ID3D11DepthStencilView* GetDepthStencilView() const
	{
		return m_pBackBuffer_DSV;
	}

	//Direct3D作成.
	void Create(HWND hWnd);

	//解放.
	void Release();

	//ラスタライズ設定.
	void SetRasterize(D3D11_CULL_MODE CullMode);

	//深度テスト(Zテスト) ON/OFF切替.
	void SetDepth(bool bFlg);

	//バックバッファのクリア.
	void ClearBackBuffer(D3DXVECTOR4 vClearColor = { 0.0f, 0.0f, 1.0f, 1.0f });

	//レンダリングされたイメージを表示.
	void RenderSwapChain();

private:

	//デバイスオブジェクト.
	ID3D11Device*				m_pDevice;
	//デバイスコンテキスト.
	ID3D11DeviceContext*		m_pDeviceContext;
	//スワップチェーン.
	IDXGISwapChain*				m_pSwapChain;
	//レンダーターゲットビュー.
	ID3D11RenderTargetView*		m_pBackBuffer_RTV;
	//バックバッファ.
	ID3D11Texture2D*			m_pBackBuffer_Tex;
	//デプスステンシルビュー.
	ID3D11DepthStencilView*		m_pBackBuffer_DSV;
	//深度(Z)テスト設定.
	ID3D11DepthStencilState*	m_pDepthStencilState;

};