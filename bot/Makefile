NAME = ../ircbot
DIR_OBJ = obj/
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -f

SRCS =  $(wildcard *.cpp) \

OBJS = $(SRCS:%.cpp=$(DIR_OBJ)%.o)

all: $(NAME)

$(DIR_OBJ)%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	@echo "$(BOLD)$(GREEN)🎆   Bot compiled to ../ircbot   🎆$(RESET)"
clean:
	@rm -rf $(DIR_OBJ)

fclean: clean
	@$(RM) $(NAME)

re: fclean all