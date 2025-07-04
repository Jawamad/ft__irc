#include "../inc/InviteCommand.hpp"
#include "../inc/Server.hpp"

void InviteCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string clientToInvite;
	args >> channelName >> clientToInvite;

	if(channelName.empty() || clientToInvite.empty())
	{
		server.errorMessage(client, 461, "INVITE :Not enough parameters");
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName); 

	if (!channel)
	{
		server.errorMessage(client, 403, "INVITE :no such channel");
		return;	
	}
	
	if (!channel->searchMember(client->getNickname()))
	{
		server.errorMessage(client, 442, channelName + "INVITE :You're not on that channel");
		return;
	}

	if (!channel->isOperator(client->getSocketFd()))
	{
		server.errorMessage(client, 482, "INVITE :You're not channel operator");
		return;
	}

	if (channel->searchMember(clientToInvite))
	{
		server.errorMessage(client, 443, "INVITE :is already on channel");
		return;
	}

	Client* clientToInvitePtr = server.findClientByNickname(clientToInvite);
	if (!clientToInvitePtr)
	{
		server.errorMessage(client, 401, "INVITE :No such nick/channel");
		return;
	}

	channel->clientGetsInviteByOperator(client->getNickname(), *clientToInvitePtr);

	std::string msgSendNumber = clientToInvitePtr->getNickname() + " " + channelName;
	server.sendNumericReply(client, 341, msgSendNumber, "");

	server.sendCommandMessage(client, "INVITE", clientToInvitePtr->getNickname() , channelName);

	std::string inviteMsg = "invite";
	send(clientToInvitePtr->getSocketFd(), inviteMsg.c_str(), inviteMsg.size(), 0);
}
