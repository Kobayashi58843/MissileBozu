#ifndef _MY_MACRO_H_
#define _MY_MACRO_H_

#include <Windows.h>

/*========/マクロ /========*/

//エラーメッセージ.
#define ERR_MSG(str,title)	{MessageBox(NULL,str,title,MB_OK);}

//解放.
#define SAFE_RELEASE(p)		{ if(p) { (p)->Release(); (p)=nullptr; } }
#define SAFE_DELETE(p)		{ if(p) { delete (p);     (p)=nullptr; } }
#define SAFE_DELETE_ARRAY(p){ if(p) { delete[] (p);   (p)=nullptr; } }

//VECTORに[ 0.0f ]を入れる.
#define CrearVECTOR2(p){(p) = { 0.0f, 0.0f };}
#define CrearVECTOR3(p){(p) = { 0.0f, 0.0f, 0.0f };}
#define CrearVECTOR4(p){(p) = { 0.0f, 0.0f, 0.0f, 0.0f};}

#endif//#ifndef _MY_MACRO_H_