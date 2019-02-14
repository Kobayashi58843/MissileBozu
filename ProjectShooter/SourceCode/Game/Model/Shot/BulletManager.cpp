#include "BulletManager.h"
#include "..\\..\\RawInput.h"

#include "..\\..\\..\\Singleton.h"

//�e�̍ő吔.
const int BULLET_MAX = 64;

//�e�̓����蔻��̑傫��.
const float BULLET_RADIUS = 0.5f;

//�e�̑傫��.
const float BULLET_SCALE = 0.5f;

BulletManager::BulletManager()
	: m_pEnemy(nullptr)
{
	//�|�C���^�𓮓I�Ɋm��.
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

//�X�V.
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

//�`��.
void BulletManager::Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj, const D3DXVECTOR3 &vEye)
{
	for ( int i = 0; i < BULLET_MAX; i++)
	{
		m_ppBullet[i]->Render(mView, mProj, vEye);
	}
}

//�e�̓����蔻��.
void BulletManager::CollisionJudgmentBullet(const SPHERE TargetSphere, clsDX9Mesh* const pTargetMesh)
{
	for ( int i = 0; i < BULLET_MAX; i++)
	{
		//���˂���Ă��Ȃ��ꍇ�͎��̔����.
		if (!m_ppBullet[i]->IsShotFlg())
		{
			continue;
		}
		else
		{
			//�e�ƑΏۂ�Sphere���Փ˂��Ă��邩.
			if (m_ppBullet[i]->BulletCollision(TargetSphere))
			{
				//�Փ�.
				m_ppBullet[i]->BulletHitToSphere();
				m_pEnemy->HitToSphere();
			}

			//�e�̃��C�ƃ��b�V�����Փ˂��Ă��邩.
			if (m_ppBullet[i]->IsRayHit(pTargetMesh))
			{
				//�Փ�.
				m_ppBullet[i]->BulletHitToMesh();
			}
		}
	}
}

//�e�̔���.
bool BulletManager::IsShot()
{
	if (!Singleton<RawInput>().GetInstance().IsLButtonDown())
	{
		return false;
	}

	for ( int i = 0; i < BULLET_MAX; i++)
	{
		//���˂���Ă���ꍇ�͎���.
		if (m_ppBullet[i]->IsShotFlg())
		{
			continue;
		}
		else
		{
			//�e�̔��ˈʒu��������ɂ�����.
			const float fAdjustmentShotPosY = 1.0f;

			m_ppBullet[i]->BulletShot(m_mShotMoveMat, { m_vShotPosition.x, m_vShotPosition.y + fAdjustmentShotPosY, m_vShotPosition.z });
			return true;
		}
	}

	return false;
}