#include "BaseSprite.h"

//シェーダファイル名(ディレクトリも含む)
const char SHADER_NAME[] = "Shader\\Sprite2D.hlsl";

//コンストラクタ.
BaseSprite::BaseSprite()
	: m_pDevice11(nullptr)
	, m_pDeviceContext11(nullptr)
	, m_pVertexShader(nullptr)
	, m_pVertexLayout(nullptr)
	, m_pPixelShader(nullptr)
	, m_pConstantBuffer(nullptr)
	, m_pVertexBuffer(nullptr)
	, m_pTexture(nullptr)
	, m_pSampleLinear(nullptr)
	, m_BlendState()
	, m_fScale(1.0f)
	, m_fAlpha(1.0f)
	, m_bDispFlg(true)
	, m_uVerMax(0)
	, m_BaseSpriteState()
{
	CrearVECTOR3(m_vPos);
	CrearVECTOR3(m_vRot);

	for (int i = 0; i < m_iVerticesMax; i++)
	{
		CrearVECTOR3(m_Vertices[i].vPos);
		CrearVECTOR2(m_Vertices[i].vTex);
	}
}

//デストラクタ.
BaseSprite::~BaseSprite()
{
	Release();

	m_pDeviceContext11 = nullptr;	//デバイスコンテキスト.
	m_pDevice11 = nullptr;		//デバイスオブジェクト.
}

