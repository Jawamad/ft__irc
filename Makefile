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
	@if [ ! -d "$(dir $@)" ]; then mkdir -p "$(dir $@)"; fi
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

re: fclean all



WAF_HTTP_PORT=8080
WAF_HTTPS_PORT=8443

# Intern Docker
API_GATEWAY_PORT=8080
AUTHSERVICE_PORT=3000
DBWRITER_PORT=4000
VAULT_PORT=8200

# URLs
AUTH_SERVICE_URL=http://authservice:3000
FRONTEND_URL=https://waf:443
DBWRITER_URL=http://dbwriter:4000
# DB
DATABASE_URL=/app/data/db.sqlite

# Vault
VAULT_ADDR=http://0.0.0.0:8200

# authservice
JWT_SECRET=a_long_random_secret_string

# elk
ELASTIC_PASSWORD=testmdp
KIBANA_PASSWORD=testmdp
LOGSTASH_PASSWORD=testmdp
