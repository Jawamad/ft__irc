#include "../inc/WhoCommand.hpp"
#include "../inc/Server.hpp"

void WhoCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string target;
	args >> target;

	if (target.empty())
	{
		for(std::map<int, Client*>::iterator it = server.getClients().begin(); it != server.getClients().end(); ++it)
		{
			Client *other = it->second;
			sendWhoReply(client, "*", other);
		}
		sendEndOfWho(client, "*");
	}
	else
	{
		if (server.hasChannel(target))
		{
			Channel *chan = server.getChan(target);
			const std::map<int, Client*> members = chan->getClients();
			for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
			{
				sendWhoReply(client, target, it->second);
			}
			sendEndOfWho(client, target);
		}
		else
		{
			Client *other = server.getClientByNick(target);
			if (other)
			{
				sendWhoReply(client, "*", other);
			}
			sendEndOfWho(client, target);
		}
	}
}

void WhoCommand::sendWhoReply( Client *requester, const std::string &channel, Client *target)
{
	std::string msg;
	msg = ":PIRC 352 " + requester->getNickname() + " " + (channel.empty() ? "*" : channel) + " "
		+ target->getUsername() + " " + target->getIp() + " PIRC " + target->getNickname() + " "
		+ "H :0 " + target->getUsername() + "\r\n";
	send(requester->getSocketFd(), msg.c_str(), msg.size(), 0);
}

void WhoCommand::sendEndOfWho( Client *requester, const std::string &channel)
{
	std::string msg;
	msg = ":PIRC 315 " + requester->getNickname() + " " + (channel.empty() ? "*" : channel) + " End of WHO list\r\n"; 
	send(requester->getSocketFd(), msg.c_str(), msg.size(), 0);
}