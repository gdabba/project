//2011105039 박동진
//본인은 이 소스파일을 직접 작성하였습니다.
//set_ticker.c

#include	<stdio.h>
#include	<sys/time.h>
#include	<unistd.h>
#include	<signal.h>
#include	<stdlib.h>

int set_ticker( int n_msecs)
{
	struct	itimerval new_timeset;
	long	n_sec, n_usecs;

	n_sec = n_msecs /1000;
	n_usecs = (n_msecs %1000) * 1000L;

	new_timeset.it_interval.tv_sec	 = n_sec;
	new_timeset.it_interval.tv_usec	 = n_usecs;
	new_timeset.it_value.tv_sec	= n_sec;
	new_timeset.it_value.tv_usec	= n_usecs;

	return setitimer(ITIMER_REAL, &new_timeset, NULL);
}

