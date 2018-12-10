#include "SoundManager.h"

#include "..\\..\\MyMacro.h"

#include <stdlib.h>

SoundManager::SoundManager()
{
	for (int i = 0; i < enBGM::enBGM_Max; i++)
	{
		m_pBGM[i] = nullptr;
	}

	for (int i = 0; i < enSE::enSE_Max; i++)
	{
		for (int j = 0; j < DUPLICABLE_MAX; j++)
		{
			m_pSE[i][j] = nullptr;
		}
	}
}

SoundManager::~SoundManager()
{
	ReleaseSE();

	ReleaseBGM();
}

//サウンドをロードする.
void SoundManager::LoadSound(const HWND hWnd)
{
	//BGM.
	LoadBGM(hWnd);

	//SE.
	LoadSE(hWnd);
}

//全サウンドを停止する.
void SoundManager::StopSound()
{
	for (int i = 0; i < enBGM::enBGM_Max; i++)
	{
		m_pBGM[i]->Stop();
	}

	for (int i = 0; i < enSE::enSE_Max; i++)
	{
		for (int j = 0; j < DUPLICABLE_MAX; j++)
		{
			m_pSE[i][j]->Stop();
		}
	}
}

//SEをはじめから再生.
void SoundManager::PlaySE(const enSE Index, const int iVolume)
{
	for (int i = 0; i < DUPLICABLE_MAX; i++)
	{
		//停止中の音のみ再生する.
		if (m_pSE[Index][i]->IsStopped())
		{
			m_pSE[Index][i]->SeekToStart();
			m_pSE[Index][i]->Play();
			m_pSE[Index][i]->SetVolume(iVolume);
			return;
		}
	}
}

//SEを停止する.
void SoundManager::StopSE(const enSE Index)
{
	for (int i = 0; i < DUPLICABLE_MAX; i++)
	{
		m_pSE[Index][i]->Stop();
	}
}

//SEの1つ目を再生.
void SoundManager::PlayFirstSE(const enSE Index)
{
	m_pSE[Index][0]->SeekToStart();
	m_pSE[Index][0]->Play();
}

//SEの1つ目が停止中か確認する.
bool SoundManager::IsStoppedFirstSE(const enSE Index)
{
	if (m_pSE[Index][0]->IsStopped())
	{
		return true;
	}
	return false;
}

//SEの音量設定.
void SoundManager::SetSEVolume(const enSE Index, const int iVolume)
{
	for (int i = 0; i < DUPLICABLE_MAX; i++)
	{
		m_pSE[Index][i]->SetVolume(iVolume);
	}
}

//BGMをロードする.
void SoundManager::LoadBGM(const HWND hWnd)
{
	for (int i = 0; i < enBGM::enBGM_Max; i++)
	{
		SOUND_DATA sData;

		switch (i)
		{
		case enBGM_Starting:
			sData = { "Data\\Sound\\BGM\\Starting.mp3", "Starting" };

			break;
		case enBGM_Title:
			sData = { "Data\\Sound\\BGM\\Title.mp3", "Title" };

			break;
		case enBGM_StartEvent:
			sData = { "Data\\Sound\\BGM\\StartEvent.mp3", "StartEvent" };

			break;
		case enBGM_Action:
			sData = { "Data\\Sound\\BGM\\Action.mp3", "Action" };

			break;
		case enBGM_Win:
			sData = { "Data\\Sound\\BGM\\Win.mp3", "Win" };

			break;
		case enBGM_Clear:
			sData = { "Data\\Sound\\BGM\\Clear.mp3", "Clear" };

			break;
		case enBGM_Lose:
			sData = { "Data\\Sound\\BGM\\Lose.mp3", "Lose" };

			break;
		case enBGM_Continue:
			sData = { "Data\\Sound\\BGM\\Continue.mp3", "Continue" };

			break;
		case enBGM_Over:
			sData = { "Data\\Sound\\BGM\\Over.mp3", "Over" };

			break;
		default:
			break;
		}

		m_pBGM[i] = new Sound;
		//音声ファイルを開く.
		m_pBGM[i]->Open(sData.sPath, sData.sAlias, hWnd);
	}
}

//BGMの解放.
void SoundManager::ReleaseBGM()
{
	for (int i = enBGM::enBGM_Max - 1; i >= 0; i--)
	{
		m_pBGM[i]->Close();
		SAFE_DELETE(m_pBGM[i]);
	}
}

