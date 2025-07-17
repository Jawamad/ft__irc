#ifndef BOT_HPP
# define BOT_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <cerrno>
# include <cstdlib>
# include <ctime>
# include <cstring>
# include <iostream>
# include <unistd.h>
# include <poll.h>
# include <vector>
# include <fstream>
# include <fcntl.h>
# include <string>
# include <set>
# include <sstream>
# include <cctype>
# include <stdexcept>
# include <csignal>

class Bot
{
	private:
		bool *sig;
		std::string addr;
		int sock;
		struct sockaddr_in serv_addr;

		std::string pass;
		std::string nick;
		bool connected;

		std::string receive;
		std::vector<std::string> messages;

		char buf[4096 + 1];

	public:
		Bot(bool *sig, std::string addr, int port = 55555, std::string pass = std::string(), std::string nick = "bot");
		~Bot();

		void	online();
		void	send_command(std::string command);
		void	talk(std::string message);
		void	serv_response();
		void	print_current_time();

		bool containsBannedWord(const std::string& message, const std::string& filename);
		void addBannedWord(const std::string& word, const std::string& filename);
		void removeBannedWord(const std::string& word, const std::string& filename);
		void displayBanlist(const std::string& filename);
};

#endif