#include "../inc/ListCommand.hpp"
#include "../inc/Server.hpp"
#include "../inc/utils.hpp"

void ListCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	(void)args;

	std::string header = ":server 321 " + client->getNickname() + " Channel :Users Name\r\n";
	send(client->getSocketFd(), header.c_str(), header.size(), 0);

	std::map<std::string, Channel*> &channels = server.getChannels();
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* chan = it->second;
		std::string chanName = chan->getName();
		int userCount = chan->getClientCount();
		std::string topic = chan->getTopic();
		
		std::string line = ":server 322 " + client->getNickname() + " " + chanName + " " + intToString(userCount) + " : " + topic + "\r\n";
		send(client->getSocketFd(), line.c_str(), line.size(), 0);
	}
	std::string footer = ":server 323 " + client->getNickname() + " :End of /LIST\r\n";
	send(client->getSocketFd(), footer.c_str(), footer.size(), 0);
}