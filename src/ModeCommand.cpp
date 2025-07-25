#include "../inc/ModeCommand.hpp"
#include "../inc/Server.hpp"
#include <cstdlib>
#include <algorithm>

void ModeCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string modeLetter;
	std::string modeValue;

	args >> channelName >> modeLetter >> modeValue;

	if(channelName.empty())
	{
		server.errorMessage(client, "461", "MODE", "Not enough parameters");	
		return;
	}

	if (channelName[0] != '#' || !server.hasChannel(channelName))
	{
		server.errorMessage(client, "403", "MODE", "No such channel");
		return;
	}
	
	Channel* channel = server.getChan(channelName); 
	
	if (modeLetter.empty() && modeValue.empty())
	{
		std::string modes = "+";
		if (channel->isInviteOnly())
			modes += "i";
		if (channel->topicIsOperatorModOnly())
			modes += "t";
		if (channel->isPasswordOnly())
			modes += "k";
		if (channel->isLimitedNbUser())
			modes += "l";

		std::string rplMode = ":" + server.getName() + " 324 " + client->getNickname() + " " + channelName + " " + modes + "\r\n";
		send(client->getSocketFd(), rplMode.c_str(), rplMode.size(), 0);

		std::ostringstream oss;
		oss << ":" << server.getName() << " 329 " << client->getNickname() << " " << channelName << " " << channel->getCreationTime() << "\r\n";
		std::string rplCreationTime = oss.str();
		send(client->getSocketFd(), rplCreationTime.c_str(), rplCreationTime.size(), 0);
		return;
	}

	if (!channel->hasClient(client->getSocketFd()))
	{
		server.errorMessage(client, "442", "MODE", "You're not on that channel");
		return;
	}

	if (modeLetter != "+i" && modeLetter != "-i" && modeLetter != "+t" && modeLetter != "-t"
		&& modeLetter != "+k" && modeLetter != "-k" && modeLetter != "+l" && modeLetter != "-l"
		&& modeLetter != "+o" && modeLetter != "-o"	)
	{
		server.errorMessage(client, "421", "MODE", "Unknown command");
		return;
	}

	if (!channel->isOperator(client->getSocketFd()))
	{
		server.errorMessage(client, "482", "MODE", "You're not channel operator");
		return;
	}

	std::string response = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " MODE " + channelName + " " + modeLetter;
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
		std::string responsePwd = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " MODE " + channelName + " " + modeLetter + "\r\n";
		channel->setPasswordStatus(true);
		channel->setChanPassword(modeValue);
		channel->broadcast(responsePwd, -1);
	}

	else if (modeLetter == "-k")
	{
		channel->setPasswordStatus(false);
		channel->setChanPassword("");
		channel->broadcast(response, -1);
	}

	else if (modeLetter == "+l")
	{
		int userLimit = std::atoi(modeValue.c_str());
		if (userLimit <= 0)
		{
			server.errorMessage(client, "467", channelName, "Invalid limit mode parameter");
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
		Client* target = server.findClientByNickname(modeValue);
		if (!target)
		{
			server.errorMessage(client, "401", modeValue, "No such nick");
			return;
		}

		if (!channel->hasClient(target->getSocketFd()))
		{
			server.errorMessage(client, "441", modeValue, "They aren't on that channel");
			return;
		}

		if (modeLetter == "+o")
			channel->addOperator(target->getSocketFd());
		else
			channel->removeOperator(target->getSocketFd());

		std::string targetNick = target->getNickname();
		targetNick.erase(targetNick.find_last_not_of("\r\n") + 1);

		std::string response = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname()
			+ " MODE " + channelName + " " + modeLetter + " " + targetNick + "\r\n";

		channel->broadcast(response, -1);
	}

	else
	{
		server.serverMessage(client, "472", modeLetter + "MODE :is unknown mode char to me");
	}
}