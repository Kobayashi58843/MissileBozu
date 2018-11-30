#include "BackBuffer.h"

BackBuffer::BackBuffer(ID3D11Device* pDevice, UINT uiWidth, UINT uiHeight)
	: m_pRTV(nullptr)
	, m_pTex(nullptr)
	, m_pSRV(nullptr)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	ZeroMemory(&RTVDesc, sizeof(RTVDesc));

	desc.Width = uiWidth;
	desc.Height = uiHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	pDevice->CreateTexture2D(&desc, NULL, &m_pTex);

	RTVDesc.Format = desc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;
	pDevice->CreateRenderTargetView(m_pTex, &RTVDesc, &m_pRTV);

	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	pDevice->CreateShaderResourceView(m_pTex, &SRVDesc, &m_pSRV);
}

BackBuffer::~BackBuffer()
{
	SAFE_RELEASE(m_pSRV);
	SAFE_RELEASE(m_pTex);
	SAFE_RELEASE(m_pRTV);
}