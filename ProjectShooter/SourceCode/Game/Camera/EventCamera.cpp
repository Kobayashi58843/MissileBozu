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
	, m_vUpVector(UP_VECTOR)
	, m_fRoll(1.0f)
{
	CrearVECTOR3(m_vPos);
	CrearVECTOR3(m_vLookAt);
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
	m_vUpVector.y = sinf(m_fRoll);

	D3DXMatrixLookAtLH(&m_mView, &m_vPos, &m_vLookAt, &m_vUpVector);
}

//プロジェクション(射影行列)変換.
void EventCamera::Projection()
{
	D3DXMatrixPerspectiveFovLH(&m_mProjection, (FLOAT)(D3DX_PI / SCOPE),
		m_fWindowWidth / m_fWindowHeight,//アスペクト比(幅÷高さ).
		0.1f, LOOK_DISTANCE);
}