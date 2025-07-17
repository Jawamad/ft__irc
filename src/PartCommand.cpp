#include "../inc/PartCommand.hpp"
#include "../inc/Server.hpp"

void PartCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	args >> channelName;

	if(channelName.empty())
	{
		server.serverMessage(client, "461", "PART :Not enough parameters");
		return;
	}
	std::map<std::string, Channel*> channels = server.getChannels();
	if (channels.find(channelName) == channels.end())
	{
		std::string err = ":" + server.getName() + " 403 " + client->getNickname() + " " + channelName + " :PART :No such channel\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}

	Channel* channel = server.getChan(channelName);

	if (!channel->hasClient(client->getSocketFd()))
	{
		std::string err = ":" + server.getName() + " 442 " + client->getNickname() + " " + channelName + " :PART :You're not on that channel\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}
	std::string partMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " PART " + channelName + ":leaving\r\n";
	std::cout << partMsg << std::endl;
	channel->broadcast(partMsg, -1);
	
	channel->removeClient(client->getSocketFd());
	if (channel->getClients().empty())
		server.delChannel(channelName);
}