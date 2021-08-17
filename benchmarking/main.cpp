#include <benchmark/benchmark.h>
#include <iostream>
#include <pthread.h>
#include "external/fork.h"
#include "linked_queue/linked_queue.h"
#include "external/event_queue.h"



static void BM_my_mutex(benchmark::State& state) {
	// Perform setup here
	t_fork mtx;
	uint32_t ts;

	for (auto _ : state) {
		fork_try_take(&mtx);
		fork_put_down(&mtx);
	}
}

static void BM_pthread_mutex(benchmark::State& state) {
	// Perform setup here
	pthread_mutex_t lock;
	if (pthread_mutex_init(&lock, NULL) != 0) {
		fprintf(stderr, "mutex init failed\n");
		exit (-1);
	}

	for (auto _ : state) {
		pthread_mutex_lock(&lock);
		pthread_mutex_unlock(&lock);
	}
}

# define	TCOUNT	10
# define	CCC		1000

int writer(size_t count) {
	for (size_t i = 0; i < count; i++) {
		mtx_enqueue(i, i+1, i+2);
	}
	return 1;
}

int reader(size_t count) {
	for (int i = 0; i < count * TCOUNT; i++) {
		mtx_dequeue();
	}
	return 1;
}

static void test_queue_mutex(int thread_count) {
	mtx_init();
	pthread_t arr[100];

	for (int i = 0; i < thread_count; i++) {
		if (pthread_create(&arr[i], NULL, (void*(*)(void*))writer, (void*)(CCC)))
		{
			fprintf(stderr, "Error on creating thread :/\n");
			exit (-1);
		}
	}
	if (pthread_create(&arr[thread_count], NULL, (void*(*)(void*))reader, (void*)(CCC)))
	{
		fprintf(stderr, "Error on creating thread :/\n");
		exit (-1);
	}
	for (int i = 0; i < thread_count + 1; i++) {
		if (pthread_join(arr[i], NULL))
			exit(-1);
	}
}

int writer2(size_t count) {
	for (size_t i = 0; i < count; i++) {
		ev_enqueue(0, Eating, i);
	}
	return 1;
}
int reader2(size_t count) {
	for (size_t i = 0; i < count * TCOUNT; i++) {
		ev_dequeue();
	}
	return 1;
}

static void test_queue_lf(int thread_count) {
	pthread_t arr[100];

	for (int i = 0; i < thread_count; i++) {
		if (pthread_create(&arr[i], NULL, (void*(*)(void*))writer2, (void*)(CCC)))
		{
			fprintf(stderr, "Error on creating thread :/\n");
			exit (-1);
		}
	}
	if (pthread_create(&arr[thread_count], NULL, (void*(*)(void*))reader2, ((void*)(CCC))))
	{
		fprintf(stderr, "Error on creating thread :/\n");
		exit (-1);
	}

	for (int i = 0; i < thread_count; i++) {
		if (pthread_join(arr[i], NULL))
			exit(-1);
	}

	if (pthread_join(arr[thread_count], NULL))
		exit(-1);

}

static void BM_queue_mutex(benchmark::State& state) {
	for (auto _ : state) {
		test_queue_mutex(TCOUNT);
	}
}

static void BM_lf_queue(benchmark::State& state) {
	for (auto _ : state) {
		ev_clean();
		test_queue_lf(TCOUNT);
	}
}


// Register the function as a benchmark
BENCHMARK(BM_my_mutex);
BENCHMARK(BM_pthread_mutex);
BENCHMARK(BM_lf_queue);
BENCHMARK(BM_queue_mutex);

// Run the benchmark
BENCHMARK_MAIN();
