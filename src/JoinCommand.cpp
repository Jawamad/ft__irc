#include "../inc/JoinCommand.hpp"
#include "../inc/Server.hpp"

void JoinCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string channelPassword;
	args >> channelName >> channelPassword;

	if(channelName.empty())
	{
		server.serverMessage(client, "461", "JOIN : Not enough parameters");
		return;
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;
	
	std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " JOIN " + channelName + "\r\n";
	std::map<std::string, Channel*>& channels = server.getChannels();
	Channel* channel;

	if (channels.find(channelName) == channels.end())
	{
		channel  = new Channel(channelName);
		channels[channelName] = channel;
		channel->addClient(client);
		channel->addOperator(client->getSocketFd());
	}
	else
	{
		channel = channels[channelName];
		if (channel->isLimitedNbUser() && channel->getClientCount() >= channel->getUserLimit()) {
			server.serverMessage(client, "471", "JOIN :Cannot join channel (+l)");
			return;
		}
		
		if (channel->isInviteOnly() && !channel->isInvite(client))
		{
			server.serverMessage(client, "473", "JOIN :Cannot join channel (+i)");
			return;
		}
		if (channel->isPasswordOnly())
		{
			if (channel->getChanPassword() != channelPassword)
			{
				server.serverMessage(client, "475", "JOIN :Cannot join channel (+k)");
				return;
			}
		}
		channel->addClient(client);
	}
	client->delInvitedList(channelName);
	channel->delInviteList(client);
	server.getChan(channelName)->broadcast(joinMsg, -1);

	std::map<int, Client*> members = server.getChan(channelName)->getClients();
	std::string namesList;
	for (std::map<int, Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		if (!it->second->getNickname().empty())
		{
			if (!namesList.empty())
				namesList += " ";
			if (channels[channelName]->isOperator(it->second->getSocketFd()))
				namesList += "@";
			namesList += it->second->getNickname();
		}
	}
	std::string rplNames = ":" + server.getName() + " 353 " + client->getNickname() + " = " + channelName + " :" + namesList + "\r\n";
	std:: string rplEndNames = ":" + server.getName() + " 366 " + client->getNickname() + " " + channelName + " :End of /NAMES list\r\n";
	send(client->getSocketFd(), rplNames.c_str(), rplNames.size(), 0);
	send(client->getSocketFd(), rplEndNames.c_str(), rplEndNames.size(), 0);
}