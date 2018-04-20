#ifndef __RINGBUF_H_
#define __RINGBUF_H_

#include <pthread.h>

typedef struct _RingBuf
{
	int _lenth;
	int _head;	//write
	int _tial;	//read
	int res_cnt;
	pthread_mutex_t r_mutex;
	pthread_cond_t r_cond;
	void **bufs;
} RingBuf;

RingBuf * new_ringbuf(int);  //res_cnt
void destroy_ringbuf(RingBuf *rbuf);

void *rpull(RingBuf *,int timeout);

/**
 * return
 * 	0	:success
 * 	others:failed
 */
int rpush(RingBuf * _self,void *buf);

#endif /* __RINGBUF_H_*/
