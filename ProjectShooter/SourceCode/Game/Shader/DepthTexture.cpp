#include "DepthTexture.h"

DepthTexture::DepthTexture()
{
}

DepthTexture::~DepthTexture()
{
}

/*================/ スタティックメッシュ /================*/

void DepthTexture::RenderStaticModel(
	ID3D11DeviceContext* const pDeviceContext11,
	ID3D11Buffer* const pVertexBuffer, ID3D11InputLayout* const pVertexLayout,
	LPD3DXMESH const pMesh,
	ID3D11Buffer* const pBuffPerMesh,
	const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj)
{
	//シェーダのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	//バッファ内のデータの書き換え開始時にMap.
	if (SUCCEEDED(pDeviceContext11->Map(pBuffPerMesh, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		CBUFF_STATICMESH_FOR_SIMPLE cb;//コンスタントバッファ.

		//ワールド行列を渡す.
		cb.mW = mWorld;
		D3DXMatrixTranspose(&cb.mW, &cb.mW);

		//ワールド,ビュー(カメラ),プロジェクション(射影)行列を渡す.
		D3DXMATRIX m = mWorld * mView * mProj;
		D3DXMatrixTranspose(&m, &m);//行列を転置する.
		//※行列の計算方法がDirectXとGPUで異なるため.

		cb.mWVP = m;

		//コピー先のバッファ,コピー先のバッファサイズ,コピー元のバッファ,コピーするサイズ.
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		//バッファ内のデータの書き換え終了時にUnmap.
		pDeviceContext11->Unmap(pBuffPerMesh, 0);
	}

	//コンスタントバッファをどのシェーダで使うか？.
	pDeviceContext11->VSSetConstantBuffers(0, 1, &pBuffPerMesh);//頂点シェーダ.
	pDeviceContext11->PSSetConstantBuffers(0, 1, &pBuffPerMesh);//ピクセルシェーダ.

	//頂点インプットレイアウトをセット.
	pDeviceContext11->IASetInputLayout(pVertexLayout);

	//プリミティブ・トポロジーをセット.
	pDeviceContext11->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//頂点バッファをセット.
	UINT stride = pMesh->GetNumBytesPerVertex();
	UINT offset = 0;
	pDeviceContext11->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
}

void DepthTexture::RenderStaticModelMesh(
	ID3D11DeviceContext* const pDeviceContext11,
	MY_MATERIAL* const pMaterials, const DWORD NumAttr, const DWORD AttrID[300],
	ID3D11Buffer** const ppIndexBuffer, ID3D11SamplerState* const pSampleLinear)
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

		//プリミティブ(ポリゴン)をレンダリング.
		pDeviceContext11->DrawIndexed(pMaterials[AttrID[i]].dwNumFace * 3, 0, 0);
	}
}

/*================/ スキンメッシュ /================*/

void DepthTexture::RenderSkinModel(
	ID3D11DeviceContext* const pDeviceContext11,
	SKIN_PARTS_MESH* const pMesh, ID3D11Buffer* const pCBuff,
	const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj,
	ID3D11SamplerState* const pSampleLinear)
{
	// マテリアルの数だけ、
	// それぞれのマテリアルのインデックスバッファを描画.
	for (DWORD i = 0; i<pMesh->dwNumMaterial; i++)
	{
		// 使用されていないマテリアル対策.
		if (pMesh->pMaterial[i].dwNumFace == 0)
		{
			continue;
		}
		// インデックスバッファをセット.
		UINT uiStride = sizeof(int);
		UINT uiOffset = 0;
		pDeviceContext11->IASetIndexBuffer(pMesh->ppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		// マテリアルの各要素と変換行列をシェーダに渡す.
		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(pDeviceContext11->Map(pCBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			CBUFF_SKINMESH_FOR_MESH cb;

			cb.mW = mWorld;
			D3DXMatrixTranspose(&cb.mW, &cb.mW);
			cb.mWVP = mWorld * mView * mProj;
			D3DXMatrixTranspose(&cb.mWVP, &cb.mWVP);

			cb.vAmbient = pMesh->pMaterial[i].Ka;
			cb.vDiffuse = pMesh->pMaterial[i].Kd;
			cb.vSpecular = pMesh->pMaterial[i].Ks;

			memcpy_s(pData.pData, pData.RowPitch, (void*)&cb, sizeof(CBUFF_SKINMESH_FOR_MESH));
			pDeviceContext11->Unmap(pCBuff, 0);
		}
		pDeviceContext11->VSSetConstantBuffers(1, 1, &pCBuff);
		pDeviceContext11->PSSetConstantBuffers(1, 1, &pCBuff);

		// テクスチャをシェーダに渡す.
		if (pMesh->pMaterial[i].szTextureName[0] != NULL)
		{
			pDeviceContext11->PSSetSamplers(0, 1, &pSampleLinear);
			pDeviceContext11->PSSetShaderResources(0, 1, &pMesh->pMaterial[i].pTexture);
		}
		else
		{
			ID3D11ShaderResourceView* Nothing[1] = { 0 };
			pDeviceContext11->PSSetShaderResources(0, 1, Nothing);
		}
		// Draw.
		pDeviceContext11->DrawIndexed(pMesh->pMaterial[i].dwNumFace * 3, 0, 0);
	}
}