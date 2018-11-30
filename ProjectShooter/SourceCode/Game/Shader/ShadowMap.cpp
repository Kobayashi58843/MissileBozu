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
	//�V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	D3D11_MAPPED_SUBRESOURCE pMeshData;
	//�o�b�t�@���̃f�[�^�̏��������J�n����Map.
	if (SUCCEEDED(pDeviceContext11->Map(pBuffPerMesh, 0, D3D11_MAP_WRITE_DISCARD, 0, &pMeshData)))
	{
		//�R���X�^���g�o�b�t�@.
		CBUFF_STATICMESH_FOR_SHADOWMAP cb;

		//���[���h�s���n��.
		D3DXMATRIX m = mWorld;
		D3DXMatrixTranspose(&m, &m);
		cb.mW = m;

		//( W * V * P )�s���n��.
		m = mWorld * mView * mProj;
		D3DXMatrixTranspose(&m, &m);
		cb.mWVP = m;

		//UV�̍s��.
		D3DXMATRIX mClipToUV;
		ZeroMemory(&mClipToUV, sizeof(D3DXMATRIX));
		mClipToUV._11 = 0.5;
		mClipToUV._22 = -0.5;
		mClipToUV._33 = 1;
		mClipToUV._41 = 0.5;
		mClipToUV._42 = 0.5;
		mClipToUV._44 = 1;

		//( W * L * P * UV )�s��.
		m = mWorld * mLightView * mProj * mClipToUV;
		D3DXMatrixTranspose(&m, &m);
		cb.mWLPT = m;

		//( W * L * P )�s��.
		m = mWorld * mLightView * mProj;
		D3DXMatrixTranspose(&m, &m);
		cb.mWLP = m;

		//�R�s�[��̃o�b�t�@,�R�s�[��̃o�b�t�@�T�C�Y,�R�s�[���̃o�b�t�@,�R�s�[����T�C�Y.
		memcpy_s(pMeshData.pData, pMeshData.RowPitch, (void*)(&cb), sizeof(cb));

		//�o�b�t�@���̃f�[�^�̏��������I������Unmap.
		pDeviceContext11->Unmap(pBuffPerMesh, 0);
	}

	//�R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�����H.
	pDeviceContext11->VSSetConstantBuffers(0, 1, &pBuffPerMesh);//���_�V�F�[�_.
	pDeviceContext11->PSSetConstantBuffers(0, 1, &pBuffPerMesh);//�s�N�Z���V�F�[�_.

	D3D11_MAPPED_SUBRESOURCE pFrameData;
	//�o�b�t�@���̃f�[�^�̏��������J�n����Map.
	if (SUCCEEDED(pDeviceContext11->Map(pBuffPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &pFrameData)))
	{
		//�R���X�^���g�o�b�t�@.
		SUB_CBUFF_STATICMESH_FOR_SHADOWMAP cb;

		cb.vEye = { vLightCameraPosition.x, vLightCameraPosition.y, vLightCameraPosition.z, 0.0f };

		//�R�s�[��̃o�b�t�@,�R�s�[��̃o�b�t�@�T�C�Y,�R�s�[���̃o�b�t�@,�R�s�[����T�C�Y.
		memcpy_s(pFrameData.pData, pFrameData.RowPitch, (void*)(&cb), sizeof(cb));

		//�o�b�t�@���̃f�[�^�̏��������I������Unmap.
		pDeviceContext11->Unmap(pBuffPerFrame, 0);
	}

	//�R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg�����H.
	pDeviceContext11->VSSetConstantBuffers(1, 1, &pBuffPerFrame);//���_�V�F�[�_.
	pDeviceContext11->PSSetConstantBuffers(1, 1, &pBuffPerFrame);//�s�N�Z���V�F�[�_.

	//���_�C���v�b�g���C�A�E�g���Z�b�g.
	pDeviceContext11->IASetInputLayout(pVertexLayout);

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g.
	pDeviceContext11->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//���_�o�b�t�@���Z�b�g.
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

		//�[�x�e�N�X�`�����V�F�[�_�ɓn��.
		pDeviceContext11->PSSetSamplers(1, 1, &pSampleLinearDepth);
		pDeviceContext11->PSSetShaderResources(1, 1, &pGBuffer);

		//�v���~�e�B�u(�|���S��)�������_�����O.
		pDeviceContext11->DrawIndexed(pMaterials[AttrID[i]].dwNumFace * 3, 0, 0);
	}
}