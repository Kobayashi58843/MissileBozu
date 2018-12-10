#pragma once

#include "Sound.h"

//重複して再生できる最大.
const int DUPLICABLE_MAX = 2;

//BGMの音量.
const int BGM_VOLUME = 600;

class SoundManager
{
public:
	enum enBGM
	{
		enBGM_Starting = 0,
		enBGM_Title,
		enBGM_StartEvent,
		enBGM_Action,
		enBGM_Win,
		enBGM_Clear,
		enBGM_Lose,
		enBGM_Continue,
		enBGM_Over,

		enBGM_Max,
	};

	enum enSE
	{
		//カーソルとボタン選択時のSE.
		enSE_Cursor = 0,
		enSE_PushButton,
		enSE_Count,
		enSE_Fire,

		//プレイヤー用のSE.
		enSE_PlayerDamage,
		enSE_PlayerDamageVoice,
		enSE_PlayerDead,
		enSE_PlayerDeadVoice,
		enSE_PlayerAttack,
		enSE_PlayerAttackVoice,
		enSE_PlayerMoveLimit,

		//エネミー用のSE.
		enSE_EnemyDamage,
		enSE_EnemyDamageVoice,
		enSE_EnemyDead,
		enSE_EnemyDeadVoice,
		enSE_EnemyAttack,
		enSE_EnemyAttackVoice,
		enSE_EnemyGuard,
		enSE_EnemyFall,
		enSE_EnemyFallVoice,

		enSE_Explosion,

		enSE_Max,
	};

	SoundManager();
	~SoundManager();

	//サウンドをロードする.
	void LoadSound(const HWND hWnd);

	//BGMをはじめから再生.
	void PlayBGM(const enBGM Index)
	{
		if (m_pBGM[Index]->IsStopped())
		{
			m_pBGM[Index]->SeekToStart();
			m_pBGM[Index]->Play();
			m_pBGM[Index]->SetVolume(BGM_VOLUME);
		}
	}

	//BGMを停止する.
	void StopBGM(const enBGM Index)
	{
		m_pBGM[Index]->Stop();
	}

	//BGMの音量設定.
	void SetBGMVolume(const enSE Index, const int iVolume)
	{
		m_pBGM[Index]->SetVolume(iVolume);
	}

	//SEをはじめから再生.
	void PlaySE(const enSE Index, const int iVolume = 1000);

	//SEを停止する.
	void StopSE(const enSE Index);

	//SEの1つ目を再生.
	void PlayFirstSE(const enSE Index);

	//SEの1つ目が停止中か確認する.
	bool IsStoppedFirstSE(const enSE Index);

	//SEの音量設定.
	void SetSEVolume(const enSE Index, const int iVolume);

	//全サウンドを停止する.
	void StopSound();

private:
	Sound* m_pBGM[enBGM_Max];
	Sound* m_pSE[enSE_Max][DUPLICABLE_MAX];

	//BGMをロードする.
	void LoadBGM(const HWND hWnd);

	//BGMの解放.
	void ReleaseBGM();

	//SEをロードする.
	void LoadSE(const HWND hWnd);

	//SEの解放.
	void ReleaseSE();
};