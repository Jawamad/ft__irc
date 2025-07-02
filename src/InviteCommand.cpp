#include "../inc/InviteCommand.hpp"
#include "../inc/Server.hpp"

void InviteCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string clientToInvite;
	args >> channelName >> clientToInvite;

	if(channelName.empty())
	{
		server.errorMessage(client, 461, "INVITE :Not enough parameters");
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName); 
	Client* clientToInvitePtr = server.findClientByNickname(clientToInvite);

	if (!clientToInvitePtr)
	{
		server.errorMessage(client, 401, "INVITE :No such nick/channel");
	}
	
	if (channel)
	{	
		if (channel->isOperator(client->getSocketFd()))
		{
			channel->clientGetsInviteByOperator(client->getNickname(), *clientToInvitePtr);

			
			// RPL_INVITING au client qui invite -- success message ✅
			std::string rplInviting = ":server 341 " 
				+ client->getNickname() + " " 
				+ clientToInvitePtr->getNickname() + " " 
				+ channelName + "\r\n";
			send(client->getSocketFd(), rplInviting.c_str(), rplInviting.size(), 0);

			// Notification INVITE au client invité  -- success message ✅
			std::string inviteMsg = ":" 
				+ client->getNickname() + "!" 
				+ client->getUsername() + "@" 
				+ client->getIp()
				+ " INVITE " 
				+ clientToInvitePtr->getNickname() 
				+ " :" 
				+ channelName 
				+ "\r\n";
			send(clientToInvitePtr->getSocketFd(), inviteMsg.c_str(), inviteMsg.size(), 0);
		}
		else
		{
			server.errorMessage(client, 482, "INVITE :You're not channel operator");
		}
	}
	else
	{
		server.errorMessage(client, 403, "INVITE :No such channel");
	}
}