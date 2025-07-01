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
	freeaddrinfo(servinfo);
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
	messages.insert(messages.begin(), "\0");
	std::memset(buf, 0, sizeof(buf));
	while (!connected)
	{
		try
		{
			send_command("NICK " + nick);
			send_command("USER " + nick + " " + nick + " " + addr + " :" + nick);
			serv_response();
			
			std::string msg = messages.front();
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
		std::string msg = messages.front();
		messages.erase(messages.begin());
		if (msg.find("001 " + nick) != std::string::npos)
			break;
	}
	send_command("JOIN " + nick);
	std::cout << "1" << std::endl;
	std::cout << "2" << std::endl;
	while (!*sig)
	{
		if (messages.empty())
			serv_response();
		std::string msg = messages.front();
		messages.erase(messages.begin());
		if ((msg.find("TIME") != std::string::npos) && (last_msg != msg))
		{
			print_current_time();
			last_msg = msg;
		}
	}
}

void	Bot::send_command(std::string command)
{
	command += "\r\n";
	if (send(sock, command.c_str(), command.size(), 0) < 0)
		throw std::runtime_error("send() failed");
}

void	Bot::serv_response()
{
	pseudo_sleep(1);
	ssize_t len = recv(sock, &buf, 4096, 0);
/* 	if (len < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		throw std::runtime_error("recv() failed");
	} */
	if (len == 0)
		throw std::runtime_error("Connection closed");
	buf[len] = '\0';
	receive += buf;
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

void	Bot::print_current_time()
{
	std::time_t now = std::time(NULL);
	std::tm* local = std::localtime(&now);

	char buffer[9];
	std::strftime(buffer, sizeof(buffer), "%H:%M:%S", local);

	std::cout << "Il est " << buffer << std::endl;
	send_command("PRIVMSG #" + nick + " Il est " + buffer + ".");
}