#pragma once

#include "StaticMesh\\DX9Mesh.h"
#include "SkinMesh\\CD3DXSKINMESH.h"

/*=-=-=-=-=-=-=-=-=/ ���\�[�X�N���X /=-=-=-=-=-=-=-=-=*/
class ModelResource
{
public:
	//�X�^�e�B�b�N���f�����.
	enum enStaticModel
	{
		enStaticModel_Ground = 0,
		enStaticModel_Shpere,

		enStaticModel_Max,
	};
	//�X�L�����f�����.
	enum enSkinModel
	{
		enSkinModel_Player = 0,
		enSkinModel_Enemy,
		
		enSkinModel_Max,
	};

	ModelResource();
	~ModelResource();

	//�S�Ẵ��f���̍쐬.
	void CreateModelAll(const HWND hWnd, ID3D11Device* const pDevice, ID3D11DeviceContext* const pContext);

	//�S�Ẵ��f���̉��.
	void ReleaceModelAll();

	/*====/ �X�^�e�B�b�N���f�� /====*/
	clsDX9Mesh*	GetStaticModels(enStaticModel enModel);

	/*====/ �X�L�����f�� /====*/
	clsD3DXSKINMESH*	GetSkinModels(enSkinModel enModel);

private:
	HWND					m_hWnd;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	clsDX9Mesh**			m_ppStaticModels;
	clsD3DXSKINMESH**		m_ppSkinModels;

	/*====/ �X�^�e�B�b�N���f�� /====*/
	void	CreateStaticModelAll(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT	InitStaticModel(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT	CreateStaticModel(LPSTR fileName, enStaticModel enModel);
	HRESULT	ReleaseStaticModel(enStaticModel enModel);
	bool	IsRengeStaticModel(enStaticModel enModel);

	/*====/ �X�L�����f�� /====*/
	void	CreateSkinModelAll(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT	InitSkinModel(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT	CreateSkinModel(LPSTR fileName, enSkinModel enModel);
	HRESULT	ReleaseSkinModel(enSkinModel enModel);
	bool	IsRengeSkinModel(enSkinModel enModel);
};
