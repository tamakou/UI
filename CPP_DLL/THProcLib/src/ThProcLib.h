// XtMfcLib.cpp : DLL �p�̏����������̒�`���s���܂��B
//
#if 0
//2012/03/27 windows2008 Service error

#include <windows.h>
 
 
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#endif

#pragma warning (disable: 4819)

#include "stdafx.h"

#ifdef WIN32

class CDcmLibDllApp : public CWinApp
{
public:
	CDcmLibDllApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};



//
//TODO: ���� DLL �� MFC DLL �ɑ΂��ē��I�Ƀ����N�����ꍇ�A
//		MFC ���ŌĂяo����邱�� DLL ����G�N�X�|�[�g���ꂽ�ǂ̊֐���
//		�֐��̍ŏ��ɒǉ������ AFX_MANAGE_STATE �}�N����
//		�����Ȃ���΂Ȃ�܂���B
//
//		��:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �ʏ�֐��̖{�̂͂��̈ʒu�ɂ���܂�
//		}
//
//		���̃}�N�����e�֐��Ɋ܂܂�Ă��邱�ƁAMFC ����
//		�ǂ̌Ăяo�����D�悷�邱�Ƃ͔��ɏd�v�ł��B
//		����͊֐����̍ŏ��̃X�e�[�g�����g�łȂ���΂� 
//		��Ȃ����Ƃ��Ӗ����܂��A�R���X�g���N�^�� MFC
//		DLL ���ւ̌Ăяo�����s���\��������̂ŁA�I�u
//		�W�F�N�g�ϐ��̐錾�����O�łȂ���΂Ȃ�܂���B
//
//		�ڍׂɂ��Ă� MFC �e�N�j�J�� �m�[�g 33 �����
//		58 ���Q�Ƃ��Ă��������B
//


// CtestDumyDll2App

BEGIN_MESSAGE_MAP(CDcmLibDllApp, CWinApp)
END_MESSAGE_MAP()


// CtestDumyDll2App �R���X�g���N�V����

CDcmLibDllApp::CDcmLibDllApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CtestDumyDll2App �I�u�W�F�N�g�ł��B

CDcmLibDllApp theApp;


// CtestDumyDll2App ������

BOOL CDcmLibDllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

#endif //WIN32