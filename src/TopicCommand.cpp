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

	if (newTopic.empty() || newTopic == " ")
	{
		if (channel->getTopic().empty())
			server.sendCommandMessage(client, "331", channelName, "No topic is set");
		else
			server.sendCommandMessage(client, "332", channelName, channel->getTopic());
		return;
	}

	if (!newTopic.empty() && newTopic[0] == ' ')
		newTopic = newTopic.substr(1);
	if (!newTopic.empty() && newTopic[0] == ':')
		newTopic = newTopic.substr(1);

	if (channel->topicIsOperatorModOnly() && !channel->isOperator(client->getSocketFd()))
	{
		server.errorMessage(client, "482", channelName, "You're not channel operator");
		return;
	}

	channel->setTopic(newTopic);
	std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname()
		+ " TOPIC " + channelName + " :" + newTopic + "\r\n";

	channel->broadcast(msg, -1);
}