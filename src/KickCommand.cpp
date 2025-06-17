#include "../inc/KickCommand.hpp"
#include "../inc/Server.hpp"

void KickCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string clientToKick;
	args >> channelName >> clientToKick;

	if(channelName.empty())
	{
		std::string err = "461 KICK :Not enough parameters\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName); 
	Client* clientToKickPtr = server.findClientByNickname(clientToKick);

	if (!clientToKickPtr)
	{
		std::cout << "Client " << clientToKick << " not found." << std::endl;
		return;
	}
	
	if (channel)
	{	
		if (channel->isOperator(client->getSocketFd()))
		{   
			channel->clientGetsKickByOperator(client->getNickname(), *clientToKickPtr);
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