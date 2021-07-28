cmake_minimum_required(VERSION 3.17)
project(philo C)

set(CMAKE_C_STANDARD 99)

add_executable(philo main.c fork.c fork.h philo.h helpers.c helpers.h initialize_context.c event_queue.c event_queue.h philo.c)
