#pragma once

#include "ShaderGathering.h"

class DepthTexture
{
public:
	DepthTexture();
	~DepthTexture();

	/*================/ �X�^�e�B�b�N���b�V�� /================*/

	//�ǂ̃��b�V���ł����ʂ��Ă���V�F�[�_�ɓn���f�[�^.
	//��)���C�g,�J�����ʒu��.
	void RenderStaticModel(
		ID3D11DeviceContext* const pDeviceContext11,
		ID3D11Buffer* const pVertexBuffer, ID3D11InputLayout* const pVertexLayout,
		LPD3DXMESH const pMesh,
		ID3D11Buffer* const pBuffPerMesh,
		const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj);

	//���b�V�����ƂɃV�F�[�_�ɓn���K�v������f�[�^.
	void RenderStaticModelMesh(
		ID3D11DeviceContext* const pDeviceContext11,
		MY_MATERIAL* const pMaterials, const DWORD NumAttr, const DWORD AttrID[300],
		ID3D11Buffer** const ppIndexBuffer, ID3D11SamplerState* const pSampleLinear);

	/*================/ �X�L�����b�V�� /================*/

	void RenderSkinModel(
		ID3D11DeviceContext* const pDeviceContext11,
		SKIN_PARTS_MESH* const pMesh, ID3D11Buffer* const pCBuff,
		const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj,
		ID3D11SamplerState* const pSampleLinear);

};