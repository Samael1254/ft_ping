NAME = ft_ping

SRCS_DIR = src/
BUILD_DIR = build/
HEADERS_DIR = includes/

HEADERS = ft_ping.h

HEADERS := $(addprefix $(HEADERS_DIR), $(HEADERS))

SRCS := main.c args.c output.c signal.c ping.c \
		request.c error.c

SRCS := $(addprefix $(SRCS_DIR), $(SRCS))

OBJS := $(addprefix $(BUILD_DIR), $(notdir $(SRCS:.c=.o)))

DEPS = $(OBJS:.o=.d)

CC = gcc

CFLAGS = -Wall -Wextra -Werror -I./includes -I./libft/includes

LIBFT = ./libft/lib/libft.a

LDFLAGS := -lft -Llibft/lib -lm

$(NAME): $(LIBFT) $(OBJS) $(HEADERS)
	@ echo " \033[33mCompiling ft_ping\033[m"
	@ $(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)
	@ echo " \033[1;32m Ping\033[0;1;32m binary compiled\033[m"

$(BUILD_DIR)%.o: $(SRCS_DIR)%.c
	@ mkdir -p $(BUILD_DIR)
	@ $(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	@ echo " \033[33mCompiling Libft\033[m"
	@ cd libft && $(MAKE) -s -j
	@ tput cuu1 && tput el
	@ tput cuu1 && tput el
	@ echo " \033[34m Libft compiled\033[m"

fclean_libft:
	@ echo " \033[33mCleaning Libft\033[m"
	@ cd libft && $(MAKE) -s fclean
	@ tput cuu1 && tput el
	@ tput cuu1 && tput el
	@ echo " \033[34m Libft cleaned\033[m"

libft: fclean_libft $(LIBFT)

all: $(NAME)

clean:
	@ echo " \033[33mCleaning\033[m"
	@ rm -f $(OBJS) $(DEPS)
	@ rm -rf $(BUILD_DIR)
	@ echo " \033[32m Ping build files cleaned\033[m"

fclean: clean
	@ rm -f $(NAME)
	@ echo " \033[32m Ping binary cleaned\033[m"

re: fclean all

.PHONY: clean fclean re all libft fclean_libft

-include $(DEP)
