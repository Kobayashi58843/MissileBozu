#pragma once

#include "Bullet.h"

#include "..\\Character\\Enemy\\Enemy.h"

class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	//XV.
	void Update(const D3DXMATRIX mShotMoveMat, const D3DXVECTOR3 vShotPosition, Enemy* const pEnemy);

	//•`‰æ.
	void Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj, const D3DXVECTOR3 &vEye);

	//’e‚Ì“–‚½‚è”»’è.
	void CollisionJudgmentBullet(const SPHERE TargetSphere, clsDX9Mesh* const pTargetMesh);

	//’e‚Ì”­Ë.
	bool IsShot();

private:
	//’e‚ÌÅ‘å”.
	const int m_iBulletMax = 64;

	//’e‚Ì“–‚½‚è”»’è‚Ì‘å‚«‚³.
	const float m_fBulletRadius = 0.5f;

	//’e‚Ì‘å‚«‚³.
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