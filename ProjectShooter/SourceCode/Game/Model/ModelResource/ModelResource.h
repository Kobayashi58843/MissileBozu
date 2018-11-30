#pragma once

#include "StaticMesh\\DX9Mesh.h"
#include "SkinMesh\\CD3DXSKINMESH.h"

/*=-=-=-=-=-=-=-=-=/ リソースクラス /=-=-=-=-=-=-=-=-=*/
class ModelResource
{
public:
	//スタティックモデル種類.
	enum enStaticModel
	{
		enStaticModel_Ground = 0,
		enStaticModel_Shpere,

		enStaticModel_Max,
	};
	//スキンモデル種類.
	enum enSkinModel
	{
		enSkinModel_Player = 0,
		enSkinModel_Enemy,
		
		enSkinModel_Max,
	};

	ModelResource();
	~ModelResource();

	//全てのモデルの作成.
	void CreateModelAll(const HWND hWnd, ID3D11Device* const pDevice, ID3D11DeviceContext* const pContext);

	//全てのモデルの解放.
	void ReleaceModelAll();

	/*====/ スタティックモデル /====*/
	clsDX9Mesh*	GetStaticModels(enStaticModel enModel);

	/*====/ スキンモデル /====*/
	clsD3DXSKINMESH*	GetSkinModels(enSkinModel enModel);

private:
	HWND					m_hWnd;
	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	clsDX9Mesh**			m_ppStaticModels;
	clsD3DXSKINMESH**		m_ppSkinModels;

	/*====/ スタティックモデル /====*/
	void	CreateStaticModelAll(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT	InitStaticModel(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT	CreateStaticModel(LPSTR fileName, enStaticModel enModel);
	HRESULT	ReleaseStaticModel(enStaticModel enModel);
	bool	IsRengeStaticModel(enStaticModel enModel);

	/*====/ スキンモデル /====*/
	void	CreateSkinModelAll(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT	InitSkinModel(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT	CreateSkinModel(LPSTR fileName, enSkinModel enModel);
	HRESULT	ReleaseSkinModel(enSkinModel enModel);
	bool	IsRengeSkinModel(enSkinModel enModel);
};
