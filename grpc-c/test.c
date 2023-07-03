#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 10

static pthread_t th[MAX] = { 0, };

void *cb(void *arg)
{
    long idx = (long) (long *)arg;

    printf("%s - start, idx=%ld\n",__func__, idx);
    sleep(1);
    printf("%s - end, idx=%ld\n",__func__, idx);

    return NULL;
}

void *cb2(void *arg)
{
    int i;
    int cnt = 0;

    printf("%s - start\n",__func__);

    while (1) {
	cnt++;
	printf("%s - cnt=%d, abc\n", __func__, cnt);
	for (i = 0; i < MAX; i++) {
	    printf("%s: cnt=%d, v[%d]=%ld\n", __func__, cnt, i, (long) *(long *)th[i]);

	    if (cnt == 1000) {
		pthread_join(th[i], NULL);
	    }
	}
	usleep(10000);
    }

    printf("%s - end\n",__func__);

    return NULL;
}


void main()
{
    int i;
    pthread_t th2;
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    /* Jake, NULL attribute over 4 threads with libc-2.27.so then
     * treat as PTHREAD_CREATE_DETACHED. 
     * so we use PTHREAD_CREATE_JOINABLE implicitly */
#if 1
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
#else
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
#endif

    for (i = 0; i < MAX; i++) {
	pthread_create(&th[i], &attr, cb, (void *)(long *)(long) i);
    }

    pthread_create(&th2, NULL, cb2, NULL);
    pthread_join(th2, NULL);
}
