#include "ringbuf.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>


RingBuf * new_ringbuf(int cnt)
{
	/*
	static RingBuf *rbuf = NULL;
	
	if(NULL != rbuf)
	{
		return rbuf;
	}
*/
	RingBuf *rbuf = NULL;
	rbuf = (RingBuf *)malloc(sizeof(RingBuf));
	/*
	if(rbuf = (RingBuf *)malloc(sizeof(RingBuf)) == NULL)
	{
		printf("Failed to malloc RingBuf!\n");
		exit(-1);
	}*/

	rbuf->bufs = (void *)malloc(sizeof(void *) * cnt);
	rbuf->_lenth = cnt;
	rbuf->_head = 0;
	rbuf->_tial = 0;
	rbuf->res_cnt = 0;
	pthread_mutex_init(&(rbuf->r_mutex),NULL);
	pthread_cond_init(&(rbuf->r_cond),NULL);

	return rbuf;
}

void destroy_ringbuf(RingBuf *rbuf)
{
	free(rbuf->bufs);
	pthread_mutex_destroy(&(rbuf->r_mutex));
	pthread_cond_destroy(&(rbuf->r_cond));
	free(rbuf);
}


void *rpull(RingBuf *rbuf,int timeout)
{
	void *element = NULL;
	pthread_mutex_lock(&(rbuf->r_mutex));
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += timeout;
	while(rbuf->res_cnt == 0)
	{
		int ret = pthread_cond_timedwait(&rbuf->r_cond,&rbuf->r_mutex,&ts);

		if(ret==ETIMEDOUT)
        {
            printf("time out\n");
			pthread_mutex_unlock(&rbuf->r_mutex);
            return NULL;
        }
	}
	element = rbuf->bufs[rbuf->_tial];
	rbuf->bufs[rbuf->_tial] = NULL;
	rbuf->res_cnt--;
	rbuf->_tial = (rbuf->_tial+1)%rbuf->_lenth;
	pthread_mutex_unlock(&rbuf->r_mutex);

	return element;
	

}

/**
 * return
 *      0       :success
 *      others:failed
 */
int rpush(RingBuf * _self,void *buf)
{
	pthread_mutex_lock(&_self->r_mutex);
	if(_self->res_cnt == _self->_lenth)
	{
		pthread_mutex_unlock(&_self->r_mutex);
		return -1;
	}
	_self->bufs[_self->_head] = buf;
	_self->res_cnt++;
	_self->_head = (_self->_head+1)%_self->_lenth;
	pthread_cond_signal(&_self->r_cond);
	pthread_mutex_unlock(&_self->r_mutex);
	return 0;
}
