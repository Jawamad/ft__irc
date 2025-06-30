#include "../inc/JoinCommand.hpp"
#include "../inc/Server.hpp"

void JoinCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string channelPassword;
	args >> channelName >> channelPassword;

	if(channelName.empty())
	{
		std::string err = "461 JOIN :Not enough parameters\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}
	if (channelName[0] != '#')
		channelName = '#' + channelName;
	
	std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " JOIN " + channelName + "\r\n";
	std::map<std::string, Channel*>& channels = server.getChannels();
	if (channels.find(channelName) == channels.end())
	{
		channels[channelName] = new Channel(channelName);
		channels[channelName]->addClient(client);
		channels[channelName]->addOperator(client->getSocketFd());
		std::cout << "New channel " << channelName << " created by " << client->getNickname() << std::endl;
	}
	else
	{
		Channel* channel = channels[channelName];

		// Vérifie si le canal est plein
		if (channel->isLimitedNbUser() && channel->getClientCount() >= channel->getUserLimit()) {
			std::cout << "Ce canal a atteint le nombre maximal de user" << std::endl;
			return;
		}
		else if (!channel->isInviteOnly())
		{
			channel->addClient(client);
			server.getChan(channelName)->broadcast(joinMsg, -1);
		}
		else if (channel->isInviteOnly())
		{
			std::cout << "join this channel is only on invitation " << std::endl;
		}
		// MODE k+ password required
		else if (channel->isPasswordOnly())
		{
			if (channel->getChanPassword() == channelPassword)
			{
				channel->addClient(client);
				server.getChan(channelName)->broadcast(joinMsg, -1);
			}
			//message erreur
		} 
	}

	std::string namesList;
	std::map<int, Client*> members = server.getChan(channelName)->getClients();
	for (std::map<int, Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		if (!it->second->getNickname().empty())
		{
			namesList += it->second->getNickname() + " ";
		}
	}
	if (!namesList.empty() && namesList[namesList.size() - 1] == ' ')
		namesList.erase(namesList.size() - 1);
	std::string rplNames = ":server 353 " + client->getNickname() + " = " + channelName + " :" + namesList + "\r\n";
	std:: string rplEndNames = ":server 366 " + client->getNickname() + " " + channelName + " :End of /NAMES list\r\n";
	send(client->getSocketFd(), rplNames.c_str(), rplNames.size(), 0);
	send(client->getSocketFd(), rplEndNames.c_str(), rplEndNames.size(), 0);
}