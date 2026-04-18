NAME = ft_ping

SRCS_DIR = src/
BUILD_DIR = build/
HEADERS_DIR = includes/

HEADERS = ft_ping.h

HEADERS := $(addprefix $(HEADERS_DIR), $(HEADERS))

SRCS := main.c args.c output.c signal.c ping.c \
		packet.c error.c

SRCS := $(addprefix $(SRCS_DIR), $(SRCS))

OBJS := $(addprefix $(BUILD_DIR), $(notdir $(SRCS:.c=.o)))

DEPS = $(OBJS:.o=.d)

CC = gcc

CFLAGS = -Wall -Wextra -Werror -I./includes

LDFLAGS = -lm

$(NAME): $(LIBFT) $(OBJS) $(HEADERS)
	@ echo " \033[33mCompiling ft_ping\033[m"
	@ $(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)
	@ echo " \033[1;32m Ping binary compiled\033[m"
	@ echo " \033[33mAdding raw socket permission\033[m"
	@ sudo setcap cap_net_raw+ep $(NAME)
	@ echo " \033[32m Ping can now create raw sockets\033[m"

$(BUILD_DIR)%.o: $(SRCS_DIR)%.c
	@ mkdir -p $(BUILD_DIR)
	@ $(CC) $(CFLAGS) -c $< -o $@

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

.PHONY: clean fclean re all

-include $(DEP)
