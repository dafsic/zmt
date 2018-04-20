#include <stdio.h>
#include <stdlib.h>
#include "ringbuf.h"


#define PRODUCER_COUNT 1
#define CONSUMER_COUNT 1

pthread_t g_thread[PRODUCER_COUNT+CONSUMER_COUNT];

void* produce(void *arg)
{
	RingBuf *rbuf = (RingBuf *)arg;
	int i;
	int *p;
	for (i = 0;i < 100;i++)
	{
		p = (int *)malloc(sizeof(int));
		*p = i;
		int ret;
		if (ret =rpush(rbuf,(void *)p) != 0)
		{
			i--;
			free(p);
		}
		else
		{
			printf("[produce]produce %dth production\n",i);
		}
	}


	return NULL;
}


void *consume(void *arg)
{
	RingBuf *rbuf = (RingBuf *)arg;
	int i;
	void *p;

	for (i = 0;i < 100;i++)
	{
		p = rpull(rbuf,2);
		if (p == NULL)
		{
			i--;
		}
		else
		{
			printf("[consume]get %d\n",*(int *)p);
			free(p);
		}

	}
	return NULL;
}


int main()
{
	int i;

	RingBuf *rbuf = new_ringbuf(10);
	
	printf("[main]Create consumer thread...\n");
	for(i = 0;i < CONSUMER_COUNT;i++)
	{
		printf("[main]Create %d consumer\n",i);
		pthread_create(&g_thread[i],NULL,consume,(void *)rbuf);
	}
	sleep(5);

	printf("[main]Create producer thread...\n");
	for(i = 0;i < PRODUCER_COUNT;i++)
	{
		printf("[main]Create %d producer\n",i);
		pthread_create(&g_thread[CONSUMER_COUNT+i],NULL,produce,(void *)rbuf);
	}

	printf("[main]Waid thread end ...\n");
	for(i = 0;i < PRODUCER_COUNT+CONSUMER_COUNT;i++)
	{
		pthread_join(g_thread[i],NULL);
	}	

	destroy_ringbuf(rbuf);

	return 0;
}
				
