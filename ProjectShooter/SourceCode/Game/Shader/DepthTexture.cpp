#include "DepthTexture.h"

DepthTexture::DepthTexture()
{
}

DepthTexture::~DepthTexture()
{
}

/*================/ �X�^�e�B�b�N���b�V�� /================*/

void DepthTexture::RenderStaticModel(
	ID3D11DeviceContext* const pDeviceContext11,
	ID3D11Buffer* const pVertexBuffer, ID3D11InputLayout* const pVertexLayout,
	LPD3DXMESH const pMesh,
	ID3D11Buffer* const pBuffPerMesh,
	const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj)
{
	//�V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	D3D11_MAPPED_SUBRESOURCE pData;
	//�o�b�t�@���̃f�[�^�̏��������J�n����Map.
	if (SUCCEEDED(pDeviceContext11->Map(pBuffPerMesh, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		CBUFF_STATICMESH_FOR_SIMPLE cb;//�R���X�^���g�o�b�t�@.

		//���[���h�s���n��.
		cb.mW = mWorld;
		D3DXMatrixTranspose(&cb.mW, &cb.mW);

		//���[���h,�r���[(�J����),�v���W�F�N�V����(�ˉe)�s���n��.
		D3DXMATRIX m = mWorld * mView * mProj;
		D3DXMatrixTranspose(&m, &m);//�s���]�u����.
		//���s��̌v�Z���@��DirectX��GPU�ňقȂ邽��.

		cb.mWVP = m;

		//�R�s�[��̃o�b�t�@,�R�s�[��̃o�b�t�@�T�C�Y,�R�s�[���̃o�b�t�@,�R�s�[����T�C�Y.
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));

		//�o�b�t�@���̃f�[�^�̏��������I������Unmap.
		pDeviceContext11->Unmap(pBuffPerMesh, 0);
	}

	//�R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�����H.
	pDeviceContext11->VSSetConstantBuffers(0, 1, &pBuffPerMesh);//���_�V�F�[�_.
	pDeviceContext11->PSSetConstantBuffers(0, 1, &pBuffPerMesh);//�s�N�Z���V�F�[�_.

	//���_�C���v�b�g���C�A�E�g���Z�b�g.
	pDeviceContext11->IASetInputLayout(pVertexLayout);

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g.
	pDeviceContext11->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//���_�o�b�t�@���Z�b�g.
	UINT stride = pMesh->GetNumBytesPerVertex();
	UINT offset = 0;
	pDeviceContext11->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
}

void DepthTexture::RenderStaticModelMesh(
	ID3D11DeviceContext* const pDeviceContext11,
	MY_MATERIAL* const pMaterials, const DWORD NumAttr, const DWORD AttrID[300],
	ID3D11Buffer** const ppIndexBuffer, ID3D11SamplerState* const pSampleLinear)
{
	//�����̐������A���ꂼ��̑����̃C���f�b�N�X�o�b�t�@��`��.
	for (DWORD i = 0; i<NumAttr; i++)
	{
		//�g�p����Ă��Ȃ��}�e���A���΍�.
		if (pMaterials[AttrID[i]].dwNumFace == 0)
		{
			continue;
		}
		//�C���f�b�N�X�o�b�t�@���Z�b�g.
		pDeviceContext11->IASetIndexBuffer(ppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		//�e�N�X�`���ƃT���v�����V�F�[�_�ɓn��.
		pDeviceContext11->PSSetSamplers(0, 1, &pSampleLinear);
		pDeviceContext11->PSSetShaderResources(0, 1, &pMaterials[AttrID[i]].pTexture);

		//�v���~�e�B�u(�|���S��)�������_�����O.
		pDeviceContext11->DrawIndexed(pMaterials[AttrID[i]].dwNumFace * 3, 0, 0);
	}
}

/*================/ �X�L�����b�V�� /================*/

void DepthTexture::RenderSkinModel(
	ID3D11DeviceContext* const pDeviceContext11,
	SKIN_PARTS_MESH* const pMesh, ID3D11Buffer* const pCBuff,
	const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj,
	ID3D11SamplerState* const pSampleLinear)
{
	// �}�e���A���̐������A
	// ���ꂼ��̃}�e���A���̃C���f�b�N�X�o�b�t�@��`��.
	for (DWORD i = 0; i<pMesh->dwNumMaterial; i++)
	{
		// �g�p����Ă��Ȃ��}�e���A���΍�.
		if (pMesh->pMaterial[i].dwNumFace == 0)
		{
			continue;
		}
		// �C���f�b�N�X�o�b�t�@���Z�b�g.
		UINT uiStride = sizeof(int);
		UINT uiOffset = 0;
		pDeviceContext11->IASetIndexBuffer(pMesh->ppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		// �}�e���A���̊e�v�f�ƕϊ��s����V�F�[�_�ɓn��.
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

		// �e�N�X�`�����V�F�[�_�ɓn��.
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