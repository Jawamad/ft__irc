#include "../inc/PrivmsgCommand.hpp"
#include "../inc/Server.hpp"
#include <iomanip>

#include <iomanip> // pour setw et setfill

void PrivmsgCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string target;
	std::string message;

	args >> target;
	std::getline(args, message);

	if (target.empty() || message.empty())
	{
		std::string err = "461 PRIVMSG :Not enough parameters\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
		return;
	}

	bool isDcc = (
		message.size() >= 2 &&
		message[0] == '\x01' &&
		message[message.size() - 1] == '\x01'
	);

	if (!isDcc)
	{
		while (!message.empty() && (message[0] == ' ' || message[0] == ':'))
			message = message.substr(1);
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

		std::string fullMessage = ":" + client->getNickname() + "!" +
			client->getUsername() + "@" + client->getIp() +
			" PRIVMSG " + target + " :" + message + "\r\n";

		channel->broadcast(fullMessage, client->getSocketFd());
	}
	else
	{
		bool found = false;
		std::map<int, Client*> clients = server.getClients();

		for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->second->getNickname() == target)
			{
				found = true;

				std::string fullMsg = ":" + client->getNickname() + "!" +
					client->getUsername() + "@" + client->getIp() +
					" PRIVMSG " + target + " :" + message + "\r\n";

				send(it->first, fullMsg.c_str(), fullMsg.size(), 0);
				break;
			}
		}
		if (!found)
		{
			std::string err = "401 " + target + " :No such nick\r\n";
			send(client->getSocketFd(), err.c_str(), err.size(), 0);
		}
	}
}



/* void PrivmsgCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string target;
	std::string message;

	args >> target;
	std::getline(args, message);
	std::cout << "Hexdump du contenu message original : ";
	for (size_t i = 0; i < message.size(); ++i)
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)message[i] << " ";
	std::cout << std::dec << std::endl;

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
		// Message classique vers canal
		if (!message.empty() && (message[0] == ' ' || message[0] == ':'))
			message = message.substr(1);

		std::string fullMessage = ":" + client->getNickname() + "!" +
			client->getUsername() + "@" + client->getIp() +
			" PRIVMSG " + target + " " + message + "\r\n";
		channel->broadcast(fullMessage, client->getSocketFd());
	}
	else
	{
		bool found = false;
		std::map<int, Client*> clients = server.getClients();

		for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->second->getNickname() == target)
			{
				found = true;

				std::string originalMessage = message;
				std::string fullMsg;

				if (originalMessage.size() >= 2 &&
					originalMessage[0] == '\x01' &&
					originalMessage[originalMessage.size() - 1] == '\x01')
				{
					std::cout << "✅ DCC message détecté. Relais brut sans altération.\n";

					fullMsg = ":" + client->getNickname() + "!" +
						client->getUsername() + "@" + client->getIp() +
						" PRIVMSG " + target + " " + originalMessage + "\r\n";
				}
				else
				{
					if (!originalMessage.empty() && (originalMessage[0] == ' ' || originalMessage[0] == ':'))
						originalMessage = originalMessage.substr(1);

					fullMsg = ":" + client->getNickname() + "!" +
						client->getUsername() + "@" + client->getIp() +
						" PRIVMSG " + target + " " + originalMessage + "\r\n";
				}

				// Debug (message envoyé, visible même pour les caractères non imprimables)
				std::cout << "Message complet envoyé à fd=" << it->first << ": ";
				for (size_t i = 0; i < fullMsg.size(); ++i)
				{
					unsigned char c = fullMsg[i];
					if (c == '\x01')
						std::cout << "[CTRL+A]";
					else if (isprint(c))
						std::cout << c;
					else
						std::cout << "[" << std::hex << (int)c << "]";
				}
				std::cout << std::endl;

				// Envoi au destinataire
				send(it->first, fullMsg.c_str(), fullMsg.size(), 0);
				break;
			}
		}
		if (!found)
		{
			std::string err = "401 " + target + " :No such nick\r\n";
			send(client->getSocketFd(), err.c_str(), err.size(), 0);
		}
	}
}
 */

/* void PrivmsgCommand::execute(Server &server, Client *client, std::istringstream &args)
{
	std::string target;
	std::string message;
	args >> target;
	std::getline(args, message);
	if (message.find(std::string("\x01") + "DCC") != std::string::npos)
	{
		// Ne rien modifier, garder le message tel quel
	}
	else if (message.size() > 0 && (message[0] == ' ' || message[0] == ':'))
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

	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->getNickname() == target)
		{
			found = true;

			// Préservation du message pour DCC
			std::string originalMessage = message;

			// Vérification s'il s'agit d'un DCC (encapsulé dans \x01)
			if (originalMessage.size() >= 2 &&
				originalMessage[0] == '\x01' &&
				originalMessage[originalMessage.size() - 1] == '\x01')
			{
				std::cout << "✅ DCC message détecté. Relais brut sans altération.\n";
			}
			else
			{
				// Traitement normal des messages : suppression du préfixe inutile
				if (!originalMessage.empty() && (originalMessage[0] == ' ' || originalMessage[0] == ':'))
					originalMessage = originalMessage.substr(1);
			}

			std::string fullMsg = ":" + client->getNickname() + "!" +
				client->getUsername() + "@" + client->getIp() +
				" PRIVMSG " + target + " :" + originalMessage + "\r\n";

			// Debug byte à byte
			std::cout << "Message complet envoyé à fd=" << it->first << ": ";
			for (size_t i = 0; i < fullMsg.size(); ++i)
			{
				unsigned char c = fullMsg[i];
				if (c == '\x01')
					std::cout << "[CTRL+A]";
				else if (isprint(c))
					std::cout << c;
				else
					std::cout << "[" << std::hex << (int)c << "]";
			}
			std::cout << std::endl;

			// Envoi du message au destinataire
			send(it->first, fullMsg.c_str(), fullMsg.size(), 0);
			break;
		}
	}

	if (!found)
	{
		std::string err = "401 " + target + " :No such nick\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
	}
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
				std::cout << fullMsg << std::endl;
				send(it->first, fullMsg.c_str(), fullMsg.size(), 0);
				break;
			}
		}
		if(!found)
		{
			std::string err = "401 " + target + " :No such nick\r\n";
			send(client->getSocketFd(), err.c_str(), err.size(), 0);
		}
	}
} */