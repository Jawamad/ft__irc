#include "../inc/QuitCommand.hpp"
#include "../inc/Server.hpp"

void QuitCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string quitMessage;
	std::getline(args, quitMessage);
	if (!quitMessage.empty() && (quitMessage[0] == ' ' || quitMessage[0] == ':'))
		quitMessage = quitMessage.substr(1);
	if (quitMessage.empty())
		quitMessage = "Client quit";
	std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " QUIT :" + quitMessage + "\r\n";
	std::map<std::string, Channel*> channels = server.getChannels();
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* chan = it->second;
		if (chan->hasClient(client->getSocketFd()))
		{
			chan->broadcast(msg, client->getSocketFd());
			server.getChan(it->first)->removeClient(client->getSocketFd());

			if (chan->getClients().empty())
				server.delChannel(it->first);
		}
	}
	server.delClient(client->getSocketFd());
}