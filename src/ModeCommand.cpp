#include "../inc/ModeCommand.hpp"
#include "../inc/Server.hpp"
#include <cstdlib> 


// MODES
// — i : Définir/supprimer le canal sur invitation uniquement
// — t : Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux
// — k : Définir/supprimer la clé du canal (mot de passe)
// — o : Donner/retirer le privilège de l’opérateur de canal
// — l : Définir/supprimer la limite d’utilisateurs pour le canal

// RPL_CHANNELMODEIS (324) n’est pas un message d’erreur, c’est un message de réponse positive envoyé au client pour lui indiquer quels sont les modes actuellement appliqués sur un canal.

// Ce que ça signifie :
// Quand un client envoie la commande MODE #channel, le serveur répond avec le code 324 pour lui dire quels modes sont en place sur ce canal.

// Exemple de message envoyé au client :

// php-template
// Copy
// Edit
// :<serveur> 324 <client> <channel> +itk secret 10
// si c'est l'oprateur on renvoie le secret 
// Ici,

// +itk : modes activés (i = invite only, t = only ops can change topic, k = channel key set),

// secret : argument de mode k (la clé),

// 10 : argument de mode l (limite du nombre d’utilisateurs).

void ModeCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string channelName;
	std::string modeletter;
	std::string modeValue;
	args >> channelName >> modeletter >> modeValue;
	
	if (channelName[0] != '#')
		channelName = '#' + channelName;
	if (!server.hasChannel(channelName))
		server.errorMessage(client, 403, "MODE :No such channel");

	if(channelName.empty())
		server.errorMessage(client, 461, "MODE :Not enough parameters");

	Channel* channel = server.getChan(channelName);

	if (modeletter == "+i")
	{
		channel->setInviteOnly(true);
		std::cout << channel->isInviteOnly() << std::endl;
		////////////////////////// notice 
		// std::cout << "From now on," << channelName << " channel access is only on invitation" << std::endl;
	}
	else if (modeletter == "-i")
	{
		std::cout << channel->isInviteOnly() << std::endl;
		channel->setInviteOnly(false);
		////////////////////////// notice
		// std::cout << "From now on," << channelName << " channel access is open even without invitation" << std::endl;
	}
	else if (modeletter == "+t")
	{
		channel->setTopicStatus(true);
		std::cout << channel->topicIsOperatorModOnly() << std::endl;
		// std::cout << "From now on," << channelName << " channel's topic can only be modify by operator" << std::endl;
	}
	else if (modeletter == "-t")
	{
		channel->setTopicStatus(false);
		std::cout << channel->topicIsOperatorModOnly() << std::endl;
		// std::cout << "From now on," << channelName << " channel's topic can be modify by everyone" << std::endl;
	}
	else if (modeletter == "+k")
	{
		if (channel->isOperator(client->getSocketFd()))
		{
			channel->setPasswordStatus(true);
			channel->setChanPassword(modeValue);
			std::cout << channel->isPasswordOnly() << std::endl;
			std::cout << modeValue << std::endl;
			// std::cout << "From now on," << channelName << " channel access need password" << std::endl;
		}
		else
		{
			server.errorMessage(client, 482, "INVITE :You're not channel operator");
		}
	}
	else if (modeletter == "-k")
	{
		if (channel->isOperator(client->getSocketFd()))
		{
			channel->setPasswordStatus(false);
			std::cout << channel->isPasswordOnly() << std::endl;
			////////////////////////// notice 
			// std::cout << "From now on," << channelName << " channel access not need password" << std::endl;
		}
		else
		{
			server.errorMessage(client, 482, "INVITE :You're not channel operator");
		}
	}
	else if (modeletter == "+l")
	{
		if (channel->isOperator(client->getSocketFd())) 
		{
			int userLimit = std::atoi(modeValue.c_str());
			
			if (userLimit <= 0)
				server.errorMessage(client, 471, "LIMIT :Cannot join channel (+l)");
			channel->setLimitedNbUser(true);
			channel->setUserLimit(userLimit);
			////////////////////////// notice
			// std::cout << "User limit set to: " << userLimit << std::endl;
			// std::cout << "From now on, " << channelName << " channel is limited to " << userLimit << " people" << std::endl;
		}
		else
		{
			server.errorMessage(client, 482, "LIMIT :You're not channel operator");
		}
	} 
	else if (modeletter == "-l")
	{
		if (channel->isOperator(client->getSocketFd()))
		{
			channel->setLimitedNbUser(false);
			// std::cout << "From now on," << channelName << " has no limited number of users" << std::endl;
		}
		else
		{
			server.errorMessage(client, 482, "MODE :You're not channel operator");
		}

	}
	else if (modeletter == "+o" || modeletter == "-o")
	{
		if (!channel->isOperator(client->getSocketFd()))
		{
			server.errorMessage(client, 482, "MODE :You're not channel operator");
		}

		Client* target = server.findClientByNickname(modeValue);
		if (!target || !channel->hasClient(target->getSocketFd()))
			server.errorMessage(client, 441, "MODE :They aren't on that channel");

		if (modeletter == "+o") {
			////////////////////////// notice 
			// std::cout << modeValue << "est maintenant operateur ! \n";
			channel->addOperator(target->getSocketFd());

		} else {
			////////////////////////// notice
			// std::cout << modeValue << "n'est plus operateur ! \n";
			channel->removeOperator(target->getSocketFd());
		}
		std::string msg = ":" + client->getNickname() + " MODE " + channelName + " " + modeletter + " " + modeValue + "\r\n";
		channel->broadcast(msg, -1);
	}
}