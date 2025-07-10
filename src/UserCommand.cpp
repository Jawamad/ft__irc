#include "../inc/UserCommand.hpp"
#include "../inc/Server.hpp"

void UserCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string username, unused1, unused2, realname, line;
	
	args >> username >> unused1 >> unused2;
	std::getline(args, line);
	std::size_t colon = line.find(':');
	realname = (colon != std::string::npos) ? line.substr(colon + 1) : "";
	if (username.empty() || realname.empty())
	{
		std::string err = "USER :Not enough parameters";
		server.serverMessage(client, "461", err);
		return;
	}
	if (username.length() > 12)
	{
		std::string err = username + ":Erroneous username";
		server.serverMessage(client, "432", err);
	}

	if (client->hasUser())
	{
		std::string err = "USER :You may not reregister";
		server.serverMessage(client, "462", err);
		return;
	}

	client->setUsername(username);
	client->setHasUser(true);
	client->logRoutine(server);
}