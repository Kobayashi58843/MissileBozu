#pragma once

#include "Global.h"

#include <mutex>

//�쐬���ꂽ���Ԃ̋t���Ŕj������N���X.
class SingletonFinalizer
{
public:
	//�j���p�֐���o�^���邽�߂̃^�C�v�f�t.
	typedef void(*FinalizerFunc)();

	//�V���O���g���ō쐬���ꂽ�N���X�̔j���p�֐���o�^.
	static void AddFinalizer(FinalizerFunc Func);

	//�o�^���ꂽ�N���X��j��.
	static void Finalize();
};

template <typename T>
//�p���o���Ȃ��悤�ɂ��Ă���.
class Singleton final
{
public:
	static T& GetInstance()
	{
		//1�x�����uCreate�v�֐����Ăяo��.
		std::call_once(m_InitFlag, Create);
		assert(s_upInstance);
		return *s_upInstance;
	}

private:
	//�N���X���쐬���A�j���p�̊֐���o�^.
	static void Create()
	{
		//�N���X���쐬.
		std::unique_ptr<T> s_upInstance(new T);

		//�j���p�̊֐���o�^.
		SingletonFinalizer::AddFinalizer(&Singleton<T>::Destroy);
	}

	//�j���p�̊֐���o�^.
	static void Destroy()
	{
		s_upInstance.release();
	}

	//coll_once()�֐��̂��߂̃t���O.
	static std::once_flag m_InitFlag;
	//�C���X�^���X.
	static std::unique_ptr<T> s_upInstance;
};

template <typename T> std::once_flag Singleton<T>::m_InitFlag;
template <typename T> T* Singleton<T>::s_upInstance = nullptr;