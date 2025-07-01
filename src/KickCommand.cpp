#include "../inc/KickCommand.hpp"
#include "../inc/Server.hpp"

void KickCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string clientToKick;
	args >> channelName >> clientToKick;

	if(channelName.empty())
	{
		server.errorMessage(client, 461, "KICK :Not enough parameters");
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName); 
	Client* clientToKickPtr = server.findClientByNickname(clientToKick);

	if (!clientToKickPtr)
	{
		server.errorMessage(client, 401, "KICK :No such nick/channel");
	}
	if (channel)
	{	
		if (channel->isOperator(client->getSocketFd()))
			channel->clientGetsKickByOperator(client->getNickname(), *clientToKickPtr);
		else
		{
			
			std::string err = ":" + serverName + " 482 " + client->getNickname() + " KICK :You're not channel operator\r\n";
			send(client->getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}
	}
	else
	{
		std::string err = ":" + serverName + " 442 " + client->getNickname() + " KICK :You are not on that channel\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;	
	}
}