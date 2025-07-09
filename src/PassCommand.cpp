#include "../inc/PassCommand.hpp"
#include "../inc/Server.hpp"

void PassCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string providedPass;
	args >> providedPass;
	if (client->hasPassedPassword())
	{
		std::string err = "462 :You may not reregister\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}
	if (providedPass.empty())
	{
		std::string err = "461 PASS :Not enough parameters\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}
	if (providedPass != server.getPassword())
	{
		std::string err = "464 PASS :Password incorrect\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		close(client->getSocketFd());
		server.delClient(client->getSocketFd());
		return;
	}
	client->setHaspassedPassword(true);
	client->logRoutine(server);
}