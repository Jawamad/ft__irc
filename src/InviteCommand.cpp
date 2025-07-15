#include "../inc/InviteCommand.hpp"
#include "../inc/Server.hpp"

void InviteCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string clientToInvite;
	args >> channelName >> clientToInvite;

	// test ::: OK ✅
	if(channelName.empty() || clientToInvite.empty())
	{
		server.errorMessage(client, "461", "INVITE", "Not enough parameters");
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName); 

	// test ::: OK ✅
	if (!channel)
	{
		server.errorMessage(client, "403", "INVITE", "No such channel");
		return;	
	}

	// test ::: OK ✅
	if (!channel->searchMember(client->getNickname()))
	{
		server.errorMessage(client, "442", channelName, "You're not on that channel");
		// server.errorMessage(client, "442", "INVITE", channelName + "You're not on that channel");
		return;
	}

	// test ::: OK ✅
	if (channel->searchMember(clientToInvite))
	{
		server.errorMessage(client, "443", "INVITE", "is already on channel");
		return;
	}

	// test ::: OK ✅
	if (!channel->isOperator(client->getSocketFd()))
	{
		server.errorMessage(client, "482", "INVITE", "You're not channel operator");
		return;
	}

	// test ::: OK ✅
	Client* clientToInvitePtr = server.findClientByNickname(clientToInvite);
	if (!clientToInvitePtr)
	{
		server.errorMessage(client, "401", clientToInvite, "No such nick/channel");
		// server.errorMessage(client, "401", "INVITE", "No such nick/channel");
		return;
	}

	channel->clientGetsInviteByOperator(client->getNickname(), *clientToInvitePtr);

	std::string msgSendNumber = clientToInvitePtr->getNickname() + " " + channelName;
	server.sendNumericReply(client, 341, msgSendNumber, "");

	server.sendCommandMessage(client, "INVITE", clientToInvitePtr->getNickname() , channelName);

	std::string inviteMsg = "invite";
	send(clientToInvitePtr->getSocketFd(), inviteMsg.c_str(), inviteMsg.size(), 0);
}
