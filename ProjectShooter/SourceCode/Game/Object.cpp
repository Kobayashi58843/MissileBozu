#include "Object.h"

Object::Object()
	: m_fScale(1.0f)
{
	m_Collision.fRadius = 0.0f;
	CrearVECTOR3(m_Collision.vCenter);

	CrearVECTOR3(m_vPos);
	CrearVECTOR3(m_vRot);
	CrearVECTOR3(m_vUpperIntersect);
	CrearVECTOR3(m_vUnderIntersect);
}

Object::~Object()
{
}

//レイと対象のメッシュが衝突しているか.
bool Object::IsRayHit(clsDX9Mesh* const pTarget)
{
	////軸ベクトルは垂直で下向き.
	//D3DXVECTOR3 vAxis = { 0.0f, -1.0f, 0.0f };

	//CrearVECTOR3(m_vIntersect);

	////距離.
	//float fDistance = 0.0f;

	////レイとメッシュの当たり判定.
	//bool bHit = Intersect(vAxis, pTarget, &fDistance, &m_vIntersect);

	//if (bHit)
	//{
	//	D3DXVECTOR3 vLength = m_Collision.vCenter - m_vIntersect;
	//	float fLength = D3DXVec3Length(&vLength);

	//	if (fabs(fLength) > m_Collision.fRadius)
	//	{
	//		bHit = false;
	//	}
	//}

	//下方向の交点座標.
	D3DXVECTOR3 vAxis = { 0.0f, -1.0f, 0.0f };
	float fDistance = 0.0f;
	Intersect(vAxis, pTarget, &fDistance, &m_vUnderIntersect);

	//上方向の交点座標.
	vAxis = { 0.0f, 1.0f, 0.0f };
	fDistance = 0.0f;
	Intersect(vAxis, pTarget, &fDistance, &m_vUpperIntersect);

	bool bHit = false;

	//当たり判定.
	HitCheckMeshByRay(pTarget, &bHit);

	//衝突.
	return bHit;
}

//レイとメッシュの当たり判定.
bool Object::Intersect(const D3DXVECTOR3 vAxis,
	clsDX9Mesh* const pTarget,	//対象の物体.
	float* pDistance,			//(out)距離.
	D3DXVECTOR3* pIntersect)	//(out)交差座標.
{
	D3DXMATRIXA16 matRot;//回転行列.

	//回転行列を計算.
	D3DXMatrixRotationY(&matRot, m_vRot.y);

	//軸ベクトルを用意.
	D3DXVECTOR3 vecAxis = vAxis;
	//Z軸ベクトルそのものを現在の回転状態により変換する.
	D3DXVec3TransformCoord(&vecAxis, &vecAxis, &matRot);

	D3DXVECTOR3 vecStart, vecEnd, vecDistance;
	//レイの開始終了位置をAttackerと合わせる.
	vecStart = vecEnd = m_Collision.vCenter;
	//Attackerの座標に回転行列を合成する.
	vecEnd += vecAxis * 1.0f;

	//対象が動いてる物体でも、対象のworld行列の、逆並列を用いれば正しくレイが当たる.
	
	//拡縮.
	D3DXMATRIX	mScale;
	D3DXMatrixIdentity(&mScale);
	
	float fScale = pTarget->GetScale();
	D3DXMatrixScaling(&mScale, fScale, fScale, fScale);
	
	//回転.
	D3DXMATRIX	mYaw, mPitch, mRoll;
	D3DXMatrixIdentity(&mYaw);
	D3DXMatrixIdentity(&mPitch);
	D3DXMatrixIdentity(&mRoll);

	D3DXVECTOR3 vRot = pTarget->GetRot();
	D3DXMatrixRotationY(&mYaw, vRot.y);		//Y軸回転.
	D3DXMatrixRotationX(&mPitch, vRot.x);	//X軸回転.
	D3DXMatrixRotationZ(&mRoll, vRot.z);	//Z軸回転.

	//移動.
	D3DXMATRIX mTrans;
	D3DXMatrixIdentity(&mTrans);

	D3DXVECTOR3 vPos = pTarget->GetPos();
	D3DXMatrixTranslation(&mTrans, vPos.x, vPos.y, vPos.z);

	//合成(拡縮×回転×移動)
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);

	mWorld = mScale * mYaw * mPitch * mRoll * mTrans;

	//逆並列を求める.
	D3DXMatrixInverse(&mWorld, NULL, &mWorld);

	D3DXVec3TransformCoord(&vecStart, &vecStart, &mWorld);
	D3DXVec3TransformCoord(&vecEnd, &vecEnd, &mWorld);

	//距離を求める.
	vecDistance = vecEnd - vecStart;

	BOOL bHit = false;

	DWORD dwIndex = 0;		//インデックス番号.
	D3DXVECTOR3 vVertex[3];	//頂点座標.
	FLOAT U = 0, V = 0;		//(out)重心ヒット座標.

	//メッシュとレイの交差を調べる.
	D3DXIntersect(
		pTarget->GetMesh(),	//対象メッシュ.
		&vecStart,			//開始位置.
		&vecDistance,		//レイの距離.
		&bHit,				//(out)判定結果.
		&dwIndex,			//(out)bHitがTrueの時、レイの始点に。最も近くの面のインデックス値へのポインタ.
		&U, &V,				//(out)重心ヒット座標.
		pDistance,			//ターゲットとの距離.
		NULL, NULL);

	if (bHit)
	{
		//命中したとき.
		FindVecticesOnPoly(pTarget->GetMeshForRay(), dwIndex, vVertex);

		//中心座標から交差点を算出.
		//ローカル交点pは、v0 + U*(v1-v0) + V*(v2-v0)で求まる.
		*pIntersect =
			vVertex[0]
			+ U * (vVertex[1] - vVertex[0])
			+ V * (vVertex[2] - vVertex[0]);

		return true;//命中している.
	}

	return false;//命中していない.
}

