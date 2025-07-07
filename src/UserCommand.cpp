#include "../inc/UserCommand.hpp"
#include "../inc/Server.hpp"

void UserCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string username, unused1, unused2, realname;
	
	args >> username >> unused1 >> unused2;
	std::getline(args, realname);
	if (username.empty() || realname.empty())
	{
		std::string err = "USER :Not enough parameters";
		server.errorMessage(client, 461, err);
		return;
	}

	if (client->hasUser())
	{
		std::string err = "USER :You may not reregister";
		server.errorMessage(client, 462, err);
		return;
	}

	client->setUsername(username);
	client->setHasUser(true);
	client->logRoutine(server);
}