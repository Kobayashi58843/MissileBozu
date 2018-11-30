#pragma once

#include <d3dx9.h>

/*================/ 共通して必要な構造体 /================*/

struct SHADER_POINTER
{
	ID3D11VertexShader*	pVertexShader;	//頂点シェーダ.
	ID3D11PixelShader*	pPixelShader;	//ピクセルシェーダ.
	ID3D11InputLayout*	pVertexLayout;	//頂点レイアウト.
};

struct SHADER_ENTRY_POINT
{
	char* pVertexShaderEntryPointName;
	char* pPixelShaderEntryPointName;
};

/*================/ スタティックメッシュに必要な構造体 /================*/

struct CBUFF_STATICMESH_FOR_SIMPLE
{
	D3DXMATRIX mW;		//ワールド(位置)座標行列.
	D3DXMATRIX mWVP;	//ワールド,ビュー,射影の合成変換行列.
};

struct CBUFF_STATICMESH_FOR_SHADOWMAP
{
	//ワールド(位置)座標行列.
	D3DXMATRIX mW;

	//( W * V * P ) 行列.
	D3DXMATRIX mWVP;

	//( W * L * P  * T ) 行列.
	D3DXMATRIX mWLPT;

	//( W * L * P ) 行列.
	D3DXMATRIX mWLP;
};

struct SUB_CBUFF_STATICMESH_FOR_SHADOWMAP
{
	//カメラ位置(視点位置).
	D3DXVECTOR4 vEye;
};

//シャドウマップに必要なポインタ.
struct SHADOWMAP_REQUIRED_POINTER
{
	//深度テクスチャ.
	ID3D11ShaderResourceView* pDepthTexture;
	//深度テクスチャのビュー行列.
	D3DXMATRIX mLightView;
	//深度テクスチャのカメラの位置.
	D3DXVECTOR3 vLightCameraPosition;
};

//頂点の構造体.
struct MESH_VERTEX
{
	D3DXVECTOR3	vPos;	//位置.
	D3DXVECTOR3	vNormal;//法線(陰影計算に必須)
	D3DXVECTOR2	vTex;	//テクスチャ座標.
};

//マテリアル構造体.
struct MY_MATERIAL
{
	D3DXVECTOR4	Ambient;	//アンビエント.
	D3DXVECTOR4	Diffuse;	//ディフューズ.
	D3DXVECTOR4	Specular;	//スペキュラ.
	CHAR szTextureName[128];//テクスチャファイル名.
	ID3D11ShaderResourceView* pTexture;//テクスチャ.
	DWORD dwNumFace;	//そのマテリアルのポリゴン数.

	//コンストラクタ.
	MY_MATERIAL()
	{
		ZeroMemory(this, sizeof(MY_MATERIAL));
	}
	//デストラクタ.
	~MY_MATERIAL()
	{
		SAFE_RELEASE(pTexture);
	}
};

/*================/ スキンメッシュに必要な構造体 /================*/

// 最大ボーン数.
#define MAX_BONES	255

struct CBUFF_SKINMESH_FOR_BONES
{
	D3DXMATRIX mBone[MAX_BONES];
	CBUFF_SKINMESH_FOR_BONES()
	{
		for (int i = 0; i<MAX_BONES; i++)
		{
			D3DXMatrixIdentity(&mBone[i]);
		}
	}
};

struct CBUFF_SKINMESH_FOR_MESH
{
	D3DXMATRIX	mW;			// ワールド行列.
	D3DXMATRIX	mWVP;		// ワールドから射影までの変換行列.
	D3DXVECTOR4 vAmbient;	// アンビエント光.
	D3DXVECTOR4 vDiffuse;	// ディフューズ色.
	D3DXVECTOR4 vSpecular;	// 鏡面反射.
};

//位置構造体.
struct TRANSFORM
{
	D3DXVECTOR3 vScale;
	float		fYaw;	//回転(Y軸)
	float		fPitch;	//回転(X軸)
	float		fRoll;	//回転(Z軸)
	D3DXVECTOR3	vPos;	//位置(X,Y,Z)
};

