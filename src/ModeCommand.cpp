#include "../inc/ModeCommand.hpp"
#include "../inc/Server.hpp"
#include <cstdlib>

void ModeCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string modeletter;
	std::string modeValue;

	args >> channelName >> modeletter >> modeValue;

	if(channelName.empty() || modeletter.empty())
	{
		server.errorMessage(client, 461, "MODE :Not enough parameters");
		return;
	}
	
	if (channelName[0] != '#')
		channelName = '#' + channelName;

	if (!server.hasChannel(channelName))
	{
		server.errorMessage(client, 403, "MODE :No such channel");
		return;
	}

	if (!channel->hasClient(client->getSocketFd()))
	{
		server.errorMessage(client, 442, "MODE :You're not on that channel");
		return;
	}

	if (!server)
	{
		server.errorMessage(client, 402, "MODE :No such server");
		return;
	}
	
	if (modeletter == "+i" || modeletter == "-i" 
		|| modeletter == "+t" || modeletter == "-t"
		|| modeletter == "+k" || modeletter == "-k"
		|| modeletter == "+l" || modeletter == "-l"
	)
	{
		server.errorMessage(client, 421, "MODE :Unknown command");
		return;
	}

	if (!channel->isOperator(client->getSocketFd()))
	{
		server.errorMessage(client, 482, "MODE :You're not channel operator");
		return;
	}

	Channel* channel = server.getChan(channelName);

	std::string response = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " MODE " + channelName + " " + modeLetter;
	if (!modeValue.empty())
		response += " " + modeValue;
	response += "\r\n";

	if (modeletter == "+i")
	{
		channel->setInviteOnly(true);
		channel->broadcast(response, -1);
	}
	else if (modeletter == "-i")
	{
		channel->setInviteOnly(false);
		channel->broadcast(response, -1);
	}
	else if (modeletter == "+t")
	{
		channel->setTopicStatus(true);
		channel->broadcast(response, -1);
	}
	else if (modeletter == "-t")
	{
		channel->setTopicStatus(false);
		channel->broadcast(response, -1);
	}
	else if (modeletter == "+k")
	{
		if (modeValue.empty())
		{
			server.errorMessage(client, 461, "MODE :Not enough parameters");
			return;
		}
		channel->setPasswordStatus(true);
		channel->setChanPassword(modeValue);
		channel->broadcast(response, -1);
	}
	else if (modeletter == "-k")
	{
		channel->setPasswordStatus(false);
		channel->setChanPassword("");
		channel->broadcast(response, -1);
	}
	else if (modeletter == "+l")
	{
		if (modeValue.empty())
		{
			server.errorMessage(client, 461, "MODE :Not enough parameters");
			return;
		}
		int userLimit = std::atoi(modeValue.c_str());
		if (userLimit <= 0)
		{
			server.errorMessage(client, 467, channelName + "MODE :Invalid limit mode parameter");
			return;
		}
		channel->setLimitedNbUser(true);
		channel->setUserLimit(userLimit);
		channel->broadcast(response, -1);
	} 
	else if (modeletter == "-l")
	{
		channel->setLimitedNbUser(false);
		channel->setUserLimit(0);
		channel->broadcast(response, -1);

	}
	else if (modeLetter == "+o" || modeLetter == "-o")
	{
		if (modeValue.empty())
		{
			server.errorMessage(client, 461, "MODE :Not enough parameters");
			return;
		}

		Client* target = server.findClientByNickname(modeValue);
		if (!target)
		{
			server.errorMessage(client, 401, modeValue + "MODE :No such nick");
			return;
		}

		if (!channel->hasClient(target->getSocketFd()))
		{
			server.errorMessage(client, 441, modeValue + " " + channelName + "MODE :They aren't on that channel");
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
		server.errorMessage(client, 472, modeLetter + "MODE :is unknown mode char to me");
	}
	
}