#include "Bullet.h"

#include "..\\..\\..\\Singleton.h"

#include "..\\..\\Sound\\SoundManager.h"
#include "..\\ModelResource\\ModelResource.h"

//弾の速度.
const float SPEED = 0.4f;

//弾の飛距離.
const float FLYABLE_DISTANCE = 100.0f;

//弾の初期位置.
const D3DXVECTOR3 POSITION = { 0.0f, -100.0f, 0.0f };

//表示サイズ.
const float EXPLOSION_SCALE = 1.0f;

//減衰値.
const int ATTENUATION_QUANTITY = 15;

//エフェクト関係.
//ボリューム最大最小.
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

//更新.
void Bullet::Update(const D3DXVECTOR3 vPlayerPos)
{
	//弾が発射中でなければ抜ける.
	if (!m_bShotFlg)
	{
		return;
	}

	m_vPlayerPosition = vPlayerPos;

	//弾の移動.
	Move();
}

//描画.
void Bullet::Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj, const D3DXVECTOR3 &vEye)
{
	//位置,回転,サイズ等をモデルに適応.
	UpdateState();

#if _DEBUG
	m_pModel->Render(mView, mProj);
#endif
}

//弾のSphereとの当たり判定.
bool Bullet::BulletCollision(SPHERE const TargetSphere)
{
	//弾のSphereとターゲットのSphereが当たっているか.
	if (SphereCollision(m_Collision, TargetSphere))
	{
		//衝突.
		return true;
	}

	//衝突していない.
	return false;
}

//弾とSphereの衝突時.
void Bullet::BulletHitToSphere()
{
	WhenBulletDisappearance();
}

//弾とメッシュの衝突時.
void Bullet::BulletHitToMesh()
{
	WhenBulletDisappearance();
}

//弾の発射.
void Bullet::BulletShot(const D3DXMATRIX mMoveMat, const D3DXVECTOR3 vShotPosition)
{
	m_bShotFlg = true;

	m_mMoveMat = mMoveMat;

	m_vPos = vShotPosition;

	m_fFlyableDistance = 0.0f;

	m_MissileHandle = m_pEffect->Play(m_vPos, clsEffects::enEfcType_Missile);

	//エフェクトの大きさ.
	float fMissileScale = 0.4f;
	m_pEffect->SetScale(m_MissileHandle, { fMissileScale, fMissileScale, fMissileScale });
}

//位置,回転,サイズ等をモデルに適応.
void Bullet::UpdateState()
{
#if _DEBUG
	m_pModel->SetPos(m_vPos);

	m_pModel->SetRot(m_vRot);

	m_pModel->SetScale(m_fScale);
#endif
	//当たり判定用Sphereの位置を合わせる.
	D3DXVECTOR3 vSpherePos = m_vPos;
	//Sphereの場所を上にずらす.
	vSpherePos.y += m_Collision.fRadius;
	m_Collision.vCenter = vSpherePos;

	//エフェクト.
	m_pEffect->SetLocation(m_MissileHandle, m_vPos);

	D3DXVECTOR3 vAxis = { 0.0f, 0.0f, 1.0f };
	D3DXVec3TransformCoord(&vAxis, &vAxis, &m_mMoveMat);
	vAxis += m_vPos;

	m_vRot.y = atan2f(vAxis.x - m_vPos.x, vAxis.z - m_vPos.z);

	m_pEffect->SetRotation(m_MissileHandle, m_vRot);
}

//弾の移動.
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

//弾の衝突時.
void Bullet::WhenBulletDisappearance()
{
	//減衰量.
	int iExplosionVolume = MAX_EXPLOSION_VOLUME;

	D3DXVECTOR3 vPosition = m_vPos - m_vPlayerPosition;
	float fLength = D3DXVec3Length(&vPosition);
	iExplosionVolume -= ATTENUATION_QUANTITY * static_cast<int>(fLength);

	//最小値より音が小さい場合は最小値にする.
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