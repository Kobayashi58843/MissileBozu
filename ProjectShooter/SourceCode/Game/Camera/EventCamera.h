#pragma once

#include "..\\Object.h"

class EventCamera
{
public:
	EventCamera(const float fWindowWidth, const float fWindowHeight);
	~EventCamera();

	//ビュー.
	D3DXMATRIX GetView() const
	{
		return m_mView;
	}

	//プロジェクション.
	D3DXMATRIX GetProjection() const
	{
		return m_mProjection;
	}

	//位置.
	void SetPos(const D3DXVECTOR3 vPos)
	{
		m_vPos = vPos;
	}
	void AddPos(const D3DXVECTOR3 vAdd)
	{
		m_vPos += vAdd;
	}
	D3DXVECTOR3 GetPos() const
	{
		return m_vPos;
	}

	//注視位置.
	void SetLookAt(const D3DXVECTOR3 vLookAt)
	{
		m_vLookAt = vLookAt;
	}
	void AddLookAt(const D3DXVECTOR3 vAdd)
	{
		m_vLookAt += vAdd;
	}
	D3DXVECTOR3 GetLookAt() const
	{
		return m_vLookAt;
	}

	void SetUpVector(const D3DXVECTOR3 vUpVec)
	{
		m_vUpVector = vUpVec;
	}
	D3DXVECTOR3 GetUpVector() const
	{
		return m_vUpVector;
	}

	void AddRoll(const float fAdd)
	{
		m_fRoll += fAdd;
	}
	float GetRoll() const
	{
		return m_fRoll;
	}

	//更新.
	void Update();

private:
	//ウィンドウ幅.
	float m_fWindowWidth;
	//ウィンドウ高さ.
	float m_fWindowHeight;

	//ビュー行列.
	D3DXMATRIX	m_mView;
	//プロジェクション行列.
	D3DXMATRIX m_mProjection;
	
	//位置.
	D3DXVECTOR3 m_vPos;
	//注視位置.
	D3DXVECTOR3 m_vLookAt;
	//上方方向.
	D3DXVECTOR3 m_vUpVector;

	float m_fRoll;

	//ビュー変換.
	void View();
	//プロジェクション(射影行列)変換.
	void Projection();
};