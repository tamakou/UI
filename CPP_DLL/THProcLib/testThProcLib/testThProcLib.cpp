// testThItkLib.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "math.h"
#include "testThProcLibCls.h"
 
int _tmain(int argc, _TCHAR* argv[])
{
	CTestThProcLib tester;
	//tester.setupStudyUID(L"1.2.392.200069.17.10338133111.1202505010003201");
	tester.setupStudyUID(L"1.2.392.200069.17.10338133111.1202306010006001");
	if (!tester.initDll()) {
		printf("initDll error\n"); return -1;
	}
	else {
		printf("initDll OK\n");
	};
 	tester.testProc();
 //   tester.testProc2(); return 0;
 //		tester.testProcSts();
  		tester.testRender();
 //    tester.testRenderCnt();
 //	tester.testCTMarker();
 //	tester.testRoiMarker();
	return 0;
}
 
 