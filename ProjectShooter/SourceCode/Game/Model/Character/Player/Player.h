#pragma once

#include "..\\Character.h"

#include "..\\..\\Shot\\BulletManager.h"
#include "..\\..\\..\\Effekseer\\CEffects.h"

class Player : public Character
{
public:
	Player(clsD3DXSKINMESH* const pModel, BulletManager* const pBulletManager);
	virtual ~Player();

	//指定した場所の方向にモデルを向かせる.
	void DecideTargetDirection(const D3DXVECTOR3 &vPos)
	{
		m_fTargetDirection = atan2f(m_vPos.x - vPos.x, m_vPos.z - vPos.z);
	}

	SPHERE GetCollisionSphere() const
	{
		return m_Collision;
	}

	bool IsDeadFlg() const
	{
		return m_State.bDead;
	}

	int GetHpMax() const
	{
		return m_iMaxHp;
	}
	int GetHp() const
	{
		return m_State.iHp;
	}

	//レイとメッシュの衝突時.
	void RayHitToMesh(clsDX9Mesh* const pTarget);

	//Sphereとの衝突時.
	void HitToSphere(SPHERE const TargetSphere);

private:
	//Hpの最大値.
	const int m_iMaxHp = 10;

	bool m_bPushedMoveButtonFlg;

	//モデルに向かせたい方向.
	float m_fTargetDirection;

	//移動前の位置.
	D3DXVECTOR3 m_vOldPos;

	BulletManager* m_pBulletManager;

	//エフェクト関連.
	clsEffects* m_pEffect;
	//ハンドル.
	::Effekseer::Handle m_HitHandle;
	::Effekseer::Handle m_DeadHandle;
	::Effekseer::Handle m_WallHitHandle;

	void UpdateProduct();

	void Wait();

	void Move();

	void Attack();

	void Hit();

	void Dead();

	//アクションの切り替え.
	void SetAction(const enAction Action);

	//移動の入力を受け付けるかどうか.
	void AcceptedMoveButton();

	//移動制限.
	void MoveLimit();

	//攻撃の入力を受け付けるかどうか.
	void AcceptedAttackButton();
};