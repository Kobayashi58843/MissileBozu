#include "Bullet.h"

#include "..\\..\\..\\Singleton.h"

#include "..\\..\\Sound\\SoundManager.h"
#include "..\\ModelResource\\ModelResource.h"

//�e�̑��x.
const float SPEED = 0.4f;

//�e�̔򋗗�.
const float FLYABLE_DISTANCE = 100.0f;

//�e�̏����ʒu.
const D3DXVECTOR3 POSITION = { 0.0f, -100.0f, 0.0f };

//�\���T�C�Y.
const float EXPLOSION_SCALE = 1.0f;

//�����l.
const int ATTENUATION_QUANTITY = 15;

//�G�t�F�N�g�֌W.
//�{�����[���ő�ŏ�.
const int MAX_EXPLOSION_VOLUME = 1000;
const int MIN_EXPLOSION_VOLUME = 50;

Bullet::Bullet(const float fCollisionRadius, const float fScale)
	: m_bShotFlg(false)
	, m_fFlyableDistance(0.0f)
{
#if _DEBUG
	m_pModel = Singleton<ModelResource>().GetInstance().GetStaticModels(ModelResource::enStaticModel_Shpere);
#endif

	m_vPos = POSITION;

	CrearVECTOR3(m_vRot);

	m_fScale = fScale;

	D3DXMatrixIdentity(&m_mMoveMat);

	CrearVECTOR3(m_vPlayerPosition);

	m_Collision.fRadius = fCollisionRadius;

	m_pEffect = clsEffects::GetInstance();
}

Bullet::~Bullet()
{
#if _DEBUG
	m_pModel = nullptr;
#endif
	m_pEffect->StopAll();
}

//�X�V.
void Bullet::Update(const D3DXVECTOR3 vPlayerPos)
{
	//�e�����˒��łȂ���Δ�����.
	if (!m_bShotFlg)
	{
		return;
	}

	m_vPlayerPosition = vPlayerPos;

	//�e�̈ړ�.
	Move();
}

//�`��.
void Bullet::Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj, const D3DXVECTOR3 &vEye)
{
	//�ʒu,��],�T�C�Y�������f���ɓK��.
	UpdateState();

#if _DEBUG
	m_pModel->Render(mView, mProj);
#endif
}

//�e��Sphere�Ƃ̓����蔻��.
bool Bullet::BulletCollision(SPHERE const TargetSphere)
{
	//�e��Sphere�ƃ^�[�Q�b�g��Sphere���������Ă��邩.
	if (SphereCollision(m_Collision, TargetSphere))
	{
		//�Փ�.
		return true;
	}

	//�Փ˂��Ă��Ȃ�.
	return false;
}

//�e��Sphere�̏Փˎ�.
void Bullet::BulletHitToSphere()
{
	WhenBulletDisappearance();
}

//�e�ƃ��b�V���̏Փˎ�.
void Bullet::BulletHitToMesh()
{
	WhenBulletDisappearance();
}

//�e�̔���.
void Bullet::BulletShot(const D3DXMATRIX mMoveMat, const D3DXVECTOR3 vShotPosition)
{
	m_bShotFlg = true;

	m_mMoveMat = mMoveMat;

	m_vPos = vShotPosition;

	m_fFlyableDistance = 0.0f;

	m_MissileHandle = m_pEffect->Play(m_vPos, clsEffects::enEfcType_Missile);

	//�G�t�F�N�g�̑傫��.
	float fMissileScale = 0.4f;
	m_pEffect->SetScale(m_MissileHandle, { fMissileScale, fMissileScale, fMissileScale });
}

//�ʒu,��],�T�C�Y�������f���ɓK��.
void Bullet::UpdateState()
{
#if _DEBUG
	m_pModel->SetPos(m_vPos);

	m_pModel->SetRot(m_vRot);

	m_pModel->SetScale(m_fScale);
#endif
	//�����蔻��pSphere�̈ʒu�����킹��.
	D3DXVECTOR3 vSpherePos = m_vPos;
	//Sphere�̏ꏊ����ɂ��炷.
	vSpherePos.y += m_Collision.fRadius;
	m_Collision.vCenter = vSpherePos;

	//�G�t�F�N�g.
	m_pEffect->SetLocation(m_MissileHandle, m_vPos);

	D3DXVECTOR3 vAxis = { 0.0f, 0.0f, 1.0f };
	D3DXVec3TransformCoord(&vAxis, &vAxis, &m_mMoveMat);
	vAxis += m_vPos;

	m_vRot.y = atan2f(vAxis.x - m_vPos.x, vAxis.z - m_vPos.z);

	m_pEffect->SetRotation(m_MissileHandle, m_vRot);
}

//�e�̈ړ�.
void Bullet::Move()
{
	D3DXVECTOR3 vAxisZ = { 0.0f, 0.0f, 1.0f };

	D3DXVec3TransformCoord(&vAxisZ, &vAxisZ, &m_mMoveMat);

	m_vPos += vAxisZ * SPEED;
	
	m_fFlyableDistance += SPEED;
	if (m_fFlyableDistance >= FLYABLE_DISTANCE)
	{
		WhenBulletDisappearance();
	}
}

//�e�̏Փˎ�.
void Bullet::WhenBulletDisappearance()
{
	//������.
	int iExplosionVolume = MAX_EXPLOSION_VOLUME;

	D3DXVECTOR3 vPosition = m_vPos - m_vPlayerPosition;
	float fLength = D3DXVec3Length(&vPosition);
	iExplosionVolume -= ATTENUATION_QUANTITY * static_cast<int>(fLength);

	//�ŏ��l��艹���������ꍇ�͍ŏ��l�ɂ���.
	if (MIN_EXPLOSION_VOLUME > iExplosionVolume)
	{
		iExplosionVolume = MIN_EXPLOSION_VOLUME;
	}

	Singleton<SoundManager>().GetInstance().PlaySE(SoundManager::enSE_Explosion, iExplosionVolume);

	m_pEffect->Stop(m_MissileHandle);
	m_ExplosionHandle = m_pEffect->Play(m_vPos, clsEffects::enEfcType_Explosion);

	m_pEffect->SetScale(m_ExplosionHandle, { EXPLOSION_SCALE, EXPLOSION_SCALE, EXPLOSION_SCALE });

	m_bShotFlg = false;
	m_vPos = POSITION;
}