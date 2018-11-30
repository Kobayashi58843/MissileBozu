#include "Singleton.h"

//破棄用関数を登録できる最大.
const  int MAX_FINALIZERS_SIZE = 256;

//現在の破棄用関数の番号.
static int FINALIZES_NUMBER = 0;

//登録されている破棄用関数.
static SingletonFinalizer::FinalizerFunc FINALIZES[MAX_FINALIZERS_SIZE];

//シングルトンで作成されたクラスの破棄用関数を登録.
void SingletonFinalizer::AddFinalizer(FinalizerFunc Func)
{
	assert(FINALIZES_NUMBER < MAX_FINALIZERS_SIZE);
	FINALIZES[FINALIZES_NUMBER++] = Func;
}

//登録されたクラスを破棄.
void SingletonFinalizer::Finalize()
{
	for (int i = FINALIZES_NUMBER - 1; i >= 0; --i)
	{
		(*FINALIZES[i])();
	}
	FINALIZES_NUMBER = 0;
}