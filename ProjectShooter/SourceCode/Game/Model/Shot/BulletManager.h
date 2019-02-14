#pragma once

#include "Bullet.h"

#include "..\\Character\\Enemy\\Enemy.h"

class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	//çXêV.
	void Update(const D3DXMATRIX mShotMoveMat, const D3DXVECTOR3 vShotPosition, Enemy* const pEnemy);

	//ï`âÊ.
	void Render(const D3DXMATRIX &mView, const D3DXMATRIX &mProj, const D3DXVECTOR3 &vEye);

	//íeÇÃìñÇΩÇËîªíË.
	void CollisionJudgmentBullet(const SPHERE TargetSphere, clsDX9Mesh* const pTargetMesh);

	//íeÇÃî≠éÀ.
	bool IsShot();

private:
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