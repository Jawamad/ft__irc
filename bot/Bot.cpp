#include "Bot.hpp"

Bot::Bot(bool *sig, std::string addr, int port, std::string pass, std::string nick): sig(sig), addr(addr), pass(pass), nick(nick)
{
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("socket() failed");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	struct addrinfo hints, *servinfo;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	rv = getaddrinfo( addr.c_str() , 0 , &hints , &servinfo);

	serv_addr.sin_addr = ((struct sockaddr_in *) servinfo->ai_addr)->sin_addr;
	std::cout << "Connecting to " << addr << ":" << port << std::endl;
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
		throw std::runtime_error("connect() failed");
	int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

}

Bot::~Bot()
{
	std::cout << "So long !" << std::endl;
	send_command("QUIT");
	close(sock);
}

void	Bot::talk(std::string message)
{
	std::cout << message << std::endl;
}

void	Bot::online()
{
	if (!pass.empty())
		send_command("PASS " + pass);
	while (!connected)
	{
		try
		{
			send_command("NICK " + nick);
			send_command("USER " + nick + " " + nick + " " + addr + " :" + nick);
			serv_response();
			std::string msg = messages[0];
			messages.erase(messages.begin());
			if (msg.find("433") != std::string::npos)
				throw std::runtime_error("Nickname already in use");
			connected = true;
		}
		catch (std::runtime_error &e)
		{
			std::cerr << e.what() << std::endl;
			nick += "_";
		}
	}
	while (!*sig)
	{
		if (messages.empty())
			serv_response();
		std::string msg = messages[0];
		messages.erase(messages.begin());
		std::cout << messages[0] << std::endl;
		if (msg.find("001 " + nick) != std::string::npos)
			break;
	}
	std::cout << "entering join" << std::endl;
	send_command("JOIN #salon");
	std::cout << "exiting join" << std::endl;
}

void	Bot::send_command(std::string command)
{
	command += "\r\n";
	if (send(sock, command.c_str(), command.size(), 0) < 0)
		throw std::runtime_error("send() failed");
}

void	Bot::serv_response()
{
	char buf[4096 + 1];

	pseudo_sleep(1);
	std::cout << "allo" << std::endl;
	ssize_t len = recv(sock, &buf, 4096, 0);
	std::cout << "allo" << std::endl;
	/* if (len < 0)
		throw std::runtime_error("recv() failed"); */
	if (len == 0)
		throw std::runtime_error("Connection closed");
	buf[len] = '\0';
	receive += buf;
	std::cout << receive << std::endl;
	while (receive.find("\r\n") != std::string::npos)
	{
		std::string msg = receive.substr(0, receive.find("\r\n"));
		receive.erase(0, receive.find("\r\n") + 2);
		messages.push_back(msg);
	}
}

void	Bot::pseudo_sleep(unsigned int seconds)
{
	std::clock_t start = std::clock();
	while ((std::clock() - start) / CLOCKS_PER_SEC < seconds)
		;
}
