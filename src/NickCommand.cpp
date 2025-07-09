#include "../inc/NickCommand.hpp"
#include "../inc/Server.hpp"

void NickCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string nickname;
	args >> nickname;
	if (nickname.empty())
	{
		std::string err = "431 :No nickname given\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}
	for (std::map<int, Client*>::const_iterator it = server.getClients().begin(); it != server.getClients().end(); ++it)
	{
		if (it->second->getNickname() == nickname)
		{
			std::string err = "433 * " + nickname + " :Nickname is already in use\r\n";
			send(client->getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}
	}
	client->setNickname(nickname);
	client->setHasNick(true);
	client->logRoutine(server);
}