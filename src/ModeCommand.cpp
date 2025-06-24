#include "../inc/ModeCommand.hpp"
#include "../inc/Server.hpp"

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
		channel->setPasswordStatus(true);
		std::cout << channel->isPasswordOnly() << std::endl;
		std::cout << "From now on," << channelName << " channel access need password" << std::endl;
	}
	else if (modeletter == "-k")
	{
		channel->setPasswordStatus(false);
		std::cout << channel->isInviteOnly() << std::endl;
		std::cout << "From now on," << channelName << " channel access not need password" << std::endl;

	}
}