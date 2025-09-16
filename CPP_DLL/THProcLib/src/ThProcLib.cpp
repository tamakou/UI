// XtMfcLib.cpp : DLL 用の初期化処理の定義を行います。
//
#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
void th_lib_close(void);
void detach_main(void)
{
    printf("detach dll \n");
  
}
void detach_thread(void)
{
    printf("detach thread \n");

}
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        detach_thread();
        break;
    case DLL_PROCESS_DETACH:
        detach_main();
        break;
    }
    return TRUE;
}