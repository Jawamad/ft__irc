#include "../inc/ModeCommand.hpp"
#include "../inc/Server.hpp"
#include <cstdlib> 


// MODES
// — i : Définir/supprimer le canal sur invitation uniquement
// — t : Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux
// — k : Définir/supprimer la clé du canal (mot de passe)
// — o : Donner/retirer le privilège de l’opérateur de canal
// — l : Définir/supprimer la limite d’utilisateurs pour le canal



void ModeCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	(void)server; 
	std::string channelName;
	std::string modeletter;
	std::string modeValue;
	args >> channelName >> modeletter >> modeValue;

	if(channelName.empty())
	{
		std::string err = "461 MODE :Not enough parameters\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName);

	if (modeletter == "+i")
	{
		channel->setInviteOnly(true);
		std::cout << channel->isInviteOnly() << std::endl;
		std::cout << "From now on," << channelName << " channel access is only on invitation" << std::endl;
	}
	else if (modeletter == "-i")
	{
		std::cout << channel->isInviteOnly() << std::endl;
		channel->setInviteOnly(false);
		std::cout << "From now on," << channelName << " channel access is open even without invitation" << std::endl;
	}
	else if (modeletter == "+t")
	{
		channel->setTopicStatus(true);
		std::cout << channel->topicIsOperatorModOnly() << std::endl;
		std::cout << "From now on," << channelName << " channel's topic can only be modify by operator" << std::endl;
	}
	else if (modeletter == "-t")
	{
		channel->setTopicStatus(false);
		std::cout << channel->topicIsOperatorModOnly() << std::endl;
		std::cout << "From now on," << channelName << " channel's topic can be modify by everyone" << std::endl;

	}
	else if (modeletter == "+k")
	{
		if (channel->isOperator(client->getSocketFd()))
		{
			channel->setPasswordStatus(true);
			channel->setChanPassword(modeValue);
			std::cout << channel->isPasswordOnly() << std::endl;
			std::cout << modeValue << std::endl;
			std::cout << "From now on," << channelName << " channel access need password" << std::endl;
		}
		else
		{
			std::cout << "You do not have the rights to set a password for this channel (not operator)" << std::endl;
		}
	}
	else if (modeletter == "-k")
	{
		if (channel->isOperator(client->getSocketFd()))
		{
			channel->setPasswordStatus(false);
			std::cout << channel->isPasswordOnly() << std::endl;
			std::cout << "From now on," << channelName << " channel access not need password" << std::endl;
		}
		else
		{
			std::cout << "You do not have the rights to set a password for this channel (not operator)" << std::endl;
		}
	}
	else if (modeletter == "+l")
	{
		if (channel->isOperator(client->getSocketFd())) 
		{
			int userLimit = std::atoi(modeValue.c_str());
			
			if (userLimit <= 0) {
				std::cerr << "Invalid user limit: must be > 0\n";
				return;
			}
			channel->setLimitedNbUser(true);
			channel->setUserLimit(userLimit);
			std::cout << "User limit set to: " << userLimit << std::endl;
			std::cout << "From now on, " << channelName << " channel is limited to " << userLimit << " people" << std::endl;
		}
		else
		{
			std::cout << "You do not have the rights to set the user limit for this channel (not operator)" << std::endl;
		}
	} 
	else if (modeletter == "-l")
	{
		if (channel->isOperator(client->getSocketFd()))
		{
			channel->setLimitedNbUser(false);
			std::cout << "From now on," << channelName << " has no limited number of users" << std::endl;
		}
		else
		{
			std::cout << "You do not have the rights to set the user limit for this channel (not operator)" << std::endl;
		}

	}
	else if (modeletter == "+o" || modeletter == "-o")
	{
		if (!channel->isOperator(client->getSocketFd())) {
			std::cout << "You must be an operator to modify operator status.\n";
			return;
		}

		Client* target = server.findClientByNickname(modeValue);
		if (!target || !channel->hasClient(target->getSocketFd())) {
			std::cout << "Target user not in the channel.\n";
			return;
		}

		if (modeletter == "+o") {
			std::cout << modeValue << "est maintenant operateur ! \n";
			channel->addOperator(target->getSocketFd());

		} else {
			std::cout << modeValue << "n'est plus operateur ! \n";
			channel->removeOperator(target->getSocketFd());
		}

		std::string msg = ":" + client->getNickname() + " MODE " + channelName + " " + modeletter + " " + modeValue + "\r\n";
		channel->broadcast(msg, -1);
	}
}