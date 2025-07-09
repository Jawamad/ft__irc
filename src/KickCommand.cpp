#include "../inc/KickCommand.hpp"
#include "../inc/Server.hpp"

void KickCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string clientToKick;
	args >> channelName >> clientToKick;

	if (channelName.empty() ||  clientToKick.empty())
	{
		server.serverMessage(client, "461", "KICK :Not enough parameters");
		return;
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName); 

	if (!channel)
	{
		server.serverMessage(client, "403", "KICK :no such channel");
		return;	
	}

	if (!channel->isOperator(client->getSocketFd()))
	{
		server.serverMessage(client, "482", "KICK :You're not channel operator");
		return;
	}

	if (!channel->searchMember(clientToKick))
	{
		server.serverMessage(client, "441", "KICK :They aren't on that channel");
		return;
	}

	if (!channel->searchMember(client->getNickname()))
	{
		server.serverMessage(client, "442", channelName + "KICK :You're not on that channel");
		return;
	}

	Client* clientToKickPtr = server.findClientByNickname(clientToKick);
	if (!clientToKickPtr)
	{
		server.serverMessage(client, "401", "KICK :No such nick/channel");
		return;
	}

	std::string kickMsg =  channelName + " " + clientToKickPtr->getNickname();
	server.sendCommandMessage(client, "KICK", kickMsg, "");

	channel->clientGetsKickByOperator(client->getNickname(), *clientToKickPtr);
			
	channel->broadcast(kickMsg, -1);
}