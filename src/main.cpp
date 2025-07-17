#include "../inc/ircserv.hpp"
#include "../inc/Server.hpp"
#include <csignal>

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>\n";
		return 1;
	}

	int port = std::atoi(argv[1]);
	if (port <= 0 || port > 65535)
	{
		std::cerr << "Error: invalid port.\n";
		return 1;
	}

	std::string password = argv[2];

	try
	{
		Server server(port, password);
		server.start();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
		return 1;
	}

	return 0;
}