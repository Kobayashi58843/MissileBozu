#include "Singleton.h"

//�j���p�֐���o�^�ł���ő�.
const  int MAX_FINALIZERS_SIZE = 256;

//���݂̔j���p�֐��̔ԍ�.
static int FINALIZES_NUMBER = 0;

//�o�^����Ă���j���p�֐�.
static SingletonFinalizer::FinalizerFunc FINALIZES[MAX_FINALIZERS_SIZE];

//�V���O���g���ō쐬���ꂽ�N���X�̔j���p�֐���o�^.
void SingletonFinalizer::AddFinalizer(FinalizerFunc Func)
{
	assert(FINALIZES_NUMBER < MAX_FINALIZERS_SIZE);
	FINALIZES[FINALIZES_NUMBER++] = Func;
}

//�o�^���ꂽ�N���X��j��.
void SingletonFinalizer::Finalize()
{
	for (int i = FINALIZES_NUMBER - 1; i >= 0; --i)
	{
		(*FINALIZES[i])();
	}
	FINALIZES_NUMBER = 0;
}