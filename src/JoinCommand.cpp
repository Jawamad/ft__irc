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
		// Vérifier si le mode "i" ou "k" est actif
		if (channels[channelName]->isInviteOnly() && channels[channelName]->isPasswordOnly()) {
			std::cout << "Le canal est sur invitation et nécessite un mot de passe." << std::endl;
			// Si le canal est en mode 'i' et 'k', on refuse l'accès.
		} 
		else if (channels[channelName]->isInviteOnly()) {
			std::cout << "Ce canal est uniquement sur invitation." << std::endl;
			// Si seul le mode 'i' est activé, on bloque l'accès.
		} 
		else if (channels[channelName]->isPasswordOnly()) {
			if (channels[channelName]->getChanPassword() == channelPassword) {
				channels[channelName]->addClient(client);
			} else {
				std::cout << "Vous ne pouvez pas rejoindre ce canal à cause du mot de passe." << std::endl;
			}
		} 
		else {
			// Le canal est accessible, aucun des modes 'i' ou 'k' n'est activé.
			channels[channelName]->addClient(client);
		}

		// if (!channels[channelName]->isInviteOnly() && !channels[channelName]->isPasswordOnly())
		// 	channels[channelName]->addClient(client);
		// else if (channels[channelName]->isInviteOnly())
		// {
		// 	std::cout << "join this channel is only on invitation " << std::endl;
		// }
		// // MODE k+ password required
		// else if (channels[channelName]->isPasswordOnly())
		// {
		// 	if (channels[channelName]->getChanPassword() == channelPassword)
		// 	{
		// 		channels[channelName]->addClient(client);
		// 	}
		// 	else 
		// 	{
		// 		std::cout << "you cannot join this channel due to password " << std::endl;
		// 	}
		// } 
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