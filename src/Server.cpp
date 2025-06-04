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

//server setup
bool	Server::start()
{
	std::cout << "Server is starting..." << std::endl;
	setupSocket();
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
	std::cout << _port << std::endl;
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
void Server::parseCommand(Client &client, const std::string &msg)
{
	std::istringstream	iss(msg);
	std::string command;
	iss >> command;
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
		std::string ok = ":server 001 " + nickname + " :Welcome to the IRC server\r\n";
		send(client.getSocketFd(), ok.c_str(), ok.size(), 0);
	}
}