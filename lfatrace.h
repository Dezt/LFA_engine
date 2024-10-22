#ifndef LFATRACE_H
#define LFATRACE_H

#if defined LFATRACE && WIN32
//TODO add right .h

using namespace std;

class ProfileTime
{

protected:
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;

	public:
	void start()
	{
    // get ticks per second
    QueryPerformanceFrequency(&frequency);

    // start timer
    QueryPerformanceCounter(&t1);
	}
   
	//.... do something

	void stop()
	{
    // stop timer
    QueryPerformanceCounter(&t2);
    // elapsed time in millisec
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	}

	// elapsed time in millisec
	double getElapsedTime()
	{		   
		return (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	}

};
//#else //windows

#endif //Profile windows


#if defined LFATRACE && __APPLE_CC__
//Something like this:
//TODO compile and test for iPhone
#import <sys/time.h>

class ProfileTime
{
protected:
	timeval start;

public:
	void start()
	{
		gettimeofday(&start, 0);			
	}
 
	void stop()
	{
		timeval	stop;
		gettimeofday(&stop, 0);
	}

	// elapsed time in millisec
	double getElapsedTime()
	{		 
		double ret = -1;
		timeval	result;
		timersub(&start,&stop,&result);
		ret =  (result.tv_sec*1000.0) + (result.tv_usec/1000.0); 

		return ret; 
	}

};





#endif

#ifndef LFATRACE
class ProfileTime
{

	public:
	void start()
	{
	}
 
	void stop()
	{
	}

	// elapsed time in millisec
	double getElapsedTime()
	{		   
		return -1; 
	}

};

#endif

#endif //LFATRACE_H
