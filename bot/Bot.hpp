#ifndef BOT_HPP
# define BOT_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <ctime>
# include <cstring>
# include <csignal>
# include <iostream>
# include <poll.h>
# include <vector>
# include <fstream>
# include <algorithm>
# include <string>
# include <set>
# include <sstream>
# include <cctype>

class Bot
{
	private:
		int sock;
		bool *sig;
		bool connected;
		char buf[4096 + 1];
		std::string addr;
		std::string pass;
		std::string nick;
		std::string receive;
		std::vector<std::string> messages;
		struct sockaddr_in serv_addr;

		bool	containsBannedWord(const std::string& message, const std::string& filename);
		void	sendCommand(std::string command);
		void	servResponse();
		void	printCurrentTime();
		void	addBannedWord(const std::string& word, const std::string& filename);
		void	removeBannedWord(const std::string& word, const std::string& filename);
		void	displayBanlist(const std::string& filename);
		void	rollDice(const std::string& msg);

	public:
		Bot();
		Bot(bool *sig, std::string addr, int port = 55555, std::string pass = std::string(), std::string nick = "bot");
		Bot(const Bot &other);
		Bot &operator=(const Bot &other);
		~Bot();

		void	talk(std::string message);
		void	online();
};

#endif