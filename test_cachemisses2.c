#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

char arr[64 * 64];

void* f(void* a)
{
	
	 size_t i = (int)a;
	
	 while(1) {
	  	// usleep(100);
	 	arr[i] = i;
		i++;
	 }
}

int main() {
	 pthread_t t, t2, t3, t1;
	
	 pthread_create(&t, NULL, f, (void*)0);
	 pthread_create(&t1, NULL, f, (void*)1);
	 pthread_create(&t2, NULL, f, (void*)2);
	 pthread_create(&t3, NULL, f, (void*)128);
	 
	 pthread_join(t, NULL);
	 pthread_join(t1, NULL);
	 pthread_join(t2, NULL);
	 pthread_join(t3, NULL);
}
