#include "BulletManager.h"
#include "..\\..\\RawInput.h"

#include "..\\..\\..\\Singleton.h"

BulletManager::BulletManager()
	: m_pEnemy(nullptr)
{
	//ƒ|ƒCƒ“ƒ^‚ð“®“I‚ÉŠm•Û.
	m_ppBullet = new Bullet*[m_iBulletMax];


	for ( int i = 0; i < m_iBulletMax; i++)
	{
		m_ppBullet[i] = nullptr;
		m_ppBullet[i] = new Bullet(m_fBulletRadius, m_fBulletScale);
	}

	D3DXMatrixIdentity(&m_mShotMoveMat);
	CrearVECTOR3(m_vShotPosition);
}

BulletManager::~BulletManager()
{
	for ( int i = 0; i < m_iBulletMax; i++)
	{
		//SAFE_DELETE_ARRAY(m_ppBullet[i]);
		if (m_ppBullet[i] != nullptr)
		{
			delete m_ppBullet[i];
		}
	}
	delete[] m_ppBullet;
}

//XV.
void BulletManager::Update(const D3DXMATRIX mShotMoveMat, const D3DXVECTOR3 vShotPosition, Enemy* const pEnemy)
{
	SetShotMoveMat(mShotMoveMat);
	SetShotPosition(vShotPosition);

	m_pEnemy = pEnemy;

	for ( int i = 0; i < m_iBulletMax; i++)
	{
		m_ppBullet[i]->Update(vShotPosition);
	}
}

//•`‰æ.
void BulletManager::Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj, const D3DXVECTOR3 &vEye)
{
	for ( int i = 0; i < m_iBulletMax; i++)
	{
		m_ppBullet[i]->Render(mView, mProj, vEye);
	}
}

//’e‚Ì“–‚½‚è”»’è.
void BulletManager::CollisionJudgmentBullet(const SPHERE TargetSphere, clsDX9Mesh* const pTargetMesh)
{
	for ( int i = 0; i < m_iBulletMax; i++)
	{
		//”­ŽË‚³‚ê‚Ä‚¢‚È‚¢ê‡‚ÍŽŸ‚Ì”»’è‚Ö.
		if (!m_ppBullet[i]->IsShotFlg())
		{
			continue;
		}
		else
		{
			//’e‚Æ‘ÎÛ‚ÌSphere‚ªÕ“Ë‚µ‚Ä‚¢‚é‚©.
			if (m_ppBullet[i]->BulletCollision(TargetSphere))
			{
				//Õ“Ë.
				m_ppBullet[i]->BulletHitToSphere();
				m_pEnemy->HitToSphere();
			}

			//’e‚ÌƒŒƒC‚ÆƒƒbƒVƒ…‚ªÕ“Ë‚µ‚Ä‚¢‚é‚©.
			if (m_ppBullet[i]->IsRayHit(pTargetMesh))
			{
				//Õ“Ë.
				m_ppBullet[i]->BulletHitToMesh();
			}
		}
	}
}

//’e‚Ì”­ŽË.
bool BulletManager::IsShot()
{
	if (!Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		return false;
	}

	for ( int i = 0; i < m_iBulletMax; i++)
	{
		//”­ŽË‚³‚ê‚Ä‚¢‚éê‡‚ÍŽŸ‚Ö.
		if (m_ppBullet[i]->IsShotFlg())
		{
			continue;
		}
		else
		{
			//’e‚Ì”­ŽËˆÊ’u‚ð­‚µã‚É‚ ‚°‚é.
			const float fAdjustmentShotPosY = 1.0f;

			m_ppBullet[i]->BulletShot(m_mShotMoveMat, { m_vShotPosition.x, m_vShotPosition.y + fAdjustmentShotPosY, m_vShotPosition.z });
			return true;
		}
	}

	return false;
}