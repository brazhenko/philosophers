cmake_minimum_required(VERSION 3.17)
project(philo C)

set(CMAKE_C_STANDARD 99)

add_executable(philo sources/main.c
		sources/fork.c sources/fork.h sources/philo.h sources/helpers.c sources/helpers.h sources/initialize_context.c
		sources/event_queue.c sources/event_queue.h sources/philo.c sources/atomic_primitives.c
		sources/atomic_primitives.h benchmarking/main.cpp benchmarking/linked_queue/linked_queue.c benchmarking/linked_queue/linked_queue.h)