// オリジナル　マテリアル構造体.
struct MY_SKINMATERIAL
{
	CHAR szName[110];
	D3DXVECTOR4	Ka;	// アンビエント.
	D3DXVECTOR4	Kd;	// ディフューズ.
	D3DXVECTOR4	Ks;	// スペキュラー.
	CHAR szTextureName[512];	// テクスチャーファイル名.
	ID3D11ShaderResourceView* pTexture;
	DWORD dwNumFace;	// そのマテリアルであるポリゴン数.
	MY_SKINMATERIAL()
	{
		ZeroMemory(this, sizeof(MY_SKINMATERIAL));
	}
	~MY_SKINMATERIAL()
	{
		SAFE_RELEASE(pTexture);
	}
};

// 頂点構造体.
struct MY_SKINVERTEX
{
	D3DXVECTOR3	vPos;		// 頂点位置.
	D3DXVECTOR3	vNorm;		// 頂点法線.
	D3DXVECTOR2	vTex;		// UV座標.
	UINT bBoneIndex[4];		// ボーン 番号.
	float bBoneWeight[4];	// ボーン 重み.
	MY_SKINVERTEX()
	{
		ZeroMemory(this, sizeof(MY_SKINVERTEX));
	}
};
// ボーン構造体.
struct BONE
{
	D3DXMATRIX mBindPose;	// 初期ポーズ（ずっと変わらない）.
	D3DXMATRIX mNewPose;	// 現在のポーズ（その都度変わる）.
	DWORD dwNumChild;		// 子の数.
	int iChildIndex[50];	// 自分の子の“インデックス”50個まで.
	CHAR Name[256];

	BONE()
	{
		ZeroMemory(this, sizeof(BONE));
		D3DXMatrixIdentity(&mBindPose);
		D3DXMatrixIdentity(&mNewPose);
	}
};

// パーツメッシュ構造体.
struct SKIN_PARTS_MESH
{
	DWORD	dwNumVert;
	DWORD	dwNumFace;
	DWORD	dwNumUV;
	DWORD			dwNumMaterial;
	MY_SKINMATERIAL*	pMaterial;
	char				TextureFileName[8][256];	// テクスチャーファイル名(8枚まで).
	bool				bTex;

	ID3D11Buffer*	pVertexBuffer;
	ID3D11Buffer**	ppIndexBuffer;

	// ボーン.
	int		iNumBone;
	BONE*	pBoneArray;

	bool	bEnableBones;	// ボーンの有無フラグ.

	SKIN_PARTS_MESH()
	{
		ZeroMemory(this, sizeof(SKIN_PARTS_MESH));
		pVertexBuffer = NULL;
		ppIndexBuffer = NULL;
		pBoneArray = NULL;
	}
};

// デバイス関係受取用構造体.
struct CD3DXSKINMESH_INIT
{
	HWND hWnd;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
};

// 派生フレーム構造体.
//	それぞれのメッシュ用の最終ワールド行列を追加する.
struct MYFRAME : public D3DXFRAME
{
	D3DXMATRIX CombinedTransformationMatrix;
	SKIN_PARTS_MESH* pPartsMesh;

	MYFRAME()
	{
		ZeroMemory(this, sizeof(MYFRAME));
	}
};

// 派生メッシュコンテナー構造体.
//	コンテナーがテクスチャを複数持てるようにポインターのポインターを追加する
struct MYMESHCONTAINER : public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9*  ppTextures;
	DWORD dwWeight;				// 重みの個数（重みとは頂点への影響。）.
	DWORD dwBoneNum;			// ボーンの数.
	LPD3DXBUFFER pBoneBuffer;	// ボーン・テーブル.
	D3DXMATRIX** ppBoneMatrix;	// 全てのボーンのワールド行列の先頭ポインタ.
	D3DXMATRIX* pBoneOffsetMatrices;// フレームとしてのボーンのワールド行列のポインタ.
};
// Xファイル内のアニメーション階層を読み下してくれるクラスを派生させる.
//	ID3DXAllocateHierarchyは派生すること想定して設計されている.
class MY_HIERARCHY : public ID3DXAllocateHierarchy
{
public:
	MY_HIERARCHY(){}
	STDMETHOD(CreateFrame)(THIS_ LPCSTR, LPD3DXFRAME *);
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR, CONST D3DXMESHDATA*, CONST D3DXMATERIAL*,
		CONST D3DXEFFECTINSTANCE*, DWORD, CONST DWORD *, LPD3DXSKININFO, LPD3DXMESHCONTAINER *);
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER);
};