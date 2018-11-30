#pragma once

#include "..\\..\\Object.h"

#include "..\\..\\Effekseer\\CEffects.h"

class Bullet : public Object
{
public:
	Bullet(const float fCollisionRadius, const float fScale);
	~Bullet();

	//更新.
	void Update(const D3DXVECTOR3 vPlayerPos);

	//描画.
	void Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj, const D3DXVECTOR3 &vEye);

	//弾のSphereとの当たり判定.
	bool BulletCollision(SPHERE const TargetSphere);

	//弾とSphereの衝突時.
	void BulletHitToSphere();

	//弾とメッシュの衝突時.
	void BulletHitToMesh();

	//弾の発射.
	void BulletShot(const D3DXMATRIX mMoveMat, const D3DXVECTOR3 vShotPosition);

	bool IsShotFlg() const
	{
		return m_bShotFlg;
	}

private:
#if _DEBUG
	//弾のモデル.
	clsDX9Mesh* m_pModel;
#endif

	//エフェクト関連.
	clsEffects* m_pEffect;
	//ハンドル.
	::Effekseer::Handle m_MissileHandle;
	::Effekseer::Handle m_ExplosionHandle;
	
	//発射中.
	bool m_bShotFlg;

	//弾の飛距離.
	float m_fFlyableDistance;

	//移動方向.
	D3DXMATRIX m_mMoveMat;

	//音量調節に使うプレイヤーの位置.
	D3DXVECTOR3 m_vPlayerPosition;

	//位置,回転,サイズ等をモデルに適応.
	void UpdateState();

	//弾の移動.
	void Move();

	//弾の消滅時.
	void WhenBulletDisappearance();
};