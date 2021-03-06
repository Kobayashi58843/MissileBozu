#include "EventCamera.h"

//視野の広さ.
const float SCOPE = 4.0f;

//描画する最大の距離.
const float LOOK_DISTANCE = 500.0f;

//初期の上方方向(真上).
const D3DXVECTOR3 UP_VECTOR = { 0.0f, 1.0f, 0.0f };

EventCamera::EventCamera(const float fWindowWidth, const float fWindowHeight)
	: m_fWindowWidth(fWindowWidth)
	, m_fWindowHeight(fWindowHeight)
{
	CrearVECTOR3(m_vPos);
	CrearVECTOR3(m_vLookAt);
	CrearVECTOR3(m_vRot);
}

EventCamera::~EventCamera()
{
}

//更新.
void EventCamera::Update()
{
	//ビュー変換.
	View();

	//プロジェクション(射影行列)変換.
	Projection();
}

//ビュー変換.
void EventCamera::View()
{
	D3DXVECTOR3 vUpVec = { 0.0f, 1.0f, 0.0f };
	
	D3DXMATRIX mLookAtMat;
	D3DXMatrixIdentity(&mLookAtMat);
	D3DXMatrixLookAtLH(&mLookAtMat, &m_vPos, &m_vLookAt, &vUpVec);
	D3DXMatrixInverse(&mLookAtMat, NULL, &mLookAtMat);


	D3DXMATRIX mRotMat;
	D3DXMatrixIdentity(&mRotMat);
	D3DXMatrixRotationYawPitchRoll(&mRotMat, m_vRot.y, m_vRot.x, m_vRot.z);

	mRotMat = mLookAtMat * mRotMat;

	//ビュー行列更新.
	memcpy(&m_mView, &mRotMat, sizeof(D3DXMATRIX));
	memcpy(&m_mView.m[3], &m_vPos, sizeof(D3DXVECTOR3));

	D3DXMatrixInverse(&m_mView, NULL, &m_mView);
}

//プロジェクション(射影行列)変換.
void EventCamera::Projection()
{
	D3DXMatrixPerspectiveFovLH(&m_mProjection, (FLOAT)(D3DX_PI / SCOPE),
		m_fWindowWidth / m_fWindowHeight,//アスペクト比(幅÷高さ).
		0.1f, LOOK_DISTANCE);
}