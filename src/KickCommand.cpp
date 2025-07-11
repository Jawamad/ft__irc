#include "../inc/KickCommand.hpp"
#include "../inc/Server.hpp"

void KickCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string clientToKick;
	args >> channelName >> clientToKick;

	if (channelName.empty() ||  clientToKick.empty())
	{
		server.errorMessage(client, "461", "KICK", "Not enough parameters");
		return;
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName); 

	if (!channel)
	{
		server.errorMessage(client, "403", "KICK", "No such channel");
		return;	
	}

	if (!channel->isOperator(client->getSocketFd()))
	{
		server.errorMessage(client, "482", "KICK", "You're not channel operator");
		return;
	}

	if (!channel->searchMember(clientToKick))
	{
		server.errorMessage(client, "441", "KICK", "They aren't on that channel");
		return;
	}

	if (!channel->searchMember(client->getNickname()))
	{

		server.errorMessage(client, "442", channelName, "You're not on that channel");
		return;
	}

	Client* clientToKickPtr = server.findClientByNickname(clientToKick);
	if (!clientToKickPtr)
	{
		server.errorMessage(client, "401", "KICK", "No such nick/channel");
		return;
	}

	// CE BLOQUE LA FONCTIONNE 
	std::string kickParams = channelName + " " + clientToKickPtr->getNickname();
	std::string reason = "Kicked by operator";

	// Envoyer le KICK aux deux clients : kicker et kické
	server.sendCommandMessage(client, "KICK", kickParams, reason);
	server.sendCommandMessage(clientToKickPtr, "KICK", kickParams, reason);

	channel->clientGetsKickByOperator(client->getNickname(), *clientToKickPtr);
			
	channel->broadcast(kickParams, -1);
}