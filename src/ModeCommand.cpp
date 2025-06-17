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
	std::string channelName;
	std::string modeletter;
	args >> channelName >> modeletter;

	if(channelName.empty())
	{
		std::string err = "461 MODE :Not enough parameters\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}

	if (channelName[0] != '#')
		channelName = '#' + channelName;

	Channel* channel = server.getChan(channelName);

	Client* clientToKickPtr = server.findClientByNickname(clientToKick);

	if (modeletter == "+i")
	{
		std::cout << "From now on, the channel access is only on invitation" << std::endl;
	}
	else if (modeletter == "-i")
	{
		std::cout << "From now on, the channel access is open even without invitation" << std::endl;
	}
	
}