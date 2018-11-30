#pragma once

#include "..\\..\\..\\Object.h"
#include "..\\..\\ModelResource\\SkinMesh\\CD3DXSKINMESH.h"

class EventModel : public Object
{
public:
	EventModel(clsD3DXSKINMESH* const pModel, const float fScale, const double dAnimSpeed);
	~EventModel();

	//3Dモデルの描画.
	void RenderModel(const D3DXMATRIX &mView, const D3DXMATRIX &mProj);

	//アニメーション切り替え関数.
	void ChangeAnimation(const int iIndex, const double dStartPos = 0.0f);

	//現在のアニメーションの終了を確認する関数.
	bool IsAnimationEnd();

	//現在のアニメーションが何割終了したかを確認する関数.
	bool IsAnimationRatioEnd(const float fRatio);

	//アニメーションの再生速度を変える.
	void SetAnimationSpeed(const double dAnimationSpeed);

	//アニメーションの最大数を取得.
	int GetAnimationQuantityMax();

private:
	//スキンメッシュのモデルに必要な情報.
	struct MESH_STATE
	{
		//再生しているアニメーションの番号.
		int iIndex;

		//アニメーションコントローラ.
		LPD3DXANIMATIONCONTROLLER AnimationController;

		//モデルのアニメーションの再生速度.
		double dAnimationSpeed;

		//モデルのアニメーションの現在の時間.
		double dAnimationTime;
	};

	//使いたいモデルのポインタ.
	clsD3DXSKINMESH* m_pModel;

	MESH_STATE m_ModelState;

	void AttachModel(clsD3DXSKINMESH* const pModel);
	void DetatchModel();

	//位置,回転,サイズをモデルに適応.
	void UpdateState();
};