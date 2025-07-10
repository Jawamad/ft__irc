#include "../inc/InviteCommand.hpp"
#include "../inc/Server.hpp"

void InviteCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string clientToInvite;
	args >> channelName >> clientToInvite;

	if(channelName.empty() || clientToInvite.empty())
	{
		server.serverMessagetest(client, "461", "INVITE", "Not enough parameters");
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName); 

	if (!channel)
	{
		server.serverMessagetest(client, "403", "INVITE", "No such channel");
		return;	
	}
	
	if (!channel->searchMember(client->getNickname()))
	{

		server.serverMessagetest(client, "442", "INVITE", channelName + "You're not on that channel");
		return;
	}

	if (!channel->isOperator(client->getSocketFd()))
	{
		server.serverMessagetest(client, "482", "INVITE", "You're not channel operator");
		return;
	}

	if (channel->searchMember(clientToInvite))
	{
		server.serverMessagetest(client, "443", "INVITE", "is already on channel");
		return;
	}

	Client* clientToInvitePtr = server.findClientByNickname(clientToInvite);
	if (!clientToInvitePtr)
	{
		server.serverMessagetest(client, "401", "INVITE", "No such nick/channel");
		return;
	}

	channel->clientGetsInviteByOperator(client->getNickname(), *clientToInvitePtr);

	std::string msgSendNumber = clientToInvitePtr->getNickname() + " " + channelName;
	server.sendNumericReply(client, 341, msgSendNumber, "");

	server.sendCommandMessage(client, "INVITE", clientToInvitePtr->getNickname() , channelName);

	std::string inviteMsg = "invite";
	send(clientToInvitePtr->getSocketFd(), inviteMsg.c_str(), inviteMsg.size(), 0);
}