//作成.
HRESULT BaseSprite::Create(ID3D11Device* const pDevice11, ID3D11DeviceContext* const pContext11, const LPSTR FileName)
{
	m_pDevice11 = pDevice11;
	m_pDeviceContext11 = pContext11;

	if (FileName != nullptr)
	{
		D3DXIMAGE_INFO imageInfo;

		//元画像の情報.
		D3DXGetImageInfoFromFile(FileName, &imageInfo);
		m_BaseSpriteState.Width = static_cast<float>(imageInfo.Width);
		m_BaseSpriteState.Height = static_cast<float>(imageInfo.Height);

		if (FAILED(CreateTexture(FileName, &m_pTexture)))
		{
			return E_FAIL;
		}
	}

	if (FAILED(CreateShader()))
	{
		return E_FAIL;
	}

	if (FAILED(CreateModel()))
	{
		return E_FAIL;
	}

	if (FAILED(CreateBlendStateList()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//解放.
HRESULT BaseSprite::Release()
{
	RelaseBlendStateList();

	SAFE_RELEASE(m_pSampleLinear);	//テクスチャのサンプラ-:テクスチャーに各種フィルタをかける.
	SAFE_RELEASE(m_pTexture);		//テクスチャ.
	SAFE_RELEASE(m_pVertexBuffer);	//頂点バッファ.
	SAFE_RELEASE(m_pConstantBuffer);//コンスタントバッファ.
	SAFE_RELEASE(m_pPixelShader);	//ピクセルシェーダ.
	SAFE_RELEASE(m_pVertexLayout);	//頂点レイアウト.
	SAFE_RELEASE(m_pVertexShader);	//頂点シェーダ.

	return S_OK;
}

//板ポリゴンを画面にレンダリング.
void BaseSprite::Render()
{
	if (!m_bDispFlg)
	{
		return;
	}

	//ワールド行列.
	D3DXMATRIX mWorldMat;

	//初期化:単位行列作成.
	D3DXMatrixIdentity(&mWorldMat);

	//ワールド変換(表示位置を設定する).
	ConversionWorld(mWorldMat);

	//使用するシェーダの登録.
	m_pDeviceContext11->VSSetShader(m_pVertexShader, NULL, 0);
	m_pDeviceContext11->PSSetShader(m_pPixelShader, NULL, 0);

	//シェーダのコンスタントバッファに各種データを渡す.
	InputConstantBuffer(mWorldMat);

	//頂点バッファをセット.
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	m_pDeviceContext11->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//頂点インプットレイアウトをセット.
	m_pDeviceContext11->IASetInputLayout(m_pVertexLayout);

	//プリミティブ・トポロジーをセット.
	m_pDeviceContext11->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//テクスチャをシェーダに渡す.
	m_pDeviceContext11->PSSetSamplers(0, 1, &m_pSampleLinear);	//サンプラ-をセット.
	m_pDeviceContext11->PSSetShaderResources(0, 1, &m_pTexture);//テクスチャをシェーダに渡す.

	//アルファブレンド用ブレンドステート作成＆設定.
	SetBlend(true);

	//プリミティブをレンダリング.
	m_pDeviceContext11->Draw(4, 0);

	//アルファブレンドを無効にする.
	SetBlend(false);
}

//モデル作成.
HRESULT BaseSprite::CreateModel()
{
	//板ポリ(四角形)の頂点を作成.
	CreateVertex();

	//バッファ構造体.
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;				//使用方(デフォルト)
	bd.ByteWidth = sizeof(VERTEX) * m_uVerMax;	//頂点サイズ(頂点×4)
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//頂点バッファとして扱う.
	bd.CPUAccessFlags = 0;		//CPUからはアクセスしない.
	bd.MiscFlags = 0;			//その他のフラグ(未使用)
	bd.StructureByteStride = 0;	//構造体のサイズ(未使用)

	//サブリソースデータ構造体.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_Vertices;//板ポリの頂点をセット.

	//頂点バッファの作成.
	if (FAILED(m_pDevice11->CreateBuffer(&bd, &InitData, &m_pVertexBuffer)))
	{
		MessageBox(NULL, "頂点バッファ作成失敗", "エラー", MB_OK);
		return E_FAIL;
	}

	//頂点バッファをセット.
	UINT stride = sizeof(VERTEX);//データ間隔.
	UINT offset = 0;
	m_pDeviceContext11->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//テクスチャ用のサンプラー構造体.
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(SamDesc));
	//リニアフィルタ-(線形補間)
	//	POINT:高速だが粗い.
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//ラッピングモード.
	//	WRAP  :繰り返し
	//	MIRROR:反転繰り返し.
	//	CLAMP :端の模様を引き伸ばす.
	//	BORDER:別途境界色を決める.
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	//サンプラー作成.
	if (FAILED(m_pDevice11->CreateSamplerState(&SamDesc, &m_pSampleLinear)))//(out)サンプラー.
	{
		MessageBox(NULL, "サンプラ作成失敗", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//テクスチャ作成.
HRESULT BaseSprite::CreateTexture(const LPSTR FileName, ID3D11ShaderResourceView** const pTex)
{
	//テクスチャ作成.
	if (FAILED(D3DX11CreateShaderResourceViewFromFile(m_pDevice11, FileName, NULL, NULL, pTex, NULL)))
	{
		ERR_MSG("BaseSprite::CreateTexture()", "エラー");
		return E_FAIL;
	}

	return S_OK;
}

//透過(アルファブレンド)設定の切り替え.
void BaseSprite::SetBlend(bool flg)
{
	ID3D11BlendState *pBlendState = nullptr;
	if (flg == true)
	{
		pBlendState = m_BlendState.m_pAlpha;
	}
	else
	{
		pBlendState = m_BlendState.m_pNonAlpha;
	}

	//ブレンドステートの設定.
	UINT mask = 0xffffffff;	//マスク値.
	m_pDeviceContext11->OMSetBlendState(pBlendState, NULL, mask);
}


HRESULT BaseSprite::CreateBlendStateList()
{
	//アルファブレンド用ブレンドステート構造体.
	//pngファイル内にアルファ情報があるので、
	//透過するようにブレンドステートを設定する.
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));//初期化.

	blendDesc.IndependentBlendEnable
		= false;//false:RenderTarget[0]のメンバーのみが使用する.
	//true :RenderTarget[0～7]が使用できる.
	//      (レンダーターゲット毎に独立したブレンド処理)
	blendDesc.AlphaToCoverageEnable
		= false;//true :アルファトゥカバレッジを使用する.
	blendDesc.RenderTarget[0].BlendEnable
		= true;	//true :アルファブレンドを使用する.
	blendDesc.RenderTarget[0].SrcBlend	//元素材に対する設定.
		= D3D11_BLEND_SRC_ALPHA;		//	アルファブレンドを指定.
	blendDesc.RenderTarget[0].DestBlend	//重ねる素材に対する設定.
		= D3D11_BLEND_INV_SRC_ALPHA;	//	アルファブレンドの反転を指定.

	blendDesc.RenderTarget[0].BlendOp	//ブレンドオプション.
		= D3D11_BLEND_OP_ADD;			//	ADD:加算合成.

	blendDesc.RenderTarget[0].SrcBlendAlpha	//元素材のアルファに対する設定.
		= D3D11_BLEND_ONE;					//	そのまま使用.
	blendDesc.RenderTarget[0].DestBlendAlpha//重ねる素材のアルファに対する設定.
		= D3D11_BLEND_ZERO;					//	何もしない.

	blendDesc.RenderTarget[0].BlendOpAlpha	//アルファのブレンドオプション.
		= D3D11_BLEND_OP_ADD;				//	ADD:加算合成.

	blendDesc.RenderTarget[0].RenderTargetWriteMask	//ピクセル毎の書き込みマスク.
		= D3D11_COLOR_WRITE_ENABLE_ALL;				//	全ての成分(RGBA)へのデータの格納を許可する.

	//ブレンドステート作成.透過あり.
	if (FAILED(m_pDevice11->CreateBlendState(&blendDesc, &m_BlendState.m_pAlpha)))
	{
		MessageBox(NULL, "ブレンドステート作成失敗", "エラー", MB_OK);
		return E_FAIL;
	}

	//透過無し.
	blendDesc.RenderTarget[0].BlendEnable = false;
	if (FAILED(m_pDevice11->CreateBlendState(&blendDesc, &m_BlendState.m_pNonAlpha)))
	{
		MessageBox(NULL, "ブレンドステート作成失敗", "エラー", MB_OK);

		//作成失敗した場合作成された透過ありのブレンドステートを消す.
		if (m_BlendState.m_pAlpha != nullptr)
		{
			SAFE_RELEASE(m_BlendState.m_pAlpha);
		}

		return E_FAIL;
	}

	return S_OK;
}

HRESULT BaseSprite::RelaseBlendStateList()
{
	SAFE_RELEASE(m_BlendState.m_pNonAlpha);
	SAFE_RELEASE(m_BlendState.m_pAlpha);

	return S_OK;
}