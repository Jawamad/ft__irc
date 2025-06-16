#include "../inc/Server.hpp"

Server::Server(int port, const std::string &password): _serverFd(-1), _port(port), _password(password), _host("localhost"), _maxFd(0)
{
	setupSocket();
}
Server::~Server()
{
	if (_serverFd >= 0)
		close(_serverFd);
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		close(it->first);
}
Server::Server(const Server &obj)
{
	*this = obj;
}
Server& Server::operator=(const Server &obj)
{
	if (this != &obj)
	{
		_serverFd = obj._serverFd;
		_port = obj._port;
		_password = obj._password;
		_host = obj._host;
		_clients = obj._clients;
		_maxFd = obj._maxFd;
	}
	return *this;
}

// Getters
Channel* Server::findChannel(const std::string& name) {
	std::map<std::string, Channel>::iterator it = _channels.find(name);
	if (it != _channels.end()) {
		return &(it->second);
	}
	return NULL;
}


int	Server::getServerFd() const
{
	return _serverFd;
}
const std::string& Server::getPassword() const
{
	return _password;
}
const std::map<int, Client>& Server::getClients() const
{
	return _clients;
}
int		Server::getPort()
{
	return _port;
}
void	Server::setPassword(const std::string &pass)
{
	_password = pass;
}
//server setup
bool	Server::start()
{
	std::cout << "Server is starting..." << std::endl;
	run();
	return true;
}
void	Server::run()
{
	while (true)
	{
		updateFdSet();
		int activity = select(_maxFd + 1, &_readFds, NULL, NULL, NULL);
		if (activity < 0)
		{
			std::cerr << "Server error: select" << std::endl;
			continue;
		}

		if (FD_ISSET(_serverFd, &_readFds))
			acceptNewClient();
		
		//a verifier
		std::map<int, Client>::iterator it = _clients.begin();
		while (it != _clients.end())
		{
			int fd = it->first;
			if (FD_ISSET(fd, &_readFds))
			{
				handleClientMessage(fd);
				it = _clients.upper_bound(fd);
			}
			else
				++it;
		}
	}
}
void	Server::acceptNewClient()
{
	sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	int clientFd = accept(_serverFd, (struct sockaddr *)&clientAddr, &addrLen);

	if (clientFd < 0)
	{
		std::cerr << "Server Error: accept." << std::endl;
		return;
	}

	std::string ip = inet_ntoa(clientAddr.sin_addr);
	std::string defaultUsername = "guest";

	_clients[clientFd] = Client(clientFd, ip, defaultUsername);
	std::cout << "New client connected : " << ip << " (fd=" << clientFd << ")" << std::endl;
}
void	Server::handleClientMessage(int clientFd)
{
	Client &client = _clients[clientFd];
	processClientData(client);
}
void	Server::removeClient(int clientFd)
{
	close( clientFd);
	_clients.erase(clientFd);
}
void	Server::setupSocket()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
	{
		std::cerr << "Server Error: init socketfd" << std::endl;
		exit(1); 
	}

	// a revoir
	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Server Error: setsockopt" << std::endl;
		exit(1);
	}

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);

	if ( bind(_serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		std::cerr << "Server Error: bind" << std::endl;
		perror("bind");
		exit(1);
	}

	if (listen(_serverFd, 5) < 0)
	{
		std::cerr << "Server Error: listen" << std::endl;
		exit(1);
	}

	std::cout << "Server run on port " << _port << std::endl;
}
void	Server::updateFdSet()
{
	FD_ZERO(&_readFds);
	FD_SET(_serverFd, &_readFds);
	_maxFd = _serverFd;

	for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		int fd = it->first;
		FD_SET(fd, &_readFds);
		if (fd > _maxFd)
			_maxFd = fd;
	}
}
void	Server::processClientData(Client &client)
{
	std::string msg = client.receiveMessage();

	if (msg.empty())
	{
		std::cout << "Connection lost with client (fd=" << client.getSocketFd() << ")" << std::endl;
		removeClient(client.getSocketFd());
		return;
	}
	
	std::cout << "Message receive from " << client.getIp() << ": " << msg;
	//parsing command

	std::istringstream ss(msg);
	std::string line;
	while(std::getline(ss, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1, 1);
		if (!line.empty())
			parseCommand(client, line);
	}
}


Client* Server::findClientByNickname(const std::string& nickname) 
{
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        Client& client = it->second;
        if (client.getNickname() == nickname) {
            return &client;  // Retourne un pointeur sur l'objet Client dans la map
        }
    }
    return NULL;
}


