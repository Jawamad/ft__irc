#include "../inc/UserCommand.hpp"
#include "../inc/Server.hpp"

void UserCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	(void) server;
	std::string username, unused1, unused2, realname;
	//check if unused has to be use or it s optional
	args >> username >> unused1 >> unused2;
	std::getline(args, realname);
	if (username.empty() || realname.empty())
	{
		std::string err = "461 USER :Not enough parameters\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}

	if (client->hasUser())
	{
		std::string err = "462 :You may not reregister\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}

	client->setUsername(username);
	client->setHasUser(true);

	if (client->hasUser() && client->hasNick())
	{
		client->setLoggedIn(true);
		std::string welcome = ":server 001 " + client->getNickname() + " :welcome to the IRC server \r\n";
		send(client->getSocketFd(), welcome.c_str(), welcome.size(), 0);
	}
}