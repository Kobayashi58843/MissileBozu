#pragma once

#include "..\\..\\..\\Shader\\ShaderGathering.h"

#include "..\\..\\..\\Shader\\NoTexture.h"
#include "..\\..\\..\\Shader\\Simple.h"
#include "..\\..\\..\\Shader\\DepthTexture.h"

// 最大アニメーションセット数.
#define MAX_ANIM_SET	100

//==================================================================================================
//
//	パーサークラス.
//
//==================================================================================================
class D3DXPARSER
{
public:
	MY_HIERARCHY cHierarchy;
	MY_HIERARCHY* m_pHierarchy;
	LPD3DXFRAME m_pFrameRoot;

	LPD3DXANIMATIONCONTROLLER m_pAnimController;//デフォルトで一つ.
	LPD3DXANIMATIONSET m_pAnimSet[MAX_ANIM_SET];

	HRESULT LoadMeshFromX(LPDIRECT3DDEVICE9, LPSTR);
	HRESULT AllocateBoneMatrix(LPD3DXMESHCONTAINER);
	HRESULT AllocateAllBoneMatrices(LPD3DXFRAME);
	VOID UpdateFrameMatrices(LPD3DXFRAME, LPD3DXMATRIX);


	int GetNumVertices(MYMESHCONTAINER* pContainer);
	int GetNumFaces(MYMESHCONTAINER* pContainer);
	int GetNumMaterials(MYMESHCONTAINER* pContainer);
	int GetNumUVs(MYMESHCONTAINER* pContainer);
	int GetNumBones(MYMESHCONTAINER* pContainer);
	int GetNumBoneVertices(MYMESHCONTAINER* pContainer, int iBoneIndex);
	DWORD GetBoneVerticesIndices(MYMESHCONTAINER* pContainer, int iBoneIndex, int iIndexInGroup);
	double GetBoneVerticesWeights(MYMESHCONTAINER* pContainer, int iBoneIndex, int iIndexInGroup);
	D3DXVECTOR3 GetVertexCoord(MYMESHCONTAINER* pContainer, DWORD iIndex);
	D3DXVECTOR3 GetNormal(MYMESHCONTAINER* pContainer, DWORD iIndex);
	D3DXVECTOR2 GetUV(MYMESHCONTAINER* pContainer, DWORD iIndex);
	int GetIndex(MYMESHCONTAINER* pContainer, DWORD iIndex);
	D3DXVECTOR4 GetAmbient(MYMESHCONTAINER* pContainer, int iIndex);
	D3DXVECTOR4 GetDiffuse(MYMESHCONTAINER* pContainer, int iIndex);
	D3DXVECTOR4 GetSpecular(MYMESHCONTAINER* pContainer, int iIndex);
	CHAR* GetTexturePath(MYMESHCONTAINER* pContainer, int index);
	float GetSpecularPower(MYMESHCONTAINER* pContainer, int iIndex);
	int GeFaceMaterialIndex(MYMESHCONTAINER* pContainer, int iFaceIndex);
	int GetFaceVertexIndex(MYMESHCONTAINER* pContainer, int iFaceIndex, int iIndexInFace);
	D3DXMATRIX GetBindPose(MYMESHCONTAINER* pContainer, int iBoneIndex);
	D3DXMATRIX GetNewPose(MYMESHCONTAINER* pContainer, int iBoneIndex);
	CHAR* GetBoneName(MYMESHCONTAINER* pContainer, int iBoneIndex);

	// メッシュコンテナを取得する.
	LPD3DXMESHCONTAINER GetMeshContainer(LPD3DXFRAME pFrame);

	// アニメーションセットの切り替え.
	void ChangeAnimSet(int index, LPD3DXANIMATIONCONTROLLER pAC = NULL);
	// アニメーションセットの切り替え(開始フレーム指定可能版).
	void ChangeAnimSet_StartPos(int index, double dStartFramePos, LPD3DXANIMATIONCONTROLLER pAC = NULL);

	// アニメーション停止時間を取得.
	double GetAnimPeriod(int index);
	// アニメーション数を取得.
	int GetAnimMax(LPD3DXANIMATIONCONTROLLER pAC = NULL);

	// 指定したボーン情報(座標・行列)を取得する関数.
	bool GetMatrixFromBone(char* sBoneName, D3DXMATRIX* pOutMat);
	bool GetPosFromBone(char* sBoneName, D3DXVECTOR3* pOutPos);

	// メッシュ解放.
	HRESULT ReleaseMesh(LPD3DXFRAME pFrame);

	// 一括解放処理.
	HRESULT Release();
};

