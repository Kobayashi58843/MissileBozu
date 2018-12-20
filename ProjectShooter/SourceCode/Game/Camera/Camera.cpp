#include"Camera.h"

//注視点からカメラまでの距離.
const float FOCUSING_SPACE_DISTANCE = 5.0f;

//回転速度.
const float DEGREE = 1.0f;

//視野の広さ.
const float SCOPE = 4.0f;

//描画する最大の距離.
const float LOOK_DISTANCE = 500.0f;

//上方向の角度の限界.
const int MAX_UP_ANGLE_DEGREE = 60;

//下方向の角度の限界.
const int MAX_DOWN_ANGLE_DEGREE = 45;

//距離の限界.
const float DISTANCE_MAX = 50.0f;

//当たり判定の大きさ.
const float RADIUS = 0.2f;

Camera::Camera(const float fWindowWidth, const float fWindowHeight)
	: m_fWindowWidth(fWindowWidth)
	, m_fWindowHeight(fWindowHeight)
	, m_fCameraAngleUnit(0.0f)
	, m_fOffsetZ(0.0f)
	, m_fDisplaceHorizontally(0.0f)
{
	CrearVECTOR3(m_vFocusingSpacePos);
	m_vFocusingSpacePos.z = -FOCUSING_SPACE_DISTANCE;

	CrearVECTOR3(m_vWorldPos);

	D3DXMatrixIdentity(&m_mView);
	D3DXMatrixIdentity(&m_mCameraPose);
	D3DXMatrixIdentity(&m_mProjection);

	CrearVECTOR3(m_vLookAt);
	CrearVECTOR3(m_vCameraMoveDirDif);
	CrearVECTOR3(m_vCamRotDif);
	SetRotPerFrame(DEGREE);

	CrearVECTOR3(m_vPos);
	CrearVECTOR3(m_vRot);
	m_Collision.fRadius = RADIUS;
	m_Collision.vCenter = m_vPos;

	m_vOldFocusingSpacePos = m_vPos + m_vFocusingSpacePos;

	m_vOldPos = m_vWorldPos;
}

Camera::~Camera()
{
}

void Camera::Update()
{
	//カメラZ軸回転で姿勢仮更新.
	D3DXMATRIX mRotMatV;
	D3DXMatrixRotationYawPitchRoll(&mRotMatV, m_vCamRotDif.y, m_vCamRotDif.x, m_vCamRotDif.z);
	m_mCameraPose = mRotMatV * m_mCameraPose;

	//緯度経度の差分を出す.
	D3DXVECTOR3 vDif;
	D3DXVec3TransformCoord(&vDif, &m_vCameraMoveDirDif, &m_mCameraPose);

	//差分がある時のみ移動をする.
	if (vDif.x != 0.0f || vDif.y != 0.0f || vDif.z != 0.0f)
	{
		//移動.
		Move(vDif);
	}

	//奥行き方向のオフセット更新.
	OffsetZUpdate();

	//移動制限.
	MoveLimit();

	//移動後カメラ姿勢更新.
	PostureUpdate();

	//カメラのワールド座標.
	m_vWorldPos = m_vFocusingSpacePos + m_vLookAt;

	//移動後の位置をレイ用のものに入れる.
	UpdateState();

	//ビュー行列更新.
	memcpy(&m_mView, &m_mCameraPose, sizeof(D3DXMATRIX));
	memcpy(&m_mView.m[3], &m_vWorldPos, sizeof(D3DXVECTOR3));
	m_mView._44 = 1.0f;
	D3DXMatrixInverse(&m_mView, 0, &m_mView);

	//横にずらす.
	m_mView.m[3][0] += m_fDisplaceHorizontally;

	//プロジェクション(射影行列)変換.
	Projection();

	//差分メンバをリセット.
	CrearVECTOR3(m_vCameraMoveDirDif);
	m_fOffsetZ = 0.0f;
	CrearVECTOR3(m_vCamRotDif);
}

//レイとメッシュの衝突時.
void Camera::RayHitToMesh(clsDX9Mesh* const pTarget)
{
}

//プロジェクション(射影行列)変換.
void Camera::Projection()
{
	D3DXMatrixPerspectiveFovLH(&m_mProjection, (FLOAT)(D3DX_PI / SCOPE),
		m_fWindowWidth / m_fWindowHeight,//アスペクト比(幅÷高さ).
		0.1f, LOOK_DISTANCE);
}

