#include "Sound.h"
#include <stdio.h>

//コンストラクタ.
Sound::Sound()
	: m_hWnd(nullptr)
{
	for ( int i = 0; i < STR_BUFF_MAX; i++)
	{
		m_sAlias[i] = NULL;
	}
}

//デストラクタ.
Sound::~Sound()
{
	//Close();

	for ( int i = 0; i < STR_BUFF_MAX; i++)
	{
		m_sAlias[i] = NULL;
	}

	m_hWnd = nullptr;
}

//音声ファイルを開く.
bool Sound::Open(LPCTSTR sFName, char *sAlias, HWND hWnd)
{
	//初期設定.
	SetInitParam( sAlias, hWnd );

	//コマンド.
	char command[STR_BUFF_MAX] = "open ";

	strcat_s( command, sizeof( command ), sFName );//ファイル名を結合.
	strcat_s( command, sizeof( command ), " alias " );
	strcat_s( command, sizeof( command ), m_sAlias );//エイリアスを結合.

	if( mciSendString( command, NULL, 0, m_hWnd ) == 0 )
	{
		return true;
	}
	return false;
}

//音声ファイルを閉じる.
bool Sound::Close()
{
	//コマンド.
	char command[STR_BUFF_MAX] = "close ";

	strcat_s( command, sizeof( command ), m_sAlias );//エイリアスを結合.

	if( mciSendString( command, NULL, 0, m_hWnd ) == 0 )
	{
		return true;
	}
	return false;
}

//再生関数.
bool Sound::Play(bool bNotify)
{
	//コマンド.
	char command[STR_BUFF_MAX] = "play ";

	strcat_s( command, sizeof( command ), m_sAlias );//エイリアスを結合.
	if( bNotify )
	{
		strcat_s( command, sizeof( command ), " notify" );
		//notify:処理が終了したときにhWndにMM_MCINOTIFYメッセージをポストする.
	}

	if( mciSendString( command, NULL, 0, m_hWnd ) == 0 ){
		return true;
	}
	return false;
}

//停止関数.
bool Sound::Stop()
{
	//コマンド.
	char command[STR_BUFF_MAX] = "stop ";

	strcat_s( command, sizeof( command ), m_sAlias );//エイリアスを結合.

	if( mciSendString( command, NULL, 0, m_hWnd ) == 0 ){
		return true;
	}
	return false;
}

//状態を取得する関数.
//	sStatus の配列数は 256以下にすること.
//※ただし、状態を取得する場合は、再生時に「notify」を設定し、
//  ウィンドウハンドルにメッセージを送っておく必要がある.
bool Sound::GetStatus(char *sStatus)
{
	//コマンド.
	char command[STR_BUFF_MAX] = "status ";

	strcat_s( command, sizeof( command ), m_sAlias );//エイリアスを結合.
	strcat_s( command, sizeof( command ), " mode" );

	if( mciSendString( command, sStatus, STR_BUFF_MAX, m_hWnd ) == 0 ){
		return true;
	}
	return false;
}

//音の停止を確認する関数.
bool Sound::IsStopped()
{
	char sStatus[STR_BUFF_MAX] = "";

	//状態の取得.
	if( GetStatus( sStatus ) )
	{
		//文字列比較.
		if( strcmp( sStatus, "stopped" ) == 0 )
		{
			return true;
		}
	}
	return false;
}

//音の再生中を確認する関数.
bool Sound::IsPlaying()
{
	char sStatus[STR_BUFF_MAX] = "";

	//状態の取得.
	if( GetStatus( sStatus ) )
	{
		//文字列比較.
		if( strcmp( sStatus, "playing" ) == 0 )
		{
			return true;
		}
	}
	return false;
}

//再生位置を初めに戻す関数.
bool Sound::SeekToStart()
{
	//コマンド.
	char command[STR_BUFF_MAX] = "seek ";

	strcat_s( command, sizeof( command ), m_sAlias );//エイリアスを結合.
	strcat_s( command, sizeof( command ), " to start" );

	if( mciSendString( command, NULL, 0, m_hWnd ) == 0 ){
		return true;
	}
	return false;
}

//音量を設定する関数.
bool Sound::SetVolume(int iVolume)
{
	//範囲内に丸める.
	if( iVolume < 0 )		{ iVolume = 0;		}
	if( iVolume > 1000 )	{ iVolume = 1000;	}

	//音量を文字列に変換.
	char sVolume[STR_BUFF_MAX] = "";
	sprintf_s( sVolume, sizeof( sVolume ), "%d", iVolume );


	//コマンド.
	char command[STR_BUFF_MAX] = "setaudio ";

	strcat_s( command, sizeof( command ), m_sAlias );//エイリアスを結合.
	strcat_s( command, sizeof( command ), " volume to " );
	strcat_s( command, sizeof( command ), sVolume );//音量を結合.

	if( mciSendString( command, NULL, 0, m_hWnd ) == 0 ){
		return true;
	}
	return false;
}

//初期設定.
void Sound::SetInitParam(char *sAlias, HWND hWnd)
{
	//ウィンドウハンドルを渡す.
	m_hWnd = hWnd;

	//エイリアスを渡す.
	strcpy_s( m_sAlias, sizeof( m_sAlias ), sAlias );
}
