#pragma once

#include "..\\..\\..\\Shader\\ShaderGathering.h"

#include "..\\..\\..\\Shader\\NoTexture.h"
#include "..\\..\\..\\Shader\\Simple.h"
#include "..\\..\\..\\Shader\\DepthTexture.h"
#include "..\\..\\..\\Shader\\ShadowMap.h"

//メッシュデータをファイルから取り出す為だけに、DirectX9を使用する.
//※レンダリング(表示)は、DirectX11で行う.
class clsDX9Mesh
	: private NoTexture
	, private Simple
	, private DepthTexture
	, private ShadowMap
{
public:
	clsDX9Mesh();
	~clsDX9Mesh();

	LPD3DXMESH GetMesh() const
	{
		return m_pMesh;
	}

	LPD3DXMESH GetMeshForRay() const
	{
		return m_pMeshForRay;
	}

	//大きさ.
	void SetScale(const float fScale)
	{
		m_fScale = fScale;
	}
	float GetScale() const
	{
		return m_fScale;
	}
	void AddScale(const float fAddScale)
	{
		m_fScale += fAddScale;
	}

	//位置.
	void SetPos(const D3DXVECTOR3 vPos)
	{
		m_vPos = vPos;
	}
	D3DXVECTOR3 GetPos() const
	{
		return m_vPos;
	}
	void AddPos(const D3DXVECTOR3 fAddPos)
	{
		m_vPos += fAddPos;
	}

	//回転.
	void SetRot(const D3DXVECTOR3 vRot)
	{
		m_vRot = vRot;
	}
	D3DXVECTOR3 GetRot() const
	{
		return m_vRot;
	}
	void AddRot(const D3DXVECTOR3 fAddRot)
	{
		m_vRot += fAddRot;
	}

	HRESULT Init(HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPSTR fileName);

	void Release();

	//レンダリング用.
	void Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj);

	//シェーダ等をセット.
	HRESULT SetShader(const enStaticModelShader enShader);

	enStaticModelShader GetShader() const
	{
		return m_enShader;
	}

	//シャドウマップに必要なポインタを渡す.
	void SetShadowMapPointer(const SHADOWMAP_REQUIRED_POINTER Pointers)
	{
		m_ShadowMapPointers = Pointers;
	}

private:
	//ブレンドステート.
	struct BLEND_STATE
	{
		//透過あり.
		ID3D11BlendState*	m_pAlpha;
		//透過無し.
		ID3D11BlendState*	m_pNonAlpha;
	};

	HWND					m_hWnd;		//ウィンドウハンドル.

	//Dx9.
	LPDIRECT3D9				m_pD3d;		//Dx9オブジェクト.
	LPDIRECT3DDEVICE9		m_pDevice9;	//Dx9デバイスオブジェクト.

	SHADER_POINTER			m_Shader;
	enStaticModelShader		m_enShader;

	DWORD					m_dwNumMaterials;	//マテリアル数.

	//Dx11.
	ID3D11Device*			m_pDevice11;		//デバイスオブジェクト.
	ID3D11DeviceContext*	m_pDeviceContext11;	//デバイスコンテキスト.

	ID3D11Buffer*			m_pCBuffPerMesh;	//コンスタントバッファ(メッシュ).
	ID3D11Buffer*			m_pCBuffPerMaterial;//コンスタントバッファ(マテリアル).
	ID3D11Buffer*			m_pCBuffPerFrame;	//コンスタントバッファ(フレーム).

	ID3D11Buffer*			m_pVertexBuffer;	//頂点(バーテックス)バッファ.
	ID3D11Buffer**			m_ppIndexBuffer;	//インデックスバッファ.

	ID3D11SamplerState*		m_pSampleLinear;		//テクスチャのサンプラ-:テクスチャーに各種フィルタをかける.
	ID3D11SamplerState*		m_pSampleLinearDepth;	//テクスチャのサンプラ-:テクスチャーに各種フィルタをかける.

	MY_MATERIAL*			m_pMaterials;		//マテリアル構造体.
	DWORD					m_NumAttr;			//属性数.
	DWORD					m_AttrID[300];		//属性ID ※300属性まで.

	//ブレンドステート.
	BLEND_STATE				m_BlendState;

	bool m_bTexture; //テクスチャの有無.

	SHADOWMAP_REQUIRED_POINTER m_ShadowMapPointers;

	LPD3DXMESH	m_pMesh;		//メッシュオブジェクト.
	LPD3DXMESH	m_pMeshForRay;	//レイとメッシュ用.

	//位置.
	D3DXVECTOR3 m_vPos;

	//モデルの回転.
	D3DXVECTOR3 m_vRot;

	//サイズ.
	float m_fScale;

	//Dx9初期化.
	HRESULT InitDx9(HWND hWnd);
	//メッシュ読込.
	HRESULT LoadXMesh(LPSTR fileName);

	//メッシュのレンダリング.
	void RenderMesh(const D3DXMATRIX &mWorld, const D3DXMATRIX &mView, const D3DXMATRIX &mProj);

	//透過(アルファブレンド)設定の切り替え.
	void SetBlend(bool flg);
	HRESULT CreateBlendStateList();
	HRESULT ReleaseBlendStateList();
};