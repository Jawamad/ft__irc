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
}

Bot::~Bot()
{
	printf("so long !\n");
}

void	Bot::talk(std::string message)
{
	std::cout << message << std::endl;
}

void	Bot::online()
{
	
}