#pragma once

 
class CThProcIf
{
public:
	virtual int doProc(int th_id) = 0;
	virtual void cancelProc(void) = 0;//#205_Cancel_import_data_added_cancel_flag
};
class CMultiThExe;
class ThExeArg
{
public:
	CMultiThExe *thExeHd;
	int thNum;
	void* m_thd ;
	unsigned long  m_tid ;
};
 
class CMultiThExe
{
public:
	CMultiThExe(void);
	CMultiThExe(int thNum,CThProcIf *thProc);
	~CMultiThExe(void);
	bool wait(int timeout=-1);
protected:
	static int exe_thread( void* arg );
	CThProcIf *m_thProc;
	int m_thNum;
	ThExeArg *m_Arg ;
 
};


class CTHLevelSetBarrier;
class CMultiThTester
{
public:
	CMultiThTester();
	~CMultiThTester();
	void testBarrier(void);
	void doTest( int thID);
protected:
	int m_runID;
	int m_timeTbl_size;
	int *m_timeTbl;
	CTHLevelSetBarrier *m_Barrier;
	int m_NumOfThreads;
};