//交差位置のポリゴンの頂点を見つける.
//※頂点座標はローカル座標.
HRESULT Object::FindVecticesOnPoly(const LPD3DXMESH pTarget, const DWORD dwPolyIndex, D3DXVECTOR3* pVecVertices)
{
	//頂点ごとのバイト数を取得.
	DWORD dwStride = pTarget->GetNumBytesPerVertex();
	//頂点数を取得.
	DWORD dwVertexAm = pTarget->GetNumVertices();
	//面数を取得.
	DWORD dwPolyAmt = pTarget->GetNumFaces();

	WORD* pwPoly = NULL;

	//インデックスバッファをロック(読み込みモード)
	pTarget->LockIndexBuffer(D3DLOCK_READONLY, (VOID**)&pwPoly);
	BYTE* pbVertices = NULL;			//頂点(バイト数)
	FLOAT* pfVertices = NULL;			//頂点(float型)
	LPDIRECT3DVERTEXBUFFER9 VB = NULL;	//頂点バッファ.
	pTarget->GetVertexBuffer(&VB);		//頂点情報の取得.

	//頂点バッファのロック.
	if (SUCCEEDED(VB->Lock(0, 0, (VOID**)&pbVertices, 0)))
	{
		for (int i = 0; i < 3; i++)
		{
			//ポリゴンの頂点の[i]つ目を取得.
			pfVertices
			= (FLOAT*)&pbVertices[dwStride*pwPoly[dwPolyIndex * 3 + i]];
			pVecVertices[i].x = pfVertices[0];
			pVecVertices[i].y = pfVertices[1];
			pVecVertices[i].z = pfVertices[2];
		}

		pTarget->UnlockIndexBuffer();//ロック解除.
		VB->Unlock();//ロック解除.
	}
	VB->Release();

	return S_OK;
}

