//
// Created by Lommy greenhands Reznak mo reznak on 7/25/21.
//

#include <pthread.h>
#include <stdio.h>
unsigned char arr[64 * 64];

void *writer(void *a)
{
	const size_t i = (size_t)a;
	int b=  0;

	while (1) {
		arr[i] = b;
		b++;
	}
}

void *reader(void *a)
{
	const size_t i = (size_t)a;
	int b = 0;

	while (1) {
		b += arr[i];
	}
}

int main()
{
	pthread_t a, b, c, d;

	
	pthread_create(&a, NULL, reader, (void*)0);
	pthread_create(&b, NULL, reader, (void*)0);
	pthread_create(&c, NULL, reader, (void*)0);
	pthread_create(&d, NULL, writer, (void*)0);

	printf("first -- %p\n", a); 
	printf("first -- %p\n", d);

	pthread_join(a, NULL);
}
