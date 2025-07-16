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

	// test ::: OK ✅ 
	if(channelName.empty() || modeLetter.empty())
	{
		server.errorMessage(client, "461", "MODE", "Not enough parameters");	
		return;
	}
	
	if (channelName[0] != '#')
		channelName = '#' + channelName;

	// test ::: OK ✅ 
	if (!server.hasChannel(channelName))
	{
		server.errorMessage(client, "403", "MODE", "No such channel");
		return;
	}

	Channel* channel = server.getChan(channelName); 

	// test ::: OK ✅ 
	if (!channel->hasClient(client->getSocketFd()))
	{
		server.errorMessage(client, "442", "MODE", "You're not on that channel");
		return;
	}

	// test ::: OK ✅ 
	if (modeLetter != "+i" && modeLetter != "-i" && modeLetter != "+t" && modeLetter != "-t"
		&& modeLetter != "+k" && modeLetter != "-k" && modeLetter != "+l" && modeLetter != "-l"
		&& modeLetter != "+o" && modeLetter != "-o"	)
	{
		server.errorMessage(client, "421", "MODE", "Unknown command");
		return;
	}

	// test ::: OK ✅
	if (!channel->isOperator(client->getSocketFd()))
	{
		server.errorMessage(client, "482", "MODE", "You're not channel operator");
		return;
	}

	std::string response = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " MODE " + channelName + " " + modeLetter;
	if (!modeValue.empty())
		response += " " + modeValue;
	response += "\r\n";

	// test ::: OK ✅
	if (modeLetter == "+i")
	{
		channel->setInviteOnly(true);
		channel->broadcast(response, -1);
	}
	// test ::: OK ✅
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
	// test ::: OK ✅
	else if (modeLetter == "+k")
	{
		channel->setPasswordStatus(true);
		channel->setChanPassword(modeValue);
		std::cout << "key mode ON" << std::endl;
		channel->broadcast(response, -1);
	}
	// test ::: OK ✅
	else if (modeLetter == "-k")
	{
		channel->setPasswordStatus(false);
		channel->setChanPassword("");
		channel->broadcast(response, -1);
	}
	// test ::: OK ✅
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
	// test ::: OK ✅
	else if (modeLetter == "-l")
	{
		channel->setLimitedNbUser(false);
		channel->setUserLimit(0);
		channel->broadcast(response, -1);
	}

	// else if (modeLetter == "+o")
	// {
	// 	channel->addOperator(target->getSocketFd());
	// }

	// else if (modeLetter == "+o")
	// {
	// 	std::cout << "REMOVE OPERATOR" << std::endl;
	// 	channel->removeOperator(target->getSocketFd());
	// }


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
		{
			std::cout << "ADD OPERATOR" << std::endl;
			channel->addOperator(target->getSocketFd());
		}
		else
		{
			std::cout << "REMOVE OPERATOR" << std::endl;
			channel->removeOperator(target->getSocketFd());
		}

		// Nettoyage du nickname cible
		std::string targetNick = target->getNickname();
		targetNick.erase(targetNick.find_last_not_of("\r\n") + 1);

		// Construction du message MODE
		std::string response = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp()
			+ " MODE " + channelName + " " + modeLetter + " " + targetNick + "\r\n";

		channel->broadcast(response, -1);

		Envoi du message à tous les membres du canal
		const std::map<int, Client*>& members = channel->getClients();
		for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			send(it->second->getSocketFd(), response.c_str(), response.size(), 0);
		}
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