#include "../inc/JoinCommand.hpp"
#include "../inc/Server.hpp"

void JoinCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	args >> channelName;

	if(channelName.empty())
	{
		std::string err = "461 JOIN :Not enough parameters\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}
	if (channelName[0] != '#')
		channelName = '#' + channelName;
	
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
		if (!channels[channelName]->isInviteOnly())
			channels[channelName]->addClient(client);
		else if (channels[channelName]->isInviteOnly())
		{
			invitationToAccess(int clientFd, channelName)
		}
	}
	std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " JOIN " + channelName + "\r\n";

	server.getChan(channelName)->broadcast(joinMsg, -1);

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