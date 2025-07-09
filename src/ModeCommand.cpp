#include "../inc/ModeCommand.hpp"
#include "../inc/Server.hpp"
#include <cstdlib>

void ModeCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string modeLetter;
	std::string modeValue;

	args >> channelName >> modeLetter >> modeValue;

	if(channelName.empty() || modeLetter.empty())
	{
		server.serverMessage(client, "461", "MODE :Not enough parameters");
		return;
	}
	
	if (channelName[0] != '#')
		channelName = '#' + channelName;

	if (!server.hasChannel(channelName))
	{
		server.serverMessage(client, "403", "MODE :No such channel");
		return;
	}

	Channel* channel = server.getChan(channelName); 

	if (!channel->hasClient(client->getSocketFd()))
	{
		server.serverMessage(client, "442", "MODE :You're not on that channel");
		return;
	}
	
	if (modeLetter != "+i" || modeLetter != "-i" || modeLetter != "+t" || modeLetter != "-t"
		|| modeLetter != "+k" || modeLetter != "-k" || modeLetter != "+l" || modeLetter != "-l")
	{
		server.serverMessage(client, "421", "MODE :Unknown command");
		return;
	}

	if (!channel->isOperator(client->getSocketFd()))
	{
		server.serverMessage(client, "482", "MODE :You're not channel operator");
		return;
	}

	std::string response = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " MODE " + channelName + " " + modeLetter;
	if (!modeValue.empty())
		response += " " + modeValue;
	response += "\r\n";

	if (modeLetter == "+i")
	{
		channel->setInviteOnly(true);
		channel->broadcast(response, -1);
	}
	else if (modeLetter == "-i")
	{
		channel->setInviteOnly(false);
		channel->broadcast(response, -1);
	}
	else if (modeLetter == "+t")
	{
		channel->setTopicStatus(true);
		channel->broadcast(response, -1);
	}
	else if (modeLetter == "-t")
	{
		channel->setTopicStatus(false);
		channel->broadcast(response, -1);
	}
	else if (modeLetter == "+k")
	{
		// if (modeValue.empty())
		// {
		// 	server.errorMessage(client, 461, "MODE :Not enough parameters");
		// 	return;
		// }
		channel->setPasswordStatus(true);
		channel->setChanPassword(modeValue);
		channel->broadcast(response, -1);
	}
	else if (modeLetter == "-k")
	{
		channel->setPasswordStatus(false);
		channel->setChanPassword("");
		channel->broadcast(response, -1);
	}
	else if (modeLetter == "+l")
	{
		// if (modeValue.empty())
		// {
		// 	server.errorMessage(client, 461, "MODE :Not enough parameters");
		// 	return;
		// }
		int userLimit = std::atoi(modeValue.c_str());
		if (userLimit <= 0)
		{
			server.serverMessage(client, "467", channelName + "MODE :Invalid limit mode parameter");
			return;
		}
		channel->setLimitedNbUser(true);
		channel->setUserLimit(userLimit);
		channel->broadcast(response, -1);
	} 
	else if (modeLetter == "-l")
	{
		channel->setLimitedNbUser(false);
		channel->setUserLimit(0);
		channel->broadcast(response, -1);

	}
	else if (modeLetter == "+o" || modeLetter == "-o")
	{
		// if (modeValue.empty())
		// {
		// 	server.errorMessage(client, 461, "MODE :Not enough parameters");
		// 	return;
		// }

		Client* target = server.findClientByNickname(modeValue);
		if (!target)
		{
			server.serverMessage(client, "401", modeValue + "MODE :No such nick");
			return;
		}

		if (!channel->hasClient(target->getSocketFd()))
		{
			server.serverMessage(client, "441", modeValue + " " + channelName + "MODE :They aren't on that channel");
			return;
		}

		if (modeLetter == "+o")
			channel->addOperator(target->getSocketFd());
		else
			channel->removeOperator(target->getSocketFd());

		channel->broadcast(response, -1);
	}
	else
	{
		server.serverMessage(client, "472", modeLetter + "MODE :is unknown mode char to me");
	}
}

//RPL_CHANNELMODEIS(324)
//RPL_CREATIONTIME (329)
// si user = âs de broadcast
// si mode besoin de params mais y en a pas --> on ignore