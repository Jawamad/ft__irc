#include "../inc/PrivmsgCommand.hpp"
#include "../inc/Server.hpp"

void PrivmsgCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string target;
	std::string message;
	args >> target;
	std::getline(args, message);
	if (message.size() > 0 && (message[0] == ' ' || message[0] == ':'))
		message = message.substr(1);
	
	if (target.empty() || message.empty())
	{
		std::string err = "461 PRIVMSG :Not enough parameters\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}

	if (target[0] == '#')
	{
		std::map<std::string, Channel*> channels = server.getChannels();
		std::map<std::string, Channel*>::iterator chanIt = channels.find(target);
		if (chanIt == channels.end())
		{
			std::string err = "403 " + target + " :No such channel\r\n";
			send(client->getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}
		Channel *channel = chanIt->second;
		if (!channel->hasClient(client->getSocketFd()))
		{
			std::string err = "442 " + target + " :You're not on that channel\r\n";
			send(client->getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}
		std::string fullMessage = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " PRIVMSG " + target + " " + message + "\r\n";
		channel->broadcast(fullMessage, client->getSocketFd());
	}
	else
	{
		bool found = false;
		std::map<int, Client*> clients = server.getClients();
		for(std::map<int, Client*>::iterator it = clients.begin();it != clients.end(); ++it)
		{
			if (it->second->getNickname() == target)
			{
				found = true;
				std::string fullMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getIp() + " PRIVMSG " + target + " " + message + "\r\n";
				send(it->second->getSocketFd(), fullMsg.c_str(), fullMsg.size(), 0);
				break;
			}
		}
		if(!found)
		{
			std::string err = "401 " + target + " :No such nick\r\n";
			send(client->getSocketFd(), err.c_str(), err.size(), 0);
		}
	}
}