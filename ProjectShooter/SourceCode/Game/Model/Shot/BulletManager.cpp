#include "BulletManager.h"
#include "..\\..\\RawInput.h"

#include "..\\..\\..\\Singleton.h"

//弾の最大数.
const int BULLET_MAX = 64;

//弾の当たり判定の大きさ.
const float BULLET_RADIUS = 0.5f;

//弾の大きさ.
const float BULLET_SCALE = 0.5f;

BulletManager::BulletManager()
	: m_pEnemy(nullptr)
{
	//ポインタを動的に確保.
	m_ppBullet = new Bullet*[BULLET_MAX];


	for ( int i = 0; i < BULLET_MAX; i++)
	{
		m_ppBullet[i] = nullptr;
		m_ppBullet[i] = new Bullet(BULLET_RADIUS, BULLET_SCALE);
	}

	D3DXMatrixIdentity(&m_mShotMoveMat);
	CrearVECTOR3(m_vShotPosition);
}

BulletManager::~BulletManager()
{
	for ( int i = 0; i < BULLET_MAX; i++)
	{
		//SAFE_DELETE_ARRAY(m_ppBullet[i]);
		if (m_ppBullet[i] != nullptr)
		{
			delete m_ppBullet[i];
		}
	}
	delete[] m_ppBullet;
}

//更新.
void BulletManager::Update(const D3DXMATRIX mShotMoveMat, const D3DXVECTOR3 vShotPosition, Enemy* const pEnemy)
{
	SetShotMoveMat(mShotMoveMat);
	SetShotPosition(vShotPosition);

	m_pEnemy = pEnemy;

	for ( int i = 0; i < BULLET_MAX; i++)
	{
		m_ppBullet[i]->Update(vShotPosition);
	}
}

//描画.
void BulletManager::Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj, const D3DXVECTOR3 &vEye)
{
	for ( int i = 0; i < BULLET_MAX; i++)
	{
		m_ppBullet[i]->Render(mView, mProj, vEye);
	}
}

//弾の当たり判定.
void BulletManager::CollisionJudgmentBullet(const SPHERE TargetSphere, clsDX9Mesh* const pTargetMesh)
{
	for ( int i = 0; i < BULLET_MAX; i++)
	{
		//発射されていない場合は次の判定へ.
		if (!m_ppBullet[i]->IsShotFlg())
		{
			continue;
		}
		else
		{
			//弾と対象のSphereが衝突しているか.
			if (m_ppBullet[i]->BulletCollision(TargetSphere))
			{
				//衝突.
				m_ppBullet[i]->BulletHitToSphere();
				m_pEnemy->HitToSphere();
			}

			//弾のレイとメッシュが衝突しているか.
			if (m_ppBullet[i]->IsRayHit(pTargetMesh))
			{
				//衝突.
				m_ppBullet[i]->BulletHitToMesh();
			}
		}
	}
}

//弾の発射.
bool BulletManager::IsShot()
{
	if (!Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		return false;
	}

	for ( int i = 0; i < BULLET_MAX; i++)
	{
		//発射されている場合は次へ.
		if (m_ppBullet[i]->IsShotFlg())
		{
			continue;
		}
		else
		{
			//弾の発射位置を少し上にあげる.
			const float fAdjustmentShotPosY = 1.0f;

			m_ppBullet[i]->BulletShot(m_mShotMoveMat, { m_vShotPosition.x, m_vShotPosition.y + fAdjustmentShotPosY, m_vShotPosition.z });
			return true;
		}
	}

	return false;
}