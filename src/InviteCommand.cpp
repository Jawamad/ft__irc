#include "../inc/InviteCommand.hpp"
#include "../inc/Server.hpp"

void InviteCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string clientToInvite;
	args >> channelName >> clientToInvite;

	if(channelName.empty())
	{
		std::string err = "461 INVITE :Not enough parameters\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}

	// Ajoute le # si absent
	if (channelName[0] != '#')
		channelName = '#' + channelName;

	std::cout << "INPUT " << channelName << " ! " << std::endl;

	Channel* channel = server.getChan(channelName); 
	Client* clientToInvitePtr = server.findClientByNickname(clientToInvite);

	if (!clientToInvitePtr)
	{
		std::cout << "Client " << clientToInvite << " not found." << std::endl;
		return;
	}
	
	if (channel)
	{
		std::cout << "FIND " << channelName << " ! " << std::endl;
		//////////
		std::cout << "operator :" << channel->isOperator(client->getSocketFd()) << std::endl;
		
		if (channel->isOperator(client->getSocketFd()))
		{   
			channel->clientGetsInviteByOperator(client->getNickname(), *clientToInvitePtr);
			std::cout << clientToInvitePtr->getNickname() << " is the client to invite" << std::endl;
			std::cout << client->getNickname() << " can INVITE " << clientToInvite << " ! " << std::endl;
		}
		else
		{
			std::cout << "Permission denied: " << client->getNickname() << " is not operator." << std::endl;
		}
	}
	else
	{
		std::cout << "Channel " << channelName << " not found." << std::endl;
	}
}