#include "../inc/TopicCommand.hpp"
#include "../inc/Server.hpp"

void TopicCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string newTopic;
	args >> channelName;
	std::getline(args, newTopic);

	if (channelName.empty())
	{
		server.errorMessage(client, "461", "TOPIC", "Not enough parameters");
		return;
	}

	if (channelName[0] != '#')
	 	channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName);

	if (!channel)
	{
		server.errorMessage(client, "403", "TOPIC", "No such channel");
		return;
	}

	if (!channel->searchMember(client->getNickname()))
	{
		server.errorMessage(client, "442", channelName, "You're not on that channel");
		return;
	}

	// Si aucun nouveau topic : affichage du topic actuel
	if (newTopic.empty() || newTopic == " ")
	{
		if (channel->getTopic().empty())
			server.sendCommandMessage(client, "331", channelName, "No topic is set");
		else
			server.sendCommandMessage(client, "332", channelName, channel->getTopic());
		return;
	}

	// Retirer le ":" du début si présent (venant de IRC)
	if (!newTopic.empty() && newTopic[0] == ' ')
		newTopic = newTopic.substr(1);
	if (!newTopic.empty() && newTopic[0] == ':')
		newTopic = newTopic.substr(1);

	// Vérification des droits si +t est activé || topicIsOperatorModOnly()  do not exists
	if (channel->topicIsOperatorModOnly() && !channel->isOperator(client->getSocketFd()))
	{
		server.errorMessage(client, "482", channelName, "You're not channel operator");
		return;
	}

	// Mise à jour du topic
	channel->setTopic(newTopic);
	std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp()
		+ " TOPIC " + channelName + " :" + newTopic + "\r\n";

	// Broadcast du changement à tous les membres du channel
	channel->broadcast(msg, -1);
}


// void TopicCommand::execute(Server &server, Client *client, std::istringstream &args)
// {
// 	std::string channelName;
// 	std::string newTopic;
// 	args >> channelName >> newTopic;

// 	if (channelName.empty())
// 	{
// 		server.errorMessage(client, "461", "TOPIC", "Not enough parameters");
// 		return;
// 	}

// 	if (channelName[0] != '#')
// 	 	channelName = '#' + channelName;

// 	Channel* channel = server.getChan(channelName);

// 	if (!channel)
// 	{
// 		server.errorMessage(client, "403", "TOPIC", "No such channel");
// 		return;	
// 	}
	
// 	if (!channel->searchMember(client->getNickname()))
// 	{
// 		server.errorMessage(client, "442", channelName, "You're not on that channel");
// 		return;
// 	}

// 	if (!channel->isOperator(client->getSocketFd()))
// 	{
// 		server.errorMessage(client, "482", "TOPIC", "You're not channel operator");
// 		return;
// 	}

// 	if(!newTopic.empty())
// 	{
// 		if (!channel->topicIsOperatorModOnly())
// 		{
// 			channel->setTopic(newTopic);
// 			std::cout << "Topic of channel updated to : " << channel->getTopic() << std::endl;
// 		}
// 		if (channel->isOperator(client->getSocketFd())) 
// 		{
// 			channel->setTopic(newTopic);
// 			std::cout << "Topic of channel updated to (by operator) : " << channel->getTopic() << std::endl;
// 		}
// 	} 
// 	else 
// 	{
// 		if (channel->getTopic().empty())
// 			server.sendCommandMessage(client, "TOPIC :No topic is set", "331", "");
// 		else
// 			server.sendCommandMessage(client, "TOPIC :" + channel->getTopic(), "332","");
// 		return;
// 	}
// }
