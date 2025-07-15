#include "../inc/NickCommand.hpp"
#include "../inc/Server.hpp"

bool isValidNickname(const std::string &nick)
{
	if (nick.empty() || !(isalpha(nick[0]) || std::strchr("[]\\`^{}|_", nick[0])))
		return false;
	for (size_t i = 1; i < nick.size(); ++i)
	{
		if (!isalnum(nick[i]) && !std::strchr("[]\\`^{}|-_", nick[i]))
			return false;
	}
	return true;
}

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
	if (nickname.length() > 30 || !isValidNickname(nickname))
	{
		std::string err = "432 " + client->getNickname() + " " + nickname + " :Erroneous nickname\r\n";
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