//==================================================================================================
//
//	スキンメッシュクラス.
//
//==================================================================================================
class clsD3DXSKINMESH
	: private NoTexture
	, private Simple
	, private DepthTexture
{
public:
	TRANSFORM m_Trans;

	//パーサークラスからアニメーションコントローラーを取得する.
	LPD3DXANIMATIONCONTROLLER GetAnimController()
	{
		return m_pD3dxMesh->m_pAnimController;
	}

	D3DXMATRIX m_mWorld;
	D3DXMATRIX m_mRotation;

	D3DXMATRIX m_mView;
	D3DXMATRIX m_mProj;

	//アニメーション速度.
	double m_dAnimSpeed;

	// メソッド.
	clsD3DXSKINMESH();
	~clsD3DXSKINMESH();

	HRESULT Init(CD3DXSKINMESH_INIT* si);
	// Xファイルからスキンメッシュを作成する.
	HRESULT CreateFromX(CHAR* szFileName);
	// 描画関数.
	void Render(const D3DXMATRIX& mView, const D3DXMATRIX& mProj, LPD3DXANIMATIONCONTROLLER pAC = NULL);
	// 解放関数.
	HRESULT Release();

	double GetAnimSpeed()				{ return m_dAnimSpeed; }
	void SetAnimSpeed(double dSpeed)	{ m_dAnimSpeed = dSpeed; }

	// アニメーションセットの切り替え.
	void ChangeAnimSet(int index, LPD3DXANIMATIONCONTROLLER pAC = NULL);
	// アニメーションセットの切り替え(開始フレーム指定可能版).
	void ChangeAnimSet_StartPos(int index, double dStartFramePos, LPD3DXANIMATIONCONTROLLER pAC = NULL);

	// アニメーション停止時間を取得.
	double GetAnimPeriod(int index);
	// アニメーション数を取得.
	int GetAnimMax(LPD3DXANIMATIONCONTROLLER pAC = NULL);

	// 指定したボーン情報(座標・行列)を取得する関数.
	bool GetMatrixFromBone(char* sBoneName, D3DXMATRIX* pOutMat);
	bool GetPosFromBone(char* sBoneName, D3DXVECTOR3* pOutPos);
	bool GetDeviaPosFromBone(char* sBoneName, D3DXVECTOR3* pOutPos, D3DXVECTOR3 vDeviation = D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//シェーダをセット.
	HRESULT SetShader(const enSkinModelShader enShader);

	enSkinModelShader GetShader() const
	{
		return m_enShader;
	}

private:
	HWND m_hWnd;
	// Dx9.
	LPDIRECT3D9 m_pD3d9;
	LPDIRECT3DDEVICE9 m_pDevice9;
	// Dx11.
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	ID3D11SamplerState*		m_pSampleLinear;

	SHADER_POINTER			m_Shader;
	enSkinModelShader		m_enShader;

	ID3D11Buffer* m_pConstantBufferBone;

	ID3D11Buffer* m_pConstantBuffer;

	// メッシュ.
	D3DXPARSER* m_pD3dxMesh;

	// XFileのパス.
	CHAR	m_FilePath[256];

	// アニメーションフレーム.
	int		m_iFrame;

	HRESULT CreateDeviceDx9(HWND hWnd);
	HRESULT CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);
	void RecursiveSetNewPoseMatrices(BONE* pBone, D3DXMATRIX* pmParent);

	// 全てのメッシュを作成する.
	void BuildAllMesh(D3DXFRAME* pFrame);

	// メッシュを作成する.
	HRESULT CreateAppMeshFromD3DXMesh(LPD3DXFRAME pFrame);

	// Xファイルからスキン関連の情報を読み出す関数.
	HRESULT ReadSkinInfo(MYMESHCONTAINER* pContainer, MY_SKINVERTEX* pvVB, SKIN_PARTS_MESH* pParts);

	// ボーンを次のポーズ位置にセットする関数.
	void SetNewPoseMatrices(SKIN_PARTS_MESH* pParts, int frame, MYMESHCONTAINER* pContainer);
	// 次の(現在の)ポーズ行列を返す関数.
	D3DXMATRIX GetCurrentPoseMatrix(SKIN_PARTS_MESH* pParts, int index);

	// フレーム描画.
	void DrawFrame(LPD3DXFRAME pFrame);
	// パーツ描画.
	void DrawPartsMesh(SKIN_PARTS_MESH* p, D3DXMATRIX World, MYMESHCONTAINER* pContainer);
	void DrawPartsMeshStatic(SKIN_PARTS_MESH* pMesh, D3DXMATRIX World, MYMESHCONTAINER* pContainer);

	// 全てのメッシュを削除.
	void DestroyAllMesh(D3DXFRAME* pFrame);
	HRESULT DestroyAppMeshFromD3DXMesh(LPD3DXFRAME p);
};