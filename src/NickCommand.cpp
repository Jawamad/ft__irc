#include "../inc/NickCommand.hpp"
#include "../inc/Server.hpp"
#include <regex>

bool isValidNickname(const std::string &nickname)
{
	if (nickname.empty() || nickname.length() > 30)
		return false;

	// Les caractères autorisés
	const std::string special = "[]\\`_^{|}";

	// Le premier caractère doit être une lettre ou un caractère spécial
	char first = nickname[0];
	if (!std::isalpha(first) && special.find(first) == std::string::npos)
		return false;

	// Les suivants peuvent être lettre, chiffre, special ou '-'
	for (std::string::size_type i = 1; i < nickname.length(); ++i)
	{
		char c = nickname[i];
		if (!std::isalnum(c) && special.find(c) == std::string::npos && c != '-')
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
	// 432 : Erroneous nickname
	if (!isValidNickname(nickname))
	{
		std::string err = "432 * " + nickname + " :Erroneous nickname\r\n";
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

	if (client->hasUser() && client->hasNick() && client->hasPassedPassword())
	{
		client->setLoggedIn(true);
		std::string welcome = ":server 001 " + client->getNickname() + " :Welcome to the IRC server \r\n";
		send(client->getSocketFd(), welcome.c_str(), welcome.size(), 0);
	}
}