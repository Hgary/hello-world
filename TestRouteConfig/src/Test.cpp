#include "Test.h"

CTestThread::CTestThread(void)
{
}

CTestThread::~CTestThread(void)
{
}

void CTestThread::Execute(void)
{
    
}

void CTestThread::Init()
{
	Start() ;
}

void CTestThread::Close()
{
	this->Terminate() ;
	this->WaitFor() ;
}

