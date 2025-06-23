#include "../inc/PingCommand.hpp"
#include "../inc/Server.hpp"

void PingCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	(void) server;
	std::string token;
	args >> token;
	std::string pong = "PONG :" + token + "\r\n";
	send(client->getSocketFd(), pong.c_str(), pong.size(), 0);
	return;
}