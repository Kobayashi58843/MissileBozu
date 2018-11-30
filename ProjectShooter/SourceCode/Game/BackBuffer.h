#pragma once

#include "..\\Global.h"

class BackBuffer
{
public:
	BackBuffer(ID3D11Device* pDevice, UINT uiWidth, UINT uiHeight);
	~BackBuffer();

	ID3D11RenderTargetView* GetRenderTargetView() const
	{
		return m_pRTV;
	}
	ID3D11Texture2D* GetTexture2D() const
	{
		return m_pTex;
	}
	ID3D11ShaderResourceView* GetShaderResourceView() const
	{
		return m_pSRV;
	}

private:
	ID3D11RenderTargetView*		m_pRTV;
	ID3D11Texture2D*			m_pTex;
	ID3D11ShaderResourceView*	m_pSRV;

};