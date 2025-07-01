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
			std::cout << clientToInvitePtr->getNickname() << " is the client to invite" << std::endl;
			std::cout << client->getNickname() << " can INVITE " << clientToInvite << " ! " << std::endl;
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