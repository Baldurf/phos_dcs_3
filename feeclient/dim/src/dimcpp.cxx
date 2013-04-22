#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __VMS
#include <starlet.h>
#endif

#define DIMLIB
//#include <dim_core.hxx>
#include "../dim/dim_core.hxx"
//#include <dim.hxx>
#include "../dim/dim.hxx"


int DimCore::inCallback = 0;

extern "C" {
static void timer_user_routine(void *tp)
{
	DimTimer *t = (DimTimer *)tp;
	DimCore::inCallback = 1;
	t->firedFlag = 1;
	t->runningFlag = 0;
	t->timerHandler();
	DimCore::inCallback = 0;
}
}

void DimTimer::start(int time)
{
	runningFlag = 1;
	firedFlag = 0;
	dtq_start_timer(time, timer_user_routine, this);
}

int DimTimer::stop() 
{
	firedFlag = 0;
	runningFlag = 0;
	return dtq_stop_timer(this);
}

DimTimer::DimTimer() 
{ 
	firedFlag = 0;
	runningFlag = 0; 
}
	
DimTimer::DimTimer(int time)
{ 
	start(time);
}

DimTimer::~DimTimer()
{
	if(runningFlag)
		stop();
}

// Threads

extern "C" {
static void thread_user_routine(void *tp)
{
	DimThread *t = (DimThread *)tp;
//	DimCore::inCallback = 1;
//	t->firedFlag = 1;
//	t->runningFlag = 0;
	t->threadHandler();
	t->itsId = 0;
//	DimCore::inCallback = 0;
}
}

DimThread::DimThread() 
{
//	start();
	itsId = 0;
}
	
DimThread::~DimThread()
{
//	if(itsId)
//		stop();
}

int DimThread::start()
{
	if(!itsId)
	{
#ifdef NOTHREADS
	  // MR: 21.12.2005 thread functions are not build if the NOTHREADS flag is set
	  // and can thus neither be used
	  return 0;
#else //!NOTHREADS
	        itsId = dim_start_thread(thread_user_routine, this);
#endif //NOTHREADS
		return 1;
	}
	return 0;
}
/*
int DimThread::stop()
{
	int ret = dim_stop_thread(itsId);
	itsId = 0;
	return ret;
}
*/
