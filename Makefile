NAME = ircserv
DIR_SRC = src/
DIR_OBJ = obj/
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -f

SRCS =  $(wildcard $(DIR_SRC)*.cpp)

OBJS = $(SRCS:$(DIR_SRC)%.cpp=$(DIR_OBJ)%.o)

all: $(NAME)

$(DIR_OBJ)%.o: $(DIR_SRC)%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@echo "$(BOLD)$(BLUE)-----------------------------------$(RESET)"
	@echo "$(BOLD)$(RED)🛠️ 🚧    Compilation started    🚧🛠️$(RESET)"
	@echo "\n"
	@echo "$(BOLD)$(ORANGE)⏳   Compilation in progress...  ⏳$(RESET)"
	@echo "\n"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS)
	@echo "$(BOLD)$(GREEN)🎆       Compilation clear       🎆$(RESET)"
	@echo "$(BOLD)$(BLUE)-----------------------------------$(RESET)"

clean:
	@rm -rf $(DIR_OBJ)
	@$(MAKE) --no-print-directory -C bot clean
	@echo "$(BOLD)$(YELLOW)🧹🧼      Cleaned .o files      🧼🧹$(RESET)"
	@echo "\n"

fclean: clean
	@$(RM) $(NAME)
	@$(MAKE) --no-print-directory -C bot fclean
	@echo "$(BOLD)$(YELLOW)🧹🧼        Cleaned exec        🧼🧹$(RESET)"
	@echo "\n"

bonus:
	@$(MAKE) --no-print-directory -C bot

re: fclean all bonus