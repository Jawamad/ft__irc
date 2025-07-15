#include "../inc/Server.hpp"
#include "../inc/interface.hpp"
#include <sstream>

volatile sig_atomic_t g_shouldExit = 0;

void handleSignal(int signum)
{
	if (signum == SIGINT)
	{
		std::cout << "\n[INFO] Caught SIGINT (Ctrl+C). Shutting down..." << std::endl;
		g_shouldExit = 1;
	}
}

Server::Server(int port, const std::string &password): _serverFd(-1), _port(port), _password(password), _host("localhost"), _name("PIRC"), _maxFd(0)
{
	_commands["KICK"] = new KickCommand();
	_commands["INVITE"] = new InviteCommand();
	_commands["TOPIC"] = new TopicCommand();
	_commands["MODE"] = new ModeCommand();
	_commands["NICK"] = new NickCommand();
	_commands["JOIN"] = new JoinCommand();
	_commands["PART"] = new PartCommand();
	_commands["PASS"] = new PassCommand();
	_commands["PRIVMSG"] = new PrivmsgCommand();
	_commands["QUIT"] = new QuitCommand();
	_commands["USER"] = new UserCommand();
	_commands["PING"] = new PingCommand();
	_commands["CAP"] = new CapCommand();
	_commands["WHO"] = new WhoCommand();
	_commands["LIST"] = new ListCommand();
	time_t now = time(0);
	char buf[128];
	strftime(buf, sizeof(buf), "This server was created %a %b %d %Y at %H:%M:%S UTC", gmtime(&now));
	_creationTime = buf;
	setupSocket();
}

Server::~Server()
{
	for (std::map<std::string, ICommand*>::iterator it = _commands.begin(); it != _commands.end(); ++it)
		delete it->second;
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
	if (_serverFd >= 0)
		close(_serverFd);
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		delete it->second;
		close(it->first);
	}
	_clients.clear();
	_channels.clear();
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

int	Server::getServerFd() const
{
	return _serverFd;
}
const std::string& Server::getPassword() const
{
	return _password;
}
const std::string& Server::getName() const
{
	return _name;
}
std::map<int, Client*>& Server::getClients()
{
	return _clients;
}
const std::map<int, Client*>& Server::getClients() const
{
	return _clients;
}
int		Server::getPort()
{
	return _port;
}
std::map<std::string, Channel*>& Server::getChannels()
{
	return _channels;
}
const	std::map<std::string, Channel*>& Server::getChannels() const
{
	return _channels;
}
const	std::map<std::string, ICommand*>& Server::getCommands() const
{
	return _commands;
}
Client*	Server::getClientByNick(std::string nickname)
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}
void	Server::setPassword(const std::string &pass)
{
	_password = pass;
}
void	Server::addChannel(std::string chanName)
{
	_channels[chanName] = new Channel(chanName);
}
void	Server::delChannel(std::string chanName)
{
	delete (_channels[chanName]);
	_channels.erase(chanName);
}
void	Server::delClient(int fd)
{
	if (_clients.find(fd) != _clients.end())
	{
		for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
			it->second->removeClient(fd);
		delete _clients[fd];
		_clients.erase(fd);
	}
	FD_CLR(fd, &_readFds);
	close(fd);
}
Channel* Server::getChan(std::string chanName)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(chanName);
	if (it == _channels.end())
		return NULL;
	return it->second;
}

bool	Server::hasClient(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end())
		return true;
	return false;
}

//server setup
bool	Server::start()
{
	std::cout << "Server is running !" << std::endl;
	run();
	std::cout << "Server is closed !" << std::endl;
	return true;
}

void	Server::run()
{
	std::signal(SIGINT, handleSignal);
	while (!g_shouldExit)
	{
		updateFdSet();
		int activity = select(_maxFd + 1, &_readFds, NULL, NULL, NULL);
		if (activity < 0)
		{
			if (!g_shouldExit)
				std::cerr << ":" << this->getName() << " :Server error: select" << std::endl;
			continue;
		}

		if (FD_ISSET(_serverFd, &_readFds))
			acceptNewClient();
		
		std::map<int, Client*>::iterator it = _clients.begin();
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
		std::cerr << ":" << this->getName() << " :Server Error: accept." << std::endl;
		return;
	}

	std::string ip = inet_ntoa(clientAddr.sin_addr);

	_clients[clientFd] = new Client(clientFd, "guest", ip);
	std::cout << "New client connected : " << ip << " (fd=" << clientFd << ")" << std::endl;
}
void	Server::handleClientMessage(int clientFd)
{
	Client* client = _clients[clientFd];
	processClientData(client);
}
void	Server::removeClient(int clientFd)
{
	std::map<int, Client*>::iterator it = _clients.find(clientFd);
	delete it->second;
	close(clientFd);
	_clients.erase(clientFd);
}
void	Server::setupSocket()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
	{
		std::cerr << ":" << this->getName() << " :Server Error: init socketfd" << std::endl;
		g_shouldExit = 1;
		return;
	}

	// a revoir
	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << ":" << this->getName() << " :Server Error: setsockopt" << std::endl;
		g_shouldExit = 1;
		return ;
	}

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);

	if ( bind(_serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		std::cerr << ":" << this->getName() << " :Server Error: bind" << std::endl;
		g_shouldExit = 1;
		return ;
	}

	if (listen(_serverFd, 5) < 0)
	{
		std::cerr << ":" << this->getName() << " :Server Error: listen" << std::endl;
		g_shouldExit = 1;
		return ;
	}

	std::cout << "Server run on port " << _port << std::endl;
}
void	Server::updateFdSet()
{
	FD_ZERO(&_readFds);
	FD_SET(_serverFd, &_readFds);
	_maxFd = _serverFd;

	for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		int fd = it->first;
		FD_SET(fd, &_readFds);
		if (fd > _maxFd)
			_maxFd = fd;
	}
}

