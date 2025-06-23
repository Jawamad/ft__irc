NAME = bot
DIR_OBJ = obj/
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -f

SRCS =  $(wildcard *.cpp) \

OBJS = $(SRCS:%.cpp=$(DIR_OBJ)%.o)


# Colors
RESET = \033[0m
BOLD = \033[1m
RED = \033[91m
GREEN = \033[92m
YELLOW = \033[33m
ORANGE = \033[93m
BLUE = \033[94m



all: $(NAME)

$(DIR_OBJ)%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@echo "$(BOLD)$(BLUE)------------------------------------------$(RESET)"
	@echo "$(BOLD)$(RED)🛠️ 🚧        Compilation started       🚧🛠️$(RESET)"
	@echo "\n"
	@echo "$(BOLD)$(ORANGE)⏳      Compilation in progress...      ⏳$(RESET)"
	@echo "\n"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	@echo "$(BOLD)$(GREEN)🎆           Compilation finished          🎆$(RESET)"
	@echo "$(BOLD)$(BLUE)------------------------------------------$(RESET)"

clean:
	@rm -rf $(DIR_OBJ)
	@echo "$(BOLD)$(YELLOW)🧹🧼     Cleaned .o bot's files     🧼🧹$(RESET)"
	@echo "\n"

fclean: clean
	@$(RM) $(NAME)
	@rm -f *.replace
	@echo "$(BOLD)$(YELLOW)🧹🧼        Cleaned bot exec        🧼🧹$(RESET)"
	@echo "\n"

re: fclean all