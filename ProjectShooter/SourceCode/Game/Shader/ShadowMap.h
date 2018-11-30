#pragma once

#include "ShaderGathering.h"

class ShadowMap
{
public:
	ShadowMap();
	~ShadowMap();

	//どのメッシュでも共通しているシェーダに渡すデータ.
	//例)ライト,カメラ位置等.
	void RenderStaticModel(
		ID3D11DeviceContext* const pDeviceContext11,
		ID3D11Buffer* const pVertexBuffer, ID3D11InputLayout* const pVertexLayout,
		LPD3DXMESH const pMesh,
		ID3D11Buffer* const pBuffPerMesh, ID3D11Buffer* const pBuffPerFrame,
		const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj,
		const D3DXMATRIX &mLightView, const D3DXVECTOR3 vLightCameraPosition);

	//メッシュごとにシェーダに渡す必要があるデータ.
	void RenderStaticModelMesh(
		ID3D11DeviceContext* const pDeviceContext11,
		MY_MATERIAL* const pMaterials, const DWORD NumAttr, const DWORD AttrID[300],
		ID3D11ShaderResourceView* const pGBuffer,
		ID3D11Buffer** const ppIndexBuffer,
		ID3D11SamplerState* const pSampleLinear, ID3D11SamplerState* const pSampleLinearDepth);
};