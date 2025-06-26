#ifndef BOT_HPP
# define BOT_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <cerrno>
# include <cstdio>
# include <cstdlib>
# include <ctime>
# include <cstring>
# include <csignal>
# include <iostream>
# include <unistd.h>
# include <poll.h>
# include <vector>
# include <fstream>
# include <algorithm>
# include <stdio.h>
# include <fcntl.h>

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

	public:
		Bot(bool *sig, std::string addr, int port = 6667, std::string pass = std::string(), std::string nick = "bot");
		~Bot();

		void	online();
		void	send_command(std::string command);
		void	talk(std::string message);
		void	serv_response();
		void	pseudo_sleep(unsigned int seconds);
};

#endif