//SEをロードする.
void SoundManager::LoadSE(const HWND hWnd)
{
	for (int i = 0; i < enSE::enSE_Max; i++)
	{
		SOUND_DATA sData;

		for (int j = 0; j < DUPLICABLE_MAX; j++)
		{
			switch (i)
			{
			case enSE_Cursor:
				sData = { "Data\\Sound\\SE\\System\\Cursor.mp3", "Cursor" };
				
				break;
			case enSE_PushButton:
				sData = { "Data\\Sound\\SE\\System\\PushButton.mp3", "PushButton" };

				break;
			case enSE_Count:
				sData = { "Data\\Sound\\SE\\System\\Count.mp3", "Count" };

				break;
			case enSE_Fire:
				sData = { "Data\\Sound\\SE\\System\\Fire.mp3", "Fire" };

				break;
			case enSE_PlayerDamage:
				sData = { "Data\\Sound\\SE\\Player\\PlayerDamage.mp3", "PlayerDamage" };

				break;
			case enSE_PlayerDamageVoice:
				sData = { "Data\\Sound\\SE\\Player\\Voice\\PlayerDamageVoice.mp3", "PlayerDamageVoice" };

				break;
			case enSE_PlayerDead:
				sData = { "Data\\Sound\\SE\\Player\\PlayerDead.mp3", "PlayerDead" };

				break;
			case enSE_PlayerDeadVoice:
				sData = { "Data\\Sound\\SE\\Player\\Voice\\PlayerDeadVoice.mp3", "PlayerDeadVoice" };

				break;
			case enSE_PlayerAttack:
				sData = { "Data\\Sound\\SE\\Player\\PlayerAttack.mp3", "PlayerAttack" };

				break;
			case enSE_PlayerAttackVoice:
				sData = { "Data\\Sound\\SE\\Player\\Voice\\PlayerAttackVoice.mp3", "PlayerAttackVoice" };

				break;
			case enSE_PlayerMoveLimit:
				sData = { "Data\\Sound\\SE\\Player\\PlayerMoveLimit.mp3", "PlayerMoveLimit" };

				break;
			case enSE_EnemyDamage:
				sData = { "Data\\Sound\\SE\\Enemy\\EnemyDamage.mp3", "EnemyDamage" };

				break;
			case enSE_EnemyDamageVoice:
				sData = { "Data\\Sound\\SE\\Enemy\\Voice\\EnemyDamageVoice.mp3", "EnemyDamageVoice" };

				break;
			case enSE_EnemyDead:
				sData = { "Data\\Sound\\SE\\Enemy\\EnemyDead.mp3", "EnemyDead" };

				break;
			case enSE_EnemyDeadVoice:
				sData = { "Data\\Sound\\SE\\Enemy\\Voice\\EnemyDeadVoice.mp3", "EnemyDeadVoice" };

				break;
			case enSE_EnemyAttack:
				sData = { "Data\\Sound\\SE\\Enemy\\EnemyAttack.mp3", "EnemyAttack" };

				break;
			case enSE_EnemyAttackVoice:
				sData = { "Data\\Sound\\SE\\Enemy\\Voice\\EnemyAttackVoice.mp3", "EnemyAttackVoice" };

				break;
			case enSE_EnemyGuard:
				sData = { "Data\\Sound\\SE\\Enemy\\EnemyGuard.mp3", "EnemyGuard" };

				break;
			case enSE_EnemyFall:
				sData = { "Data\\Sound\\SE\\Enemy\\EnemyFall.mp3", "EnemyFall" };

				break;
			case enSE_EnemyFallVoice:
				sData = { "Data\\Sound\\SE\\Enemy\\Voice\\EnemyFallVoice.mp3", "EnemyFallVoice" };

				break;
			case enSE_Explosion:
				sData = { "Data\\Sound\\SE\\Player\\Explosion.mp3", "Explosion" };

				break;
			default:
				break;
			}

			char cNo[8];
			_itoa_s(j, cNo, 10);
			strcat_s(sData.sAlias, cNo);

			m_pSE[i][j] = new Sound;
			//音声ファイルを開く.
			m_pSE[i][j]->Open(sData.sPath, sData.sAlias, hWnd);
		}
	}
}

//SEの解放.
void SoundManager::ReleaseSE()
{
	for (int i = enSE::enSE_Max - 1; i >= 0; i--)
	{
		//音を重ねる数.
		for (int j = DUPLICABLE_MAX - 1; j >= 0; j--)
		{
			m_pSE[i][j]->Close();
			SAFE_DELETE(m_pSE[i][j]);
		}
	}
}