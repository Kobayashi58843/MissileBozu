#pragma once

#include "ShaderGathering.h"

class DepthTexture
{
public:
	DepthTexture();
	~DepthTexture();

	/*================/ スタティックメッシュ /================*/

	//どのメッシュでも共通しているシェーダに渡すデータ.
	//例)ライト,カメラ位置等.
	void RenderStaticModel(
		ID3D11DeviceContext* const pDeviceContext11,
		ID3D11Buffer* const pVertexBuffer, ID3D11InputLayout* const pVertexLayout,
		LPD3DXMESH const pMesh,
		ID3D11Buffer* const pBuffPerMesh,
		const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj);

	//メッシュごとにシェーダに渡す必要があるデータ.
	void RenderStaticModelMesh(
		ID3D11DeviceContext* const pDeviceContext11,
		MY_MATERIAL* const pMaterials, const DWORD NumAttr, const DWORD AttrID[300],
		ID3D11Buffer** const ppIndexBuffer, ID3D11SamplerState* const pSampleLinear);

	/*================/ スキンメッシュ /================*/

	void RenderSkinModel(
		ID3D11DeviceContext* const pDeviceContext11,
		SKIN_PARTS_MESH* const pMesh, ID3D11Buffer* const pCBuff,
		const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj,
		ID3D11SamplerState* const pSampleLinear);

};