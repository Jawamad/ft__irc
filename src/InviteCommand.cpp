#include "../inc/InviteCommand.hpp"
#include "../inc/Server.hpp"

void InviteCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string clientToInvite;
	args >> clientToInvite >> channelName;

	if (channelName.empty() || clientToInvite.empty())
	{
		server.errorMessage(client, "461", "INVITE", "Not enough parameters");
		return;
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName); 

	if (!channel)
	{
		server.errorMessage(client, "403", "INVITE", "No such channel");
		return;	
	}

	if (!channel->searchMember(client->getNickname()))
	{
		server.errorMessage(client, "442", channelName, "You're not on that channel");
		return;
	}

	if (channel->searchMember(clientToInvite))
	{
		server.errorMessage(client, "443", clientToInvite, "is already on channel");
		return;
	}

	if (!channel->isOperator(client->getSocketFd()))
	{
		server.errorMessage(client, "482", channelName, "You're not channel operator");
		return;
	}

	Client* clientToInvitePtr = server.findClientByNickname(clientToInvite);
	if (!clientToInvitePtr)
	{
		server.errorMessage(client, "401", clientToInvite, "No such nick/channel");
		return;
	}

	channel->clientGetsInviteByOperator(client->getNickname(), *clientToInvitePtr);

	server.sendNumericReply(client, 341, clientToInvite + " " + channelName, "");

	std::string inviteMsg = ":" + client->getNickname() + "!" +
		client->getUsername() + "@" + client->getIp() +
		" INVITE " + clientToInvite + " " + channelName + "\r\n";

	send(clientToInvitePtr->getSocketFd(), inviteMsg.c_str(), inviteMsg.size(), 0);
}

