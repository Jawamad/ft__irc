#include "../inc/KickCommand.hpp"
#include "../inc/Server.hpp"

void KickCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string clientToKick;
	args >> channelName >> clientToKick;

	if(channelName.empty())
	{
		server.errorMessage(client, 461, "KICK :Not enough parameters");
		return;
	}
	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName); 
	Client* clientToKickPtr = server.findClientByNickname(clientToKick);

	if (!clientToKickPtr)
	{
		server.errorMessage(client, 401, "KICK :No such nick/channel");
		return;
	}
	if (channel.searchMember(clientToKick))
	{
		// -- success message ✅
		if (channel->isOperator(client->getSocketFd()))
		{
			std::string kickMsg = ":" 
				+ client->getNickname() + "!" 
				+ client->getUsername() + "@" 
				+ client->getIp()
				+ " KICK " 
				+ channelName + " " 
				+ clientToKickPtr->getNickname()
				+ " :Kicked\r\n";
			channel->clientGetsKickByOperator(client->getNickname(), *clientToKickPtr);
			channel->broadcast(kickMsg, -1);
		}
		else
		{
			server.errorMessage(client, 482, "KICK :You're not channel operator");
			return;
		}
	}
	else
	{
		server.errorMessage(client, 441, "KICK :They aren't on that channel");
		return;
	}
	else
	{
		server.errorMessage(client, 403, "KICK :no such channel");
		return;	
	}
}