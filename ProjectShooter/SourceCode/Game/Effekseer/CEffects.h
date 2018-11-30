#ifndef _CEFFECTS_H_
#define _CEFFECTS_H_
//警告についてのコード分析を無効にする。4005：再定義.
#pragma warning( disable : 4005 )

/*=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=*|
|*   先にしておくヘッダ,ライブラリの読込   *|
|*=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=*/
#include<stdio.h>

#include<D3D11.h>
#include<XAudio2.h>
#pragma comment(lib,"d3d11.lib")

/*=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=*|
|*   ここからEffekseer関係のヘッダ,ライブラリの読込  *|
|*=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=*/
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <EffekseerSoundXAudio2.h>

#if _DEBUG
//VisualStudioのバージョンが違う場合は.
//___________________↓↓ここを変えれば行けるはず!!.
#pragma comment(lib,"VS2013\\Debug\\Effekseer.lib")
#pragma comment(lib,"VS2013\\Debug\\EffekseerRendererDX11.lib")
#pragma comment(lib,"VS2013\\Debug\\EffekseerSoundXAudio2.lib")
#else//#if _DEBUG
#pragma comment(lib,"VS2013\\Release\\Effekseer.lib")
#pragma comment(lib,"VS2013\\Release\\EffekseerRendererDX11.lib")
#pragma comment(lib,"VS2013\\Release\\EffekseerSoundXAudio2.lib")
#endif//#if _DEBUG

/*=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=*|
|*           「D3DX～」使用で必須                *|
|*   ※注意 : Effekseer関係より後に読み込むこと  *|
|*=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=*/
#include<D3DX10.h>
#pragma comment(lib,"d3dx10.lib")

/*=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=*|
|*   フリーソフト「Effekseer」のデータを使うためのクラス   *|
|*=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=*/
class clsEffects
{
public:
	//エフェクト種類列挙体.
	enum enEfcType
	{
		enEfcType_Missile = 0,
		enEfcType_Explosion,

		enEfcType_PlayerHit,
		enEfcType_PlayerDead,
		enEfcType_PlayerWallHit,

		enEfcType_EnemyHit,
		enEfcType_EnemyDead,

		enEfcType_Max,
	};

	//インスタンス取得(唯一のアクセス経路).
	static clsEffects* GetInstance()
	{
		//唯一のインスタンスを作成する.
		//(staticで作成されたので2回目は無視される).
		static clsEffects s_Instance;
		return &s_Instance;
	}
	~clsEffects();	//デストラクタ.

	//構築関数.
	HRESULT Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	//破棄関数.
	HRESULT Destroy();
	//データ読込関数.
	HRESULT LoadData();
	//データ解放関数.
	HRESULT ReleaseData();
	//描画関数.
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj, D3DXVECTOR3& vEye);
	//ビュー行列設定関数.
	void SetViewMatrix(D3DXMATRIX& mView);
	//プロジェクション行列設定関数.
	void SetProjectionMatrix(D3DXMATRIX& mProj);

	//再生関数.
	::Effekseer::Handle Play(D3DXVECTOR3 vPos, enEfcType enType)
	{
		return m_pManager->Play(m_pEffect[enType], vPos.x, vPos.y, vPos.z);
	}
	//一時停止.
	void Paused(::Effekseer::Handle handle, bool bFlag)
	{
		m_pManager->SetPaused(handle, bFlag);//bFlag:true_一時停止.
	}
	//停止.
	void Stop(::Effekseer::Handle handle)
	{
		m_pManager->StopEffect(handle);
	}
	//全て停止.
	void StopAll()
	{
		m_pManager->StopAllEffects();
	}
	//サイズを指定する.
	void SetScale(::Effekseer::Handle handle, D3DXVECTOR3 vScale)
	{
		m_pManager->SetScale(handle, vScale.x, vScale.y, vScale.z);
	}
	//回転を指定する.
	void SetRotation(::Effekseer::Handle handle, D3DXVECTOR3 vRot)
	{
		m_pManager->SetRotation(handle, vRot.x, vRot.y, vRot.z);
	}
	void SetRotation(::Effekseer::Handle handle, D3DXVECTOR3 vAxis, float fAngle)
	{
		m_pManager->SetRotation(handle, ::Effekseer::Vector3D(vAxis.x, vAxis.y, vAxis.z), fAngle);
	}
	//位置を指定する.
	void SetLocation(::Effekseer::Handle handle, D3DXVECTOR3 vPos)
	{
		m_pManager->SetLocation(handle, ::Effekseer::Vector3D(vPos.x, vPos.y, vPos.z));
	}

	bool PlayCheck(::Effekseer::Handle handle)
	{
		return m_pManager->Exists(handle);
	}
	/*=+=+=+=+=+=+=/ ベクター変換関数 /=+=+=+=+=+=+=*/
	::Effekseer::Vector3D Vector3DxToEfk(const D3DXVECTOR3* pSrcVec3Dx);
	D3DXVECTOR3 Vector3EfkToDx(const ::Effekseer::Vector3D* pSrcVec3Efk);

	/*=+=+=+=+=+=+=/ 行列変換関数 /=+=+=+=+=+=+=*/
	::Effekseer::Matrix44 MatrixDxToEfk(const D3DXMATRIX* pSrcMatDx);
	D3DXMATRIX MatrixEfkToDx(const ::Effekseer::Matrix44* pSrcMatEfk);

private:
	//生成やコピーを禁止する.
	clsEffects();	//コンストラクタ.
	clsEffects(const clsEffects& rhs);
	clsEffects& operator = (const clsEffects& rhs);

	//エフェクトを動作させるために必要.
	::Effekseer::Manager*			m_pManager;
	::EffekseerRenderer::Renderer*	m_pRender;
	::EffekseerSound::Sound*		m_pSound;
	//エフェクトデータに含まれる音再生に必要.
	IXAudio2*						m_pXA2;
	IXAudio2MasteringVoice*			m_pXA2Master;

	//エフェクトの種類ごとに必要.
	::Effekseer::Effect*			m_pEffect[enEfcType_Max];
};


#endif//#ifndef _CEFFECTS_H_