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

	//Direct3D�쐬.
	void Create(HWND hWnd);

	//���.
	void Release();

	//���X�^���C�Y�ݒ�.
	void SetRasterize(D3D11_CULL_MODE CullMode);

	//�[�x�e�X�g(Z�e�X�g) ON/OFF�ؑ�.
	void SetDepth(bool bFlg);

	//�o�b�N�o�b�t�@�̃N���A.
	void ClearBackBuffer(D3DXVECTOR4 vClearColor = { 0.0f, 0.0f, 1.0f, 1.0f });

	//�����_�����O���ꂽ�C���[�W��\��.
	void RenderSwapChain();

private:

	//�f�o�C�X�I�u�W�F�N�g.
	ID3D11Device*				m_pDevice;
	//�f�o�C�X�R���e�L�X�g.
	ID3D11DeviceContext*		m_pDeviceContext;
	//�X���b�v�`�F�[��.
	IDXGISwapChain*				m_pSwapChain;
	//�����_�[�^�[�Q�b�g�r���[.
	ID3D11RenderTargetView*		m_pBackBuffer_RTV;
	//�o�b�N�o�b�t�@.
	ID3D11Texture2D*			m_pBackBuffer_Tex;
	//�f�v�X�X�e���V���r���[.
	ID3D11DepthStencilView*		m_pBackBuffer_DSV;
	//�[�x(Z)�e�X�g�ݒ�.
	ID3D11DepthStencilState*	m_pDepthStencilState;

};