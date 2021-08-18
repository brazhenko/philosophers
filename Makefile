COMPILER = clang
CFLAGS = -Wall -Wextra -Werror -O2 -DALIGN_8=__attribute__\(\(aligned\(8\)\)\)
LFLAGS = -Wall -Wextra -Werror -O2

NAME = philo
SRC_DIR = sources
SOURCES = $(SRC_DIR)/atomic_primitives.c \
		  $(SRC_DIR)/event_queue.c \
		  $(SRC_DIR)/fork.c \
		  $(SRC_DIR)/helpers.c \
		  $(SRC_DIR)/initialize_context.c \
		  $(SRC_DIR)/main.c \
		  $(SRC_DIR)/philo.c \
		  $(SRC_DIR)/philo_life.c \

OBJS = $(patsubst %, %.o, $(SOURCES))
DEPS = $(patsubst %, %.d, $(SOURCES))
LIBS = -lpthread

all : $(NAME)

$(NAME): $(OBJS)
	$(COMPILER) $(LFLAGS) -o $(NAME) $(OBJS)  $(LIBS)

%.c.o: %.c
	$(COMPILER) $(CFLAGS)  -o $@ -c $< -MMD

clean:
	rm -rf $(OBJS) $(DEPS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

-include $(DEPS)
