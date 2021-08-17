# COMPILER_OPTIONS
COMPILER = clang++
CFLAGS =   -std=c++11 -I benchmark/include -O2 -DALIGN_8=__attribute__\(\(aligned\(8\)\)\)
LFLAGS =  $(CFLAGS) $(CPPFLAGS) -Wall -Wextra -Werror

NAME = bench
SOURCES = main.cpp external/fork.c external/atomic_primitives.c linked_queue/linked_queue.c external/event_queue.c
OBJ_DIR = build
OBJS = $(patsubst %, $(OBJ_DIR)/%.o, $(SOURCES))


DEPS = $(patsubst %, $(OBJ_DIR)/%.d, $(SOURCES))
LIBS = -lbenchmark -lpthread -Lbenchmark/build/src

all : $(NAME)

$(NAME): $(OBJS)
	$(COMPILER) $(CFLAGS) -o $(NAME) $(OBJS)  $(LIBS)

$(OBJ_DIR)/%.cpp.o: %.cpp
	mkdir -p $(OBJ_DIR)
	printf 'Compiling...\n'
	$(COMPILER) $(CFLAGS)  -o $@ -c $< -MMD

$(OBJ_DIR)/%.c.o: %.c
	mkdir -p $(OBJ_DIR)
	printf 'Compiling...\n'
	$(COMPILER) $(CFLAGS)  -o $@ -c $< -MMD

test:
	./$(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

deps:
	git clone https://github.com/google/benchmark.git
	git clone https://github.com/google/googletest.git benchmark/googletest
	cd benchmark
	cmake -E make_directory "build"
	cmake -E chdir "build" cmake -DCMAKE_BUILD_TYPE=Release ../
	cmake --build "build" --config Release

-include $(DEPS)
