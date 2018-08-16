#pragma once 

#include "Thread.h"

using namespace netlib;

class CTestThread:public CThread
{
public:
	CTestThread(void);
	virtual ~CTestThread(void);
	void Init() ;
	void Close() ;

protected:
    virtual void Execute(void) ;
};