// a modifier avec pointeur sur fonction pour chaque commande
void Server::parseCommand(Client &client, const std::string &msg)
{
	std::istringstream	iss(msg);
	std::string command;
	iss >> command;

	if (!_password.empty() && !client.hasPassedPassword() && command != "PASS" && command != "QUIT")
	{
		std::string err = "464 :Password required\r\n";
		send(client.getSocketFd(), err.c_str(), err.size(), 0);
		return ;
	}
	if (!client.isLoggedIn() && command != "NICK" && command != "USER" && command != "PASS" && command != "QUIT")
	{
		std::string err = "451 :You have not registered\r\n";
		send(client.getSocketFd(), err.c_str(), err.size(), 0);
		return ;
	}
	if (command == "NICK")
	{
		std::string nickname;
		iss >> nickname;
		if (nickname.empty())
		{
			std::string err = "431 :No nickname given\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}
		for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			if (it->second.getNickname() == nickname)
			{
				std::string err = "433 * " + nickname + " :Nickname is already in use\r\n";
				send(client.getSocketFd(), err.c_str(), err.size(), 0);
				return;
			}
		}
		client.setNickname(nickname);
		client.setHasNick(true);

		if (client.hasUser() && client.hasNick())
		{
			client.setLoggedIn(true);
			std::string welcome = ":server 001 " + client.getNickname() + " :Welcome to the IRC server \r\n";
			send(client.getSocketFd(), welcome.c_str(), welcome.size(), 0);
		}
	}
	
	else if (command == "PRIVMSG")
	{
		std::string target;
		std::string message;
		iss >> target;
		std::getline (iss, message);
		if (message.size() > 0 && (message[0] == ' ' || message[0] == ':'))
			message = message.substr(1);
		
		if (target.empty() || message.empty())
		{
			std::string err = "461 PRIVMSG :Not enough parameters\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}

		if (target[0] == '#')
		{
			std::map<std::string, Channel>::iterator chanIt = _channels.find(target);
			if (chanIt == _channels.end())
			{
				std::string err = "403 " + target + " :No such channel\r\n";
				send(client.getSocketFd(), err.c_str(), err.size(), 0);
				return;
			}
			Channel& channel = chanIt->second;
			if (!channel.hasClient(client.getSocketFd()))
			{
				std::string err = "442 " + target + " :You're not on that channel\r\n";
				send(client.getSocketFd(), err.c_str(), err.size(), 0);
				return;
			}
			std::string fullMessage = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIp() + " PRIVMSG " + target + " :" + message + "\r\n";
			channel.broadcast(fullMessage, client.getSocketFd());
		}
		else
		{
			bool found = false;
			for(std::map<int, Client>::iterator it = _clients.begin();it != _clients.end(); ++it)
			{
				if (it->second.getNickname() == target)
				{
					found = true;
					std::string fullMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIp() + " PRIVMSG " + target + " :" + message + "\r\n";
					send(it->second.getSocketFd(), fullMsg.c_str(), fullMsg.size(), 0);
					break;
				}
			}
			if(!found)
			{
				std::string err = "401 " + target + " :No such nick\r\n";
				send(client.getSocketFd(), err.c_str(), err.size(), 0);
			}
		}
	}
	else if (command == "PART")
	{
		std::string channelName;
		iss >> channelName;

		if(channelName.empty())
		{
			std::string err = "461 PART :Not enough parameters\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}
		if (_channels.find(channelName) == _channels.end())
		{
			std::string err = "403 " + channelName + " :No such channel\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}

		Channel& channel = _channels[channelName];

		if (!channel.hasClient(client.getSocketFd()))
		{
			std::string err = "442 " + channelName + " :You're not on that channel\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}
		std::string partMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIp() + " PART " + channelName + "\r\n";

		channel.broadcast(partMsg, -1);

		channel.removeClient(client.getSocketFd());
		if (channel.getClients().empty())
			_channels.erase(channelName);
	}
	else if (command == "QUIT")
	{
		std::string quitMessage;
		std::getline(iss, quitMessage);
		if (!quitMessage.empty() && (quitMessage[0] == ' ' || quitMessage[0] == ':'))
			quitMessage = quitMessage.substr(1);
		if (quitMessage.empty())
			quitMessage = "Client quit";
		std::string msg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIp() + " QUIT :" + quitMessage + "\r\n";
		for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			Channel& chan = it->second;
			if (chan.hasClient(client.getSocketFd()))
			{
				chan.broadcast(msg, client.getSocketFd());
				chan.removeClient(client.getSocketFd());

				if (chan.getClients().empty())
					_channels.erase(it);
			}
		}
		close(client.getSocketFd());
		_clients.erase(client.getSocketFd());
	}
	else if (command == "PASS")
	{
		std::string providedPass;
		iss >> providedPass;
		if (client.hasPassedPassword())
		{
			std::string err = "462 :You may not reregister\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}
		if (providedPass.empty())
		{
			std::string err = "461 PASS :Not enough parameters\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}
		if (providedPass != _password)
		{
			std::string err = "464 PASS :Password incorrect\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			close(client.getSocketFd());
			_clients.erase(client.getSocketFd());
			return;
		}
		client.setHaspassedPassword(true);
	}
	else if (command == "USER")
	{
		std::string username, unused1, unused2, realname;
		//check if unused has to be use or it s optional
		iss >> username >> unused1 >> unused2;
		std::getline(iss, realname);
		if (username.empty() || realname.empty())
		{
			std::string err = "461 USER :Not enough parameters\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}

		if (client.hasUser())
		{
			std::string err = "462 :You may not reregister\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}

		client.setUsername(username);
		client.setHasUser(true);

		if (client.hasUser() && client.hasNick())
		{
			client.setLoggedIn(true);
			std::string welcome = ":server 001 " + client.getNickname() + " :welcome to the IRC server \r\n";
			send(client.getSocketFd(), welcome.c_str(), welcome.size(), 0);
		}
	}
	else if (command == "JOIN")
	{
		std::string channelName;
		iss >> channelName;

		if(channelName.empty())
		{
			std::string err = "461 JOIN :Not enough parameters\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}
		if (channelName[0] != '#')
			channelName = '#' + channelName;
		
		if (_channels.find(channelName) == _channels.end())
		{
			_channels[channelName] = Channel(channelName);
			std::cout << "New channel " << channelName << " created by " << client.getNickname() << std::endl;

			_channels[channelName].addClient(&client);                    // On l'ajoute au channel
			_channels[channelName].addOperator(client.getSocketFd());     // Le client devient opérateur	
		}
		
		_channels[channelName].addClient(&client);

		std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIp() + " JOIN " + channelName + "\r\n";

		_channels[channelName].broadcast(joinMsg, -1);

		std::string namesList;
		std::map<int, Client*> members =_channels[channelName].getClients();
		for (std::map<int, Client*>::iterator it = members.begin(); it != members.end(); ++it)
		{
			if (!it->second->getNickname().empty())
			{
				namesList += it->second->getNickname() + " ";
			}
		}
		if (!namesList.empty() && namesList[namesList.size() - 1] == ' ')
			namesList.erase(namesList.size() - 1);
		std::string rplNames = ":server 353 " + client.getNickname() + " = " + channelName + " :" + namesList + "\r\n";
		std:: string rplEndNames = ":server 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list\r\n";
		send(client.getSocketFd(), rplNames.c_str(), rplNames.size(), 0);
		send(client.getSocketFd(), rplEndNames.c_str(), rplEndNames.size(), 0);
	}
	/// operator
	else if (command == "KICK")
	{
		std::string channelName;
		std::string clientToKick;
		iss >> channelName >> clientToKick;

		if(channelName.empty())
		{
			std::string err = "461 KICK :Not enough parameters\r\n";
			send(client.getSocketFd(), err.c_str(), err.size(), 0);
			return;
		}

		// Ajoute le # si absent
		if (channelName[0] != '#')
			channelName = '#' + channelName;

		std::cout << "INPUT " << channelName << " ! " << std::endl;

		Channel* channel = findChannel(channelName); 
		Client* clientToKickPtr = findClientByNickname(clientToKick);
		if (!clientToKickPtr)
		{
			std::cout << "Client " << clientToKick << " not found." << std::endl;
			return;
		}
		
		if (channel)
		{
			std::cout << "FIND " << channelName << " ! " << std::endl;
			std::cout << "operator :::::::::::" << channel->isOperator(client.getSocketFd()) << std::endl;
			
			if (channel->isOperator(client.getSocketFd()))
			{   
				channel->clientGetsKickByOperator(client.getNickname(), *clientToKickPtr);
				std::cout << clientToKickPtr->getNickname() << " is the client to kick" << std::endl;
				std::cout << client.getNickname() << " can KICK " << clientToKick << " ! " << std::endl;
			}
			else
			{
				std::cout << "Permission denied: " << client.getNickname() << " is not operator." << std::endl;
			}
		}
		else
		{
			std::cout << "Channel " << channelName << " not found." << std::endl;
		}
	}
}