//当たり判定間連.
void Object::HitCheckMeshByRay(clsDX9Mesh* const pWall, bool* bHit)
{
	const int iDirectionMax = 10;
	D3DXVECTOR3 vAxis[iDirectionMax];

	FLOAT fDistance;//距離.
	float fDis, fYaw;//距離と回転.

	//レイの向きによる当たる壁までの距離を求める.
	vAxis[0] = { 0.0f, 0.0f, 1.0f };	//軸ベクトル前.
	vAxis[1] = { 1.0f, 0.0f, 1.0f };	//軸ベクトル右前.
	vAxis[2] = { 1.0f, 0.0f, 0.0f };	//軸ベクトル右.
	vAxis[3] = { 1.0f, 0.0f, -1.0f };	//軸ベクトル右後ろ.
	vAxis[4] = { 0.0f, 0.0f, -1.0f };	//軸ベクトル後ろ.
	vAxis[5] = { -1.0f, 0.0f, -1.0f };	//軸ベクトル左後ろ.
	vAxis[6] = { -1.0f, 0.0f, 0.0f };	//軸ベクトル左.
	vAxis[7] = { -1.0f, 0.0f, 1.0f };	//軸ベクトル左前.

	vAxis[8] = { 0.0f, 1.0f, 0.0f };
	vAxis[9] = { 0.0f, -1.0f, 0.0f };

	fYaw = fabs(m_vRot.y);//fabs:絶対値(float版)
	dirOverGuard(&fYaw);//0～2πの間に収める.

	D3DXMATRIX mYaw;
	D3DXMatrixRotationY(&mYaw, m_vRot.y);//Y軸回転.

	for (int i = 0; i < iDirectionMax; i++)
	{
		D3DXVECTOR3 vIntersect;
		Intersect(vAxis[i], pWall, &fDistance, &vIntersect);
		fDis = fDistance;

		if (fDis < m_Collision.fRadius && fDis > 0.01f)
		{
			D3DXVec3TransformCoord(&vAxis[i], &vAxis[i], &mYaw);
			m_vPos -= vAxis[i] * (m_Collision.fRadius - fDis);
			*bHit = true;
			return;
		}
	}
}

//回転値調整.
void Object::dirOverGuard(float* fYaw)
{
	if (*fYaw > D3DX_PI * 2.0f)
	{
		//1周以上している.
		*fYaw -= static_cast<float>D3DX_PI * 2.0f;//2π(360°)分を引く.
	
		//再帰関数.
		if (*fYaw > D3DX_PI*2.0f)
		{
			dirOverGuard(fYaw);
		}
	}
}

//Sphere対Sphereの当たり判定.
bool Object::SphereCollision(SPHERE const pAttacker, SPHERE const pTarget)
{
	//2つの物体の中心間の距離を求める.
	D3DXVECTOR3 vLength = pTarget.vCenter - pAttacker.vCenter;
	//長さに変換する.
	float fLength = D3DXVec3Length(&vLength);

	//2物体間の距離が、2物体の半径を足したもの.
	//小さいということは、スフィア同士が重なっている.
	//(衝突している)ということ.
	if (fLength <= pAttacker.fRadius + pTarget.fRadius)
	{
		//衝突.
		return true;
	}

	//衝突していない.
	return false;
}

//モデルを徐々に目的の方向へ回転.
void Object::SpinModel(double dDestinationYaw, const float fTurnSpeed)
{
	if (dDestinationYaw - m_vRot.y > D3DXToRadian(180))
	{
		dDestinationYaw -= D3DXToRadian(360);
	}
	else if (dDestinationYaw - m_vRot.y < -D3DXToRadian(180))
	{
		dDestinationYaw += D3DXToRadian(360);
	}

	if (m_vRot.y > D3DXToRadian(180))
	{
		m_vRot.y -= static_cast<float>D3DXToRadian(360);
	}
	else if (m_vRot.y < -D3DXToRadian(180))
	{
		m_vRot.y += static_cast<float>D3DXToRadian(360);
	}

	//角度が近づく.
	if (fabs(dDestinationYaw - m_vRot.y) > fTurnSpeed)
	{
		if (m_vRot.y < dDestinationYaw)
		{
			m_vRot.y += fTurnSpeed;
		}
		else if (m_vRot.y > dDestinationYaw)
		{
			m_vRot.y -= fTurnSpeed;
		}
	}
	else
	{
		m_vRot.y = static_cast<float>(dDestinationYaw);
	}
}