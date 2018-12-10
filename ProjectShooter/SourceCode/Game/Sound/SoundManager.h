#pragma once

#include "Sound.h"

//�d�����čĐ��ł���ő�.
const int DUPLICABLE_MAX = 2;

//BGM�̉���.
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
		//�J�[�\���ƃ{�^���I������SE.
		enSE_Cursor = 0,
		enSE_PushButton,
		enSE_Count,
		enSE_Fire,

		//�v���C���[�p��SE.
		enSE_PlayerDamage,
		enSE_PlayerDamageVoice,
		enSE_PlayerDead,
		enSE_PlayerDeadVoice,
		enSE_PlayerAttack,
		enSE_PlayerAttackVoice,
		enSE_PlayerMoveLimit,

		//�G�l�~�[�p��SE.
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

	//�T�E���h�����[�h����.
	void LoadSound(const HWND hWnd);

	//BGM���͂��߂���Đ�.
	void PlayBGM(const enBGM Index)
	{
		if (m_pBGM[Index]->IsStopped())
		{
			m_pBGM[Index]->SeekToStart();
			m_pBGM[Index]->Play();
			m_pBGM[Index]->SetVolume(BGM_VOLUME);
		}
	}

	//BGM���~����.
	void StopBGM(const enBGM Index)
	{
		m_pBGM[Index]->Stop();
	}

	//BGM�̉��ʐݒ�.
	void SetBGMVolume(const enSE Index, const int iVolume)
	{
		m_pBGM[Index]->SetVolume(iVolume);
	}

	//SE���͂��߂���Đ�.
	void PlaySE(const enSE Index, const int iVolume = 1000);

	//SE���~����.
	void StopSE(const enSE Index);

	//SE��1�ڂ��Đ�.
	void PlayFirstSE(const enSE Index);

	//SE��1�ڂ���~�����m�F����.
	bool IsStoppedFirstSE(const enSE Index);

	//SE�̉��ʐݒ�.
	void SetSEVolume(const enSE Index, const int iVolume);

	//�S�T�E���h���~����.
	void StopSound();

private:
	Sound* m_pBGM[enBGM_Max];
	Sound* m_pSE[enSE_Max][DUPLICABLE_MAX];

	//BGM�����[�h����.
	void LoadBGM(const HWND hWnd);

	//BGM�̉��.
	void ReleaseBGM();

	//SE�����[�h����.
	void LoadSE(const HWND hWnd);

	//SE�̉��.
	void ReleaseSE();
};