//移動.
void Camera::Move(const D3DXVECTOR3 vDif)
{
	D3DXVECTOR3 vRotAxis;
	D3DXVECTOR3* camZAxis = (D3DXVECTOR3*)m_mCameraPose.m[2];
	D3DXVec3Cross(&vRotAxis, &vDif, camZAxis);

	D3DXQUATERNION qTrans;
	D3DXQuaternionRotationAxis(&qTrans, &vRotAxis, m_fCameraAngleUnit);
	D3DXMATRIX mTransRot;
	D3DXMatrixRotationQuaternion(&mTransRot, &qTrans);
	D3DXVec3TransformCoord(&m_vFocusingSpacePos, &m_vFocusingSpacePos, &mTransRot);
}

//移動制限.
void Camera::MoveLimit()
{
	//変更前の距離.
	float fOldDistance = D3DXVec3Length(&m_vFocusingSpacePos);

	//カメラの角度.
	float fCamAngleY = atanf(m_vFocusingSpacePos.y);

	if (fCamAngleY < D3DXToRadian(MAX_UP_ANGLE_DEGREE) &&
		fCamAngleY > D3DXToRadian(-MAX_DOWN_ANGLE_DEGREE))
	{
		//注視点空間でのカメラの位置を保存.
		m_vOldFocusingSpacePos = m_vFocusingSpacePos;
	}
	else
	{
		//範囲外の場合は前の位置に戻す.
		m_vFocusingSpacePos.y = m_vOldFocusingSpacePos.y;
	}

	/*====/ 注視位置とカメラの距離の調整 /====*/

	//変更後の距離.
	float fDistance = D3DXVec3Length(&m_vFocusingSpacePos);

	D3DXVECTOR3 vCameraPoseZ;
	vCameraPoseZ = -m_vFocusingSpacePos;
	D3DXVec3Normalize(&vCameraPoseZ, &vCameraPoseZ);

	//変更前後の距離に差がある場合は調整する.
	if (fOldDistance != fDistance)
	{
		m_vFocusingSpacePos -= (fOldDistance - fDistance) * vCameraPoseZ;
	}
}

//姿勢更新.
void Camera::PostureUpdate()
{
	D3DXVECTOR3 vCameraPoseX, vCameraPoseY, vCameraPoseZ;

	vCameraPoseZ = -m_vFocusingSpacePos;
	D3DXVec3Normalize(&vCameraPoseZ, &vCameraPoseZ);

	vCameraPoseY = { 0.0f, 1.0f, 0.0f };
	D3DXVec3Cross(&vCameraPoseX, &vCameraPoseY, &vCameraPoseZ);
	D3DXVec3Normalize(&vCameraPoseX, &vCameraPoseX);

	D3DXVec3Cross(&vCameraPoseY, &vCameraPoseZ, &vCameraPoseX);
	D3DXVec3Normalize(&vCameraPoseY, &vCameraPoseY);

	D3DXMatrixIdentity(&m_mCameraPose);
	memcpy(m_mCameraPose.m[0], &vCameraPoseX, sizeof(D3DXVECTOR3));
	memcpy(m_mCameraPose.m[1], &vCameraPoseY, sizeof(D3DXVECTOR3));
	memcpy(m_mCameraPose.m[2], &vCameraPoseZ, sizeof(D3DXVECTOR3));
}

//奥行き方向のオフセット更新.
void Camera::OffsetZUpdate()
{
	D3DXVECTOR3 vCameraPoseZ;
	vCameraPoseZ = -m_vFocusingSpacePos;
	D3DXVec3Normalize(&vCameraPoseZ, &vCameraPoseZ);

	float fDistance = D3DXVec3Length(&m_vFocusingSpacePos);

	if (fDistance - m_fOffsetZ <= DISTANCE_MAX &&
		fDistance - m_fOffsetZ > 0.0f)
	{
		m_vFocusingSpacePos += m_fOffsetZ * vCameraPoseZ;
	}
}

//位置,回転,サイズを適応.
void Camera::UpdateState()
{
	CrearVECTOR3(m_vRot);
	m_fScale = 0.0f;

	m_Collision.fRadius = 1.0f;

	m_vPos = m_vWorldPos;
	m_Collision.vCenter = m_vPos;
}