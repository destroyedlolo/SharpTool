/*
 *	counter.hxx
 *	(c) L.Faillie	2002
 *
 *	Counter w/ microsecond resolution
 *
 *	------------- V 2.0 -----------
 *	01/12/2002	Creation
 */
#ifndef COUNTERS_HXX
#define COUNTERS_HXX

#include <ctime>
#include <sys/time.h>

struct counter {
	void set(){ 
		gettimeofday(&tv, NULL);
	 };
	
	unsigned long operator - ( counter &ct2 ){
		return( (tv.tv_sec - ct2.tv.tv_sec) * 1000 + tv.tv_usec / 1000 -  ct2.tv.tv_usec / 1000 );
	};

private:
	timeval tv;
};

#endif
