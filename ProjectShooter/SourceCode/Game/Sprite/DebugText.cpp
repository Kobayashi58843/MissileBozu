#include "DebugText.h"

//シェーダファイル名(パスも含む).
const char SHADER_NAME[] = "Shader\\DebugText.hlsl";

//コンストラクタ.
DebugText::DebugText()
	: m_pDevice11(nullptr)
	, m_pDeviceContext11(nullptr)
	, m_pVertexShader(nullptr)
	, m_pVertexLayout(nullptr)
	, m_pPixelShader(nullptr)
	, m_pConstantBuffer(nullptr)
	, m_pAsciiTexture(nullptr)
	, m_pSampleLinear(nullptr)
	, m_BlendState()
	, m_dwWindowWidth(0)
	, m_dwWindowHeight(0)
	, m_fScale(1.0f)
	, m_fAlpha(0.0f)
{
	const int iVertexBufferMax = 100;
	for (int i = 0; i < iVertexBufferMax; i++)
	{
		m_pVertexBuffer[i] = nullptr;
	}

	const int iKerningMax = 100;
	for (int i = 0; i < iKerningMax; i++)
	{
		m_fKerning[i] = 0.0f;
	}

	CrearVECTOR4(m_vColor);

	D3DXMatrixIdentity(&m_mView);
	D3DXMatrixIdentity(&m_mProj);
}

//デストラクタ.
DebugText::~DebugText()
{
	Release();

	m_pDeviceContext11 = nullptr;
	m_pDevice11 = nullptr;
}

