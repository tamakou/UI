#include "MultiThExe.h"

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#pragma warning (disable: 4244)
 
CMultiThExe::CMultiThExe(int thNum,CThProcIf *thProc )
{
	m_thNum = thNum;
	m_Arg = new ThExeArg [m_thNum] ;
	
	m_thProc = thProc;
	//
	for(int th_i=0;th_i<m_thNum;th_i++){
		m_Arg[th_i].thExeHd = this;
		m_Arg[th_i].thNum = th_i;
		HANDLE tmp = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)exe_thread, &(m_Arg[ th_i ]), 0, &(m_Arg[ th_i ].m_tid) );
		if (tmp)
		{
			m_Arg[ th_i ].m_thd  = tmp;
			 
		}else{
			m_Arg[ th_i ].m_thd  = 0;
		}
	}

}

CMultiThExe::~CMultiThExe(void)
{
	if(m_Arg) delete [] m_Arg;
	 
}
  
int CMultiThExe::exe_thread( void* arg )

{
	ThExeArg* exe_prm = ( ThExeArg*)arg;
	return exe_prm->thExeHd->m_thProc->doProc(exe_prm->thNum);
}
bool CMultiThExe::wait( int timeout) 
{
	HANDLE *wait_hd = new HANDLE[m_thNum];
	for(int th_i=0;th_i<m_thNum;th_i++){
		wait_hd[th_i] = m_Arg[ th_i ].m_thd ;
	}
	DWORD wait_timeout = timeout;
	if(wait_timeout<0) wait_timeout = INFINITE;
	if(WAIT_TIMEOUT == WaitForMultipleObjects( m_thNum, wait_hd, TRUE, wait_timeout )){
		return false;
	}

	for(int th_i=0;th_i<m_thNum;th_i++){
		CloseHandle(wait_hd[th_i] );
	}
	 
	return true;
}

class CTestThProc : public CThProcIf
{
public:
	CTestThProc(CMultiThTester *tester){
		m_tester	= tester;
		
	 
	}
protected:
	virtual int doProc( int th_id){
		 m_tester->doTest(th_id);
		return 0;
	}
	CMultiThTester *m_tester;
};

