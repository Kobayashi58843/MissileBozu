#pragma once

#include "..\\Object.h"

class Camera : public Object
{
public:
	Camera(const float fWindowWidth, const float fWindowHeight);
	~Camera();

	//毎フレーム更新するもの.
	void Update();

	//注視空間でのカメラ位置.
	void SetFocusingSpacePos(const D3DXVECTOR3 vPos)
	{
		m_vFocusingSpacePos = vPos;
	}
	D3DXVECTOR3 GetFocusingSpacePos() const
	{
		return m_vFocusingSpacePos;
	}

	//カメラのワールド座標.
	D3DXVECTOR3 GetWorldPos() const
	{
		return m_vWorldPos;
	}

	//ビュー行列.
	D3DXMATRIX GetView() const
	{
		return m_mView;
	}

	//カメラ姿勢行列.
	D3DXMATRIX GetCameraPose() const
	{
		return m_mCameraPose;
	}

	//プロジェクション行列.
	D3DXMATRIX GetProjection() const
	{
		return m_mProjection;
	}

	//注視点.
	void SetLookAt(const D3DXVECTOR3 vLook)
	{
		m_vLookAt = vLook;
	}
	D3DXVECTOR3 GetLookAt() const
	{
		return m_vLookAt;
	}

	//移動.
	void MoveLeft()
	{
		m_vCameraMoveDirDif.x = -1.0f;
	}
	void MoveRight()
	{
		m_vCameraMoveDirDif.x = 1.0f;
	}
	void MoveUp()
	{
		m_vCameraMoveDirDif.y = 1.0f;
	}
	void MoveDown()
	{
		m_vCameraMoveDirDif.y = -1.0f;
	}

	//回転方向ベクトルの差分.
	void SetRotDif(const D3DXVECTOR3 vRotDif)
	{
		m_vCamRotDif = vRotDif;
	}

	//カメラ回転時に動く角度の単位(度数法で角度を入れてください).
	void SetRotPerFrame(const float fDegree)
	{
		double dMoveAngle = D3DXToRadian(fDegree);
		m_fCameraAngleUnit = static_cast<float>(dMoveAngle);
	}

	//カメラの奥行きを差し引き.
	void SetOffsetZ(const float fOffsetZ)
	{
		m_fOffsetZ = fOffsetZ;
	}

	//横にずらす値.
	void SetDisplaceHorizontally(const float fDisplace)
	{
		m_fDisplaceHorizontally = fDisplace;
	}

	//注視位置からの距離.
	float GetDistance() const
	{
		return D3DXVec3Length(&m_vFocusingSpacePos);
	}

	//レイとメッシュの衝突時.
	void RayHitToMesh(clsDX9Mesh* const pTarget);

private:
	//ウィンドウ幅.
	float m_fWindowWidth;
	//ウィンドウ高さ.
	float m_fWindowHeight;

	//注視点空間でのカメラの位置.
	D3DXVECTOR3 m_vFocusingSpacePos;
	//カメラのワールド座標.
	D3DXVECTOR3 m_vWorldPos;
	//ビュー行列.
	D3DXMATRIX	m_mView;
	//カメラ姿勢行列.
	D3DXMATRIX m_mCameraPose;
	//プロジェクション行列.
	D3DXMATRIX m_mProjection;

	//注視点.
	D3DXVECTOR3	m_vLookAt;
	//カメラの移動方向ベクトルの差分.
	D3DXVECTOR3 m_vCameraMoveDirDif;
	//カメラ軸での回転の差分.
	D3DXVECTOR3 m_vCamRotDif;
	//移動時回転単位（ラジアン）.
	float m_fCameraAngleUnit;
	//奥行きを差し引き.
	float m_fOffsetZ;

	//前の注視点空間でのカメラの位置.
	D3DXVECTOR3 m_vOldFocusingSpacePos;

	//横にずらす.
	float m_fDisplaceHorizontally;

	//プロジェクション(射影行列)変換.
	void Projection();

	//移動.
	void Move(const D3DXVECTOR3 vDif);

	//移動制限.
	void MoveLimit();

	//姿勢更新.
	void PostureUpdate();

	//奥行き方向のオフセット更新.
	void OffsetZUpdate();

	//位置,回転,サイズを適応.
	void UpdateState();
};
