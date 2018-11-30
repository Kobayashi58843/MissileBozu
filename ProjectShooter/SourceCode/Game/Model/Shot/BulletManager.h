#pragma once

#include "Bullet.h"

#include "..\\Character\\Enemy\\Enemy.h"

class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	//更新.
	void Update(const D3DXMATRIX mShotMoveMat, const D3DXVECTOR3 vShotPosition, Enemy* const pEnemy);

	//描画.
	void Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj, const D3DXVECTOR3 &vEye);

	//弾の当たり判定.
	void CollisionJudgmentBullet(const SPHERE TargetSphere, clsDX9Mesh* const pTargetMesh);

	//弾の発射.
	bool IsShot();

private:
	//弾の最大数.
	const int m_iBulletMax = 64;

	//弾の当たり判定の大きさ.
	const float m_fBulletRadius = 0.5f;

	//弾の大きさ.
	const float m_fBulletScale = 0.5f;

	Bullet** m_ppBullet;

	D3DXMATRIX m_mShotMoveMat;
	D3DXVECTOR3 m_vShotPosition;

	Enemy* m_pEnemy;

	void SetShotMoveMat(const D3DXMATRIX mMoveMat)
	{
		m_mShotMoveMat = mMoveMat;
	}

	void SetShotPosition(const D3DXVECTOR3 vShotPosition)
	{
		m_vShotPosition = vShotPosition;
	}

};