//作成.
HRESULT DebugText::Create(
	ID3D11DeviceContext* pContext,
	DWORD dwWidth, DWORD dwHeight,
	float fSize, D3DXVECTOR4 vColor)
{
	m_fAlpha = vColor.w;
	m_vColor = vColor;
	m_fScale = fSize / 25.0f;

	//95文字分繰り返し.
	for (int i = 0; i<95; i++){
		m_fKerning[i] = 10.0f;
	}
	//デバイスコンテキストをコピー.
	m_pDeviceContext11 = pContext;
	m_pDeviceContext11->GetDevice(&m_pDevice11);

	//windowサイズ.
	m_dwWindowWidth = dwWidth;
	m_dwWindowHeight = dwHeight;

	//フォント毎にクアッド(矩形)作成
	float left = 0.0f, top = 0.0f, right = 0.0f, bottom = 0.0f;
	int cnt = 0;
	//2重ループで1文字ずつ指定する.
	for (int k = 0; k<10; k++)
	{
		for (int i = 0; i<10; i++)
		{
			left = k * WDIMENSION;
			top = i * DIMENSION;
			right = left + m_fKerning[cnt];
			bottom = top + DIMENSION;

			left /= TEX_DIMENSION;
			top /= TEX_DIMENSION;
			right /= TEX_DIMENSION;
			bottom /= TEX_DIMENSION;

			TEXT_VERTEX vertices[] =
			{
				D3DXVECTOR3(0.0f, DIMENSION, 0.0f), D3DXVECTOR2(left, bottom),	//頂点1.
				D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(left, top),		//頂点2.
				D3DXVECTOR3(WDIMENSION, DIMENSION, 0.0f), D3DXVECTOR2(right, bottom),	//頂点3.
				D3DXVECTOR3(WDIMENSION, 0.0, 0.0f), D3DXVECTOR2(right, top)		//頂点4.
			};
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(TEXT_VERTEX) * 4;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = vertices;
			if (FAILED(m_pDevice11->CreateBuffer(
				&bd, &InitData, &m_pVertexBuffer[cnt])))
			{
				MessageBox(NULL,
					"頂点バッファ作成失敗(DebugText:Init)",
					"error", MB_OK);
				return E_FAIL;
			}
			cnt++;
		}
	}

	//テクスチャ用サンプラ作成.
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	//テクスチャフィルタと貼り付け方の指定.
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	if (FAILED(m_pDevice11->CreateSamplerState(
		&SamDesc, &m_pSampleLinear)))
	{
		MessageBox(NULL,
			"サンプラ作成失敗(DebugText:Init)",
			"", MB_OK);
		return E_FAIL;
	}

	//フォントのテクスチャ作成.
	if (FAILED(
		D3DX11CreateShaderResourceViewFromFile(
		m_pDevice11,
		"Data\\DebugText\\ascii0.png",
		NULL, NULL,
		&m_pAsciiTexture, NULL)))
	{
		MessageBox(NULL,
			"フォントテクスチャ作成失敗(DebugText:Init)",
			"error", MB_OK);
		return E_FAIL;
	}

	//=====================================
	//	hlslファイル読込.
	//=====================================
	ID3DBlob *pCompileShader = NULL;
	ID3DBlob *pErrors = NULL;
	if (FAILED(D3DX11CompileFromFile(
		SHADER_NAME, NULL, NULL,
		"VS", "vs_5_0", 0, 0,
		NULL, &pCompileShader,
		&pErrors, NULL)))
	{
		MessageBox(NULL,
			"hlsl読込失敗(VS)",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(m_pDevice11->CreateVertexShader(
		pCompileShader->GetBufferPointer(),
		pCompileShader->GetBufferSize(),
		NULL, &m_pVertexShader)))
	{
		SAFE_RELEASE(pCompileShader);
		MessageBox(NULL,
			"バーテックスシェーダ作成失敗",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}

	//頂点インプットレイアウトを定義.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウト作成.
	if (FAILED(
		m_pDevice11->CreateInputLayout(
		layout, numElements,
		pCompileShader->GetBufferPointer(),
		pCompileShader->GetBufferSize(),
		&m_pVertexLayout)))
	{
		MessageBox(NULL,
			"頂点インプットレイアウト作成失敗",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}

	//ピクセルシェーダ作成.
	if (FAILED(D3DX11CompileFromFile(
		SHADER_NAME, NULL, NULL,
		"PS", "ps_5_0", 0, 0,
		NULL, &pCompileShader,
		&pErrors, NULL)))
	{
		MessageBox(NULL,
			"hlsl読込失敗(PS)",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(m_pDevice11->CreatePixelShader(
		pCompileShader->GetBufferPointer(),
		pCompileShader->GetBufferSize(),
		NULL, &m_pPixelShader)))
	{
		SAFE_RELEASE(pCompileShader);
		MessageBox(NULL,
			"ピクセルシェーダ作成失敗",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompileShader);

	//コンスタントバッファ作成.
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(TEXT_CONSTANT_BUFFER);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(m_pDevice11->CreateBuffer(
		&cb, NULL, &m_pConstantBuffer)))
	{
		MessageBox(NULL,
			"コンスタントバッファ作成",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}

	//ブレンドステート作成.
	CreateBlendStateList();

	return S_OK;
}

//解放.
void DebugText::Release()
{
	ReleaseBlendStateList();

	SAFE_RELEASE(m_pSampleLinear);//テクスチャのサンプラ.
	SAFE_RELEASE(m_pAsciiTexture);//テクスチャ.

	//頂点バッファ(100個分).
	for (int i = 0; i < MAX_VERTEX_BUFFER; i++)
	{
		SAFE_RELEASE(m_pVertexBuffer[i]);
	}

	SAFE_RELEASE(m_pConstantBuffer);//コンスタントバッファ.
	SAFE_RELEASE(m_pPixelShader);	//ピクセルシェーダ.
	SAFE_RELEASE(m_pVertexLayout);	//頂点レイアウト.
	SAFE_RELEASE(m_pVertexShader);	//頂点シェーダ.
}

//レンダリング関数.
void DebugText::Render(char* text, int x, int y)
{
	//ビュートランスフォーム.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -1.0f);
	D3DXVECTOR3 vLook(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(
		&m_mView, &vEye, &vLook, &vUp);

#if 0
	//プロジェクショントランスフォーム(固定)
	D3DMATRIX mOtho = {
		2.0f / static_cast<float>(m_dwWindowWidth), 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f / static_cast<float>(m_dwWindowHeight), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};
#else
	D3DXMATRIX mOtho;
	D3DXMatrixIdentity(&mOtho);//単位行列.
	mOtho._11 = 2.0f / static_cast<float>(m_dwWindowWidth);
	mOtho._22 = -2.0f / static_cast<float>(m_dwWindowHeight);
	mOtho._41 = -1.0f;
	mOtho._42 = 1.0f;
#endif
	m_mProj = mOtho;

	//プリミティブ・トポロジー.
	m_pDeviceContext11->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//頂点インプットレイアウトをセット.
	m_pDeviceContext11->IASetInputLayout(
		m_pVertexLayout);

	//使用するシェーダの登録.
	m_pDeviceContext11->VSSetShader(
		m_pVertexShader, NULL, 0);
	m_pDeviceContext11->PSSetShader(
		m_pPixelShader, NULL, 0);

	//このコンスタントバッファを使うシェーダの登録.
	m_pDeviceContext11->VSSetConstantBuffers(
		0, 1, &m_pConstantBuffer);
	m_pDeviceContext11->PSSetConstantBuffers(
		0, 1, &m_pConstantBuffer);

	//テクスチャをシェーダに渡す.
	m_pDeviceContext11->PSSetSamplers(
		0, 1, &m_pSampleLinear);
	m_pDeviceContext11->PSSetShaderResources(
		0, 1, &m_pAsciiTexture);

	SetBlend(true);

	//文字数分ループ.
	for (unsigned int i = 0; i<strlen(text); i++)
	{
		char font = text[i];
		int index = font - 32;	//フォントインデックス作成.

		//フォントレンダリング.
		RenderFont(index, x, y);

		x += static_cast<int>(m_fKerning[index]);
	}

	SetBlend(false);
}

//フォントレンダリング関数.
void DebugText::RenderFont(int FontIndex, int x, int y)
{
	//ワールド変換.
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixTranslation(
		&mWorld, static_cast<float>(x), static_cast<float>(y),
		-100.0f);	//Z(-100)で手前表示可能.

	//シェーダのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE	pData;
	TEXT_CONSTANT_BUFFER		cb;
	if (SUCCEEDED(m_pDeviceContext11->Map(
		m_pConstantBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//ワールド,ビュー,プロジェクションの合成行列を渡す.
		D3DXMATRIX m = mWorld * m_mView * m_mProj;
		D3DXMatrixTranspose(&m, &m);
		cb.mWVP = m;
		//カラーを渡す.
		cb.vColor = m_vColor;
		//透明度を渡す.
		cb.fAlpha.x = m_fAlpha;

		memcpy_s(pData.pData, pData.RowPitch,
			(void*)(&cb), sizeof(cb));
		m_pDeviceContext11->Unmap(
			m_pConstantBuffer, 0);
	}
	//バーテックスバッファをセット.
	UINT stride = sizeof(TEXT_VERTEX);
	UINT offset = 0;
	m_pDeviceContext11->IASetVertexBuffers(
		0, 1, &m_pVertexBuffer[FontIndex],
		&stride, &offset);

	//描画.
	UINT ColoKey = 0xffffffff;
	m_pDeviceContext11->OMSetBlendState(m_BlendState.m_pAlpha, NULL, ColoKey);
	m_pDeviceContext11->Draw(4, 0);
	m_pDeviceContext11->OMSetBlendState(0, NULL, ColoKey);
}

//透過(アルファブレンド)設定の切り替え.
void DebugText::SetBlend(bool flg)
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

HRESULT DebugText::CreateBlendStateList()
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
		= true;//true :アルファトゥカバレッジを使用する.
	blendDesc.RenderTarget[0].BlendEnable
		= false;	//true :アルファブレンドを使用する.
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

HRESULT DebugText::ReleaseBlendStateList()
{
	SAFE_RELEASE(m_BlendState.m_pNonAlpha);
	SAFE_RELEASE(m_BlendState.m_pAlpha);

	return S_OK;
}