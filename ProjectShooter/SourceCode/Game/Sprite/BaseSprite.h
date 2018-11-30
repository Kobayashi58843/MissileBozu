#pragma once

#include "..\\..\\Global.h"

#define ALIGN16 _declspec(align(16))

struct SPRITE_STATE
{
	//ファイル名とそこまでのパス.
	char sPath[MAX_PATH];
	//元画像を何分割するか.
	D3DXVECTOR2 vDivisionQuantity;
};

class BaseSprite
{
public:
	const int m_iVerticesMax = 4;

	//頂点の構造体.
	struct VERTEX
	{
		D3DXVECTOR3	vPos;	//位置.
		D3DXVECTOR2	vTex;	//テクスチャ座標.
	};

	//コンスタントバッファのアプリ側の定義(Sprite2D.hlsl).
	struct CONSTANT_BUFFER
	{
		ALIGN16 D3DXMATRIX	mW;				//ワールド行列.
		ALIGN16 float		fViewPortWidth;	//ビューポート幅.
		ALIGN16 float		fViewPortHeight;//ビューポート高さ.
		ALIGN16 float		fAlpha;			//アルファ値(透過で使用).
		ALIGN16 D3DXVECTOR2	vUV;			//UV座標.
	};

	struct SPRITE_INFO
	{
		float Width;
		float Height;
	};

	BaseSprite();			//コンストラクタ.
	virtual ~BaseSprite();	//デストラクタ.

	//作成.
	HRESULT Create(ID3D11Device* const pDevice11, ID3D11DeviceContext* const pContext11, const LPSTR FileName = nullptr);

	//解放.
	HRESULT Release();

	//描画(レンダリング)
	void Render();

protected:
	//ブレンドステート.
	struct BLEND_STATE
	{
		//透過あり.
		ID3D11BlendState*	m_pAlpha;
		//透過無し.
		ID3D11BlendState*	m_pNonAlpha;
	};

	ID3D11Device*				m_pDevice11;		//デバイスオブジェクト.
	ID3D11DeviceContext*		m_pDeviceContext11;	//デバイスコンテキスト.

	ID3D11VertexShader*			m_pVertexShader;	//頂点シェーダ.
	ID3D11InputLayout*			m_pVertexLayout;	//頂点レイアウト.
	ID3D11PixelShader*			m_pPixelShader;		//ピクセルシェーダ.
	ID3D11Buffer*				m_pConstantBuffer;	//コンスタントバッファ.

	ID3D11Buffer*				m_pVertexBuffer;	//頂点バッファ.

	ID3D11ShaderResourceView*	m_pTexture;			//テクスチャ.
	ID3D11SamplerState*			m_pSampleLinear;	//テクスチャのサンプラ-:テクスチャーに各種フィルタをかける.

	BLEND_STATE					m_BlendState;

	SPRITE_INFO					m_BaseSpriteState;	//元画像の情報.

	D3DXVECTOR3		m_vPos;			//位置.
	D3DXVECTOR3		m_vRot;			//回転.
	float			m_fScale;		//サイズ.
	float			m_fAlpha;		//アルファ値.
	bool			m_bDispFlg;		//表示フラグ.

	UINT			m_uVerMax;		//頂点の最大数.
	VERTEX			m_Vertices[4];

	//シェーダ作成.
	virtual HRESULT CreateShader() = 0;
	//モデル作成.
	HRESULT CreateModel();
	//テクスチャ作成.
	HRESULT CreateTexture(const LPSTR FileName, ID3D11ShaderResourceView** const pTex);

	//透過(アルファブレンド)設定の切り替え.
	void SetBlend(bool flg);
	HRESULT CreateBlendStateList();
	HRESULT RelaseBlendStateList();

	//板ポリ(四角形)の頂点を作成.
	virtual void CreateVertex() = 0;
	//ワールド変換.
	virtual void ConversionWorld(D3DMATRIX &mWorld) = 0;
	//シェーダのコンスタントバッファに各種データを渡す.
	virtual void InputConstantBuffer(const D3DMATRIX mWorld) = 0;
};
