#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::RenderStaticModel(
	ID3D11DeviceContext* const pDeviceContext11,
	ID3D11Buffer* const pVertexBuffer, ID3D11InputLayout* const pVertexLayout,
	LPD3DXMESH const pMesh,
	ID3D11Buffer* const pBuffPerMesh, ID3D11Buffer* const pBuffPerFrame,
	const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj,
	const D3DXMATRIX &mLightView, const D3DXVECTOR3 vLightCameraPosition)
{
	//シェーダのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pMeshData;
	//バッファ内のデータの書き換え開始時にMap.
	if (SUCCEEDED(pDeviceContext11->Map(pBuffPerMesh, 0, D3D11_MAP_WRITE_DISCARD, 0, &pMeshData)))
	{
		//コンスタントバッファ.
		CBUFF_STATICMESH_FOR_SHADOWMAP cb;

		//ワールド行列を渡す.
		D3DXMATRIX m = mWorld;
		D3DXMatrixTranspose(&m, &m);
		cb.mW = m;

		//( W * V * P )行列を渡す.
		m = mWorld * mView * mProj;
		D3DXMatrixTranspose(&m, &m);
		cb.mWVP = m;

		//UVの行列.
		D3DXMATRIX mClipToUV;
		ZeroMemory(&mClipToUV, sizeof(D3DXMATRIX));
		mClipToUV._11 = 0.5;
		mClipToUV._22 = -0.5;
		mClipToUV._33 = 1;
		mClipToUV._41 = 0.5;
		mClipToUV._42 = 0.5;
		mClipToUV._44 = 1;

		//( W * L * P * UV )行列.
		m = mWorld * mLightView * mProj * mClipToUV;
		D3DXMatrixTranspose(&m, &m);
		cb.mWLPT = m;

		//( W * L * P )行列.
		m = mWorld * mLightView * mProj;
		D3DXMatrixTranspose(&m, &m);
		cb.mWLP = m;

		//コピー先のバッファ,コピー先のバッファサイズ,コピー元のバッファ,コピーするサイズ.
		memcpy_s(pMeshData.pData, pMeshData.RowPitch, (void*)(&cb), sizeof(cb));

		//バッファ内のデータの書き換え終了時にUnmap.
		pDeviceContext11->Unmap(pBuffPerMesh, 0);
	}

	//コンスタントバッファをどのシェーダで使うか？.
	pDeviceContext11->VSSetConstantBuffers(0, 1, &pBuffPerMesh);//頂点シェーダ.
	pDeviceContext11->PSSetConstantBuffers(0, 1, &pBuffPerMesh);//ピクセルシェーダ.

	D3D11_MAPPED_SUBRESOURCE pFrameData;
	//バッファ内のデータの書き換え開始時にMap.
	if (SUCCEEDED(pDeviceContext11->Map(pBuffPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &pFrameData)))
	{
		//コンスタントバッファ.
		SUB_CBUFF_STATICMESH_FOR_SHADOWMAP cb;

		cb.vEye = { vLightCameraPosition.x, vLightCameraPosition.y, vLightCameraPosition.z, 0.0f };

		//コピー先のバッファ,コピー先のバッファサイズ,コピー元のバッファ,コピーするサイズ.
		memcpy_s(pFrameData.pData, pFrameData.RowPitch, (void*)(&cb), sizeof(cb));

		//バッファ内のデータの書き換え終了時にUnmap.
		pDeviceContext11->Unmap(pBuffPerFrame, 0);
	}

	//コンスタントバッファをどのシェーダで使うか？.
	pDeviceContext11->VSSetConstantBuffers(1, 1, &pBuffPerFrame);//頂点シェーダ.
	pDeviceContext11->PSSetConstantBuffers(1, 1, &pBuffPerFrame);//ピクセルシェーダ.

	//頂点インプットレイアウトをセット.
	pDeviceContext11->IASetInputLayout(pVertexLayout);

	//プリミティブ・トポロジーをセット.
	pDeviceContext11->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点バッファをセット.
	UINT stride = pMesh->GetNumBytesPerVertex();
	UINT offset = 0;
	pDeviceContext11->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
}

void ShadowMap::RenderStaticModelMesh(
	ID3D11DeviceContext* const pDeviceContext11,
	MY_MATERIAL* const pMaterials, const DWORD NumAttr, const DWORD AttrID[300],
	ID3D11ShaderResourceView* const pGBuffer,
	ID3D11Buffer** const ppIndexBuffer,
	ID3D11SamplerState* const pSampleLinear, ID3D11SamplerState* const pSampleLinearDepth)
{
	//属性の数だけ、それぞれの属性のインデックスバッファを描画.
	for (DWORD i = 0; i<NumAttr; i++)
	{
		//使用されていないマテリアル対策.
		if (pMaterials[AttrID[i]].dwNumFace == 0)
		{
			continue;
		}
		//インデックスバッファをセット.
		pDeviceContext11->IASetIndexBuffer(ppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		//テクスチャとサンプラをシェーダに渡す.
		pDeviceContext11->PSSetSamplers(0, 1, &pSampleLinear);
		pDeviceContext11->PSSetShaderResources(0, 1, &pMaterials[AttrID[i]].pTexture);

		//深度テクスチャをシェーダに渡す.
		pDeviceContext11->PSSetSamplers(1, 1, &pSampleLinearDepth);
		pDeviceContext11->PSSetShaderResources(1, 1, &pGBuffer);

		//プリミティブ(ポリゴン)をレンダリング.
		pDeviceContext11->DrawIndexed(pMaterials[AttrID[i]].dwNumFace * 3, 0, 0);
	}
}