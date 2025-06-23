#ifndef BOT_HPP
# define BOT_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <cerrno>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <csignal>
# include <iostream>
# include <unistd.h>
# include <poll.h>
# include <vector>
# include <fstream>
# include <algorithm>
# include <stdio.h>

#define DEBUG false

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
		std::ofstream log;

		std::vector<std::string> split(std::string str, std::string delim);

	public:
		Bot(bool *sig, std::string addr, int port = 6667, std::string pass = std::string(), std::string nick = "bot");
		~Bot();

		void	online();
		void	talk(std::string message);
};


#endif