#include "MaskSprite.h"

#include "..\\Direct3D.h"

//シェーダファイル名(ディレクトリも含む)
const char SHADER_NAME[] = "Shader\\MaskSprite.hlsl";

MaskSprite::MaskSprite(const float fDivisionQuantityWidth, const float fDivisionQuantityHeight)
	: m_vDivisionQuantity({ fDivisionQuantityWidth, fDivisionQuantityHeight })
	, m_pMaskTexture_SRV(nullptr)
{
	CrearVECTOR2(m_vPattern);
	CrearVECTOR2(m_vUV);
}

MaskSprite::~MaskSprite()
{
	m_pMaskTexture_SRV = nullptr;
}

//シェーダを作成する.
HRESULT MaskSprite::CreateShader()
{
	ID3DBlob* pCompiledShader = NULL;
	ID3DBlob* pErrors = NULL;
	UINT uCompileFlag = 0;

#ifdef _DEBUG
	uCompileFlag
		= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif//#ifdef _DEBUG

	//HLSLからバーテックスシェーダのブロブを作成.
	if (FAILED(
		D3DX11CompileFromFile(
		SHADER_NAME,		//シェーダファイル名(HLSLファイル)
		NULL,				//マクロ定義の配列へのポインタ(未使用)
		NULL,				//インクルードファイルを扱うインターフェイスへのポインタ(未使用)
		"VS",				//シェーダエントリーポイント関数の名前.
		"vs_5_0",			//シェーダのモデルを指定する文字列(プロファイル)
		uCompileFlag,		//シェーダコンパイルフラグ.
		0,					//エフェクトコンパイルフラグ(未使用)
		NULL,				//スレッドポンプインターフェイスへのポインタ(未使用)
		&pCompiledShader,	//ブロブを格納するメモリへのポインタ.
		&pErrors,			//エラーと警告一覧を格納するメモリへのポインタ.
		NULL)))				//戻り値へのポインタ(未使用)
	{
		MessageBox(NULL, "hlsl読込失敗", "エラー", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//上記で作成したブロブから「バーテックスシェーダ」を作成.
	if (FAILED(m_pDevice11->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
		NULL, &m_pVertexShader)))//(out)バーテックスシェーダ.
	{
		MessageBox(NULL, "vs作成失敗", "エラー", MB_OK);
		return E_FAIL;
	}

	//頂点インプットレイアウトを定義.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",				//位置.
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,//DXGIのフォーマット(32bit float型*3)
			0,
			0,							//データの開始位置.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD",		//テクスチャ位置.
			0,
			DXGI_FORMAT_R32G32_FLOAT,	//DXGIのフォーマット(32bit float型*2)
			0,
			12,							//テクスチャデータの開始位置.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	//頂点インプットレイアウトの配列要素数を算出.
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウトの作成.
	if (FAILED(m_pDevice11->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
		&m_pVertexLayout)))//(out)頂点インプットレイアウト.
	{
		MessageBox(NULL, "頂点インプットレイアウト作成失敗", "エラー", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	//HLSLからピクセルシェーダのブロブを作成.
	if (FAILED(
		D3DX11CompileFromFile(
		SHADER_NAME,	//シェーダファイル名(HLSLファイル)
		NULL, NULL,
		"PS_Main",		//シェーダエントリーポイント関数の名前.
		"ps_5_0",		//シェーダのモデルを指定する文字列(プロファイル)
		uCompileFlag,	//シェーダコンパイルフラグ.
		0, NULL,
		&pCompiledShader,//ブロブを格納するメモリへのポインタ.
		&pErrors,
		NULL)))
	{
		MessageBox(NULL, "hlsl(ps)読込失敗", "エラー", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//上記で作成したブロブから「ピクセルシェーダ」を作成.
	if (FAILED(m_pDevice11->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
		NULL, &m_pPixelShader)))//(out)ピクセルシェーダ.
	{
		MessageBox(NULL, "ps作成失敗", "エラー", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);//ブロブ解放.

	//コンスタント(定数)バッファ作成 シェーダに特定の数値を送るバッファ.
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//コンスタントバッファを指定.
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);//コンスタントバッファのサイズ.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//書き込みでアクセス.
	cb.MiscFlags = 0;//その他のフラグ(未使用)
	cb.StructureByteStride = 0;//構造体のサイズ(未使用)
	cb.Usage = D3D11_USAGE_DYNAMIC;//使用方法:直接書き込み.

	//コンスタントバッファ作成.
	if (FAILED(m_pDevice11->CreateBuffer(&cb, NULL, &m_pConstantBuffer)))
	{
		MessageBox(NULL, "コンスタントバッファ作成失敗", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//板ポリ(四角形)の頂点を作成.
void MaskSprite::CreateVertex()
{
	assert(m_vDivisionQuantity.x != 0.0f);
	assert(m_vDivisionQuantity.y != 0.0f);

	//表示スプライト幅、高さ.
	float fW = (m_BaseSpriteState.Width / m_vDivisionQuantity.x) * m_fScale;
	float fH = (m_BaseSpriteState.Height / m_vDivisionQuantity.y) * m_fScale;

	float fU = (m_BaseSpriteState.Width / m_vDivisionQuantity.x) / m_BaseSpriteState.Width;
	float fV = (m_BaseSpriteState.Height / m_vDivisionQuantity.y) / m_BaseSpriteState.Height;

	//板ポリ(四角形)の頂点を作成.
	VERTEX vertices[] =
	{
		D3DXVECTOR3(-fW / 2, fH / 2, 0.0f), D3DXVECTOR2(0.0f, fV),
		D3DXVECTOR3(-fW / 2, -fH / 2, 0.0f), D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR3(fW / 2, fH / 2, 0.0f), D3DXVECTOR2(fU, fV),
		D3DXVECTOR3(fW / 2, -fH / 2, 0.0f), D3DXVECTOR2(fU, 0.0f)
	};
	//最大要素数を算出する.
	m_uVerMax = sizeof(vertices) / sizeof(vertices[0]);

	for (unsigned int i = 0; i < m_uVerMax; i++)
	{
		m_Vertices[i] = vertices[i];
	}
}

//ワールド変換.
void MaskSprite::ConversionWorld(D3DMATRIX &mWorld)
{
	D3DXMATRIX mTrans;
	D3DXMATRIX mYaw, mPitch, mRoll;
	D3DXMATRIX mScale;

	//サイズ.
	D3DXMatrixScaling(&mScale, m_fScale, m_fScale, m_fScale);

	//回転.
	D3DXMatrixRotationX(&mPitch, m_vRot.x);
	D3DXMatrixRotationY(&mYaw, m_vRot.y);
	D3DXMatrixRotationZ(&mRoll, m_vRot.z);

	//平行移動.
	D3DXMatrixTranslation(&mTrans, m_vPos.x, m_vPos.y, m_vPos.z);

	mWorld = mScale * mPitch * mYaw * mRoll * mTrans;
}

//シェーダのコンスタントバッファに各種データを渡す.
void MaskSprite::InputConstantBuffer(const D3DMATRIX mWorld)
{
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER cd;	//コンスタントバッファ.
	//バッファ内のデータの書き方開始時にmap.
	if (SUCCEEDED(m_pDeviceContext11->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//ワールド行列を渡す.
		D3DXMATRIX m = mWorld;
		D3DXMatrixTranspose(&m, &m);//行列を転置する.					
		//行列の計算方法がDirectXとGPUで異なる為.
		cd.mW = m;

		//ビューポートの幅、高さを渡す.
		cd.fViewPortWidth = WINDOW_WIDTH;
		cd.fViewPortHeight = WINDOW_HEIGHT;

		//アルファ値を渡す.
		cd.fAlpha = m_fAlpha;

		//UV座標を渡す.
		cd.vUV.x = (((m_BaseSpriteState.Width / m_vDivisionQuantity.x) / m_BaseSpriteState.Width) * m_vPattern.x)
			+ (m_vUV.x / m_BaseSpriteState.Width);

		cd.vUV.y = (((m_BaseSpriteState.Height / m_vDivisionQuantity.y) / m_BaseSpriteState.Height) * m_vPattern.y)
			+ (m_vUV.y / m_BaseSpriteState.Height);

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cd), sizeof(cd));

		m_pDeviceContext11->Unmap(m_pConstantBuffer, 0);
	}

	//このコンスタントバッファをどのシェーダで使うか？.
	m_pDeviceContext11->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext11->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	//マスク処理に使うテクスチャをシェーダに渡す.
	m_pDeviceContext11->PSSetShaderResources(1, 1, &m_pMaskTexture_SRV);
}
