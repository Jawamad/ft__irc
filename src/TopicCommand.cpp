#include "../inc/TopicCommand.hpp"
#include "../inc/Server.hpp"

void TopicCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string newTopic;
	args >> channelName >> newTopic;

	(void)server;
	(void)client;

	if (channelName[0] != '#')
	 	channelName = '#' + channelName;
	Channel* channel = server.getChan(channelName); 

	if(!newTopic.empty() && !channel->topisIsOperatorModOnly())
	{
		channel->setTopic(newTopic);
		std::cout << "Topic of channel updated to : " << channel->getTopic() << std::endl;
	} 
	else 
	{
		if (channel->getTopic().empty())
			std::cout << "the current topic of the channel is EMPTY" << std::endl;
		else
			std::cout << "the current topic of the channel is : " << channel->getTopic() << std::endl;
		return;
	}
}

	if (!channel->topisIsOperatorModOnly())
			channel->setTopic(newTopic);
		else if (channel->topisIsOperatorModOnly())
		{
			std::cout << "the topic can be modified only by an operator " << std::endl;
		}