void	Server::processClientData(Client* client)
{
	bool ok = client->receiveMessage();

	if (!ok)
	{
		std::cout << "Connection lost with client (fd=" << client->getSocketFd() << ")" << std::endl;
		removeClient(client->getSocketFd());
		return;
	}
	size_t pos;
	int fd = client->getSocketFd();
	while (hasClient(fd) && ((pos = client->getBuffer().find('\n')) != std::string::npos))
	{
		std::string line = client->getBuffer().substr(0, pos);
		client->getBuffer().erase(0, pos + 1);

		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		if (!line.empty())
		{
			std::cout << "Message receive from " << client->getIp() << "=" << client->getUsername() << "@" << client->getNickname() << ": " << line << std::endl;
			parseCommand(client, line);
		}
	}
}

bool	Server::hasChannel(std::string chanName)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(chanName);
	if (it == _channels.end())
		return false;
	return true;
}


Client* Server::findClientByNickname(const std::string& nickname) 
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		Client* client = it->second;
		std::cout << client->getNickname();
		if (client->getNickname() == nickname) {
			return client;
		}
	}
	return NULL;
}

Client* Server::findClientByFd(int clientFd)
{
	  for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		Client* client = it->second;
		std::cout << client->getSocketFd();
		if (client->getSocketFd() == clientFd) {
			return client;
		}
	}
	return NULL;
}

void Server::parseCommand(Client* client, const std::string &msg)
{
	std::istringstream	iss(msg);
	std::string command;
	iss >> command;

	if (!client->isLoggedIn() && command != "NICK" && command != "USER" && command != "PASS" && command != "QUIT" && command != "CAP")
	{
		serverMessage(client, "451", "Server Error :You have not registered");
		return ;
	}

	std::map<std::string, ICommand*>::iterator it = _commands.find(command);
	if (it != _commands.end())
		it->second->execute(*this, client, iss);
	else
	{
		std::string err = ":" + this->getName() + " 421 " + client->getNickname() + " " + command + " :Server Error :Unknown command\r\n";
		send(client->getSocketFd(), err.c_str(), err.size(), 0);
	}
}

void Server::serverMessage(Client* client, std::string errorCode,  const std::string& errorMsg)
{
	std::string err = ":PIRC " + errorCode + " " + client->getNickname() + " :" + errorMsg + "\r\n";
	send(client->getSocketFd(), err.c_str(), err.size(), 0);
}
// ;essqge d erreur aui fonctionne
void Server::errorMessage(Client* client, const std::string& code, const std::string& command, const std::string& message)
{
	std::string err = ":PIRC " + code + " " + client->getNickname() + " " + command + " :" + message + "\r\n";
	send(client->getSocketFd(), err.c_str(), err.size(), 0);
}


void Server::sendCommandMessage(Client* sender, const std::string& command, const std::string& params, const std::string& trailing)
{
	std::string msg = ":" 
		+ sender->getNickname() + "!"
		+ sender->getUsername() + "@"
		+ sender->getIp()
		+ " " + command + " " + params;
	
	if (!trailing.empty())
		msg += " :" + trailing;

	msg += "\r\n";

	send(sender->getSocketFd(), msg.c_str(), msg.size(), 0);
}

void Server::sendNumericReply(Client* target, int code, const std::string& params, const std::string& trailing)
{
	std::ostringstream oss;
	oss << code;
	std::string msg = ":PIRC" + oss.str() + " " + target->getNickname();

	if (!params.empty())
		msg += " " + params;

	if (!trailing.empty())
		msg += " :" + trailing;

	msg += "\r\n";

	send(target->getSocketFd(), msg.c_str(), msg.size(), 0);
}

const	std::string& Server::getHost() const
{
	return _host;
}

const	std::string& Server::getCreationTime() const
{
	return _creationTime;
}