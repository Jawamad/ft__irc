#include "../inc/Client.hpp"
#include "../inc/Server.hpp"

Client::Client(): _loggedIn(false), _hasPassedPassword(false), _hasUser(false), _hasNick(false)
{

}

Client::Client(int fd, const std::string &hostname, const std::string &ip)
	: _socketFd(fd), _nickname(""), _username(""), _loggedIn(false), _hasPassedPassword(false), _hasUser(false),
		_hasNick(false),_hostname(hostname), _ip(ip), _buffer("")
{
}

Client::Client(const Client& obj)
{
	*this = obj;
}

Client::~Client() {}

Client& Client::operator=(const Client& obj)
{
	if (this != &obj)
	{
		_socketFd = obj.getSocketFd();
		_nickname = obj.getNickname();
		_username = obj.getUsername();
		_ip = obj.getIp();
		_buffer = obj.getBuffer();
		_loggedIn = obj.isLoggedIn();
	}
	return *this;
}

int	Client::getSocketFd() const
{
	return _socketFd;
}
const std::string& Client::getIp() const
{
	return _ip;
}
const std::string& Client::getNickname() const
{
	return _nickname;
}
const std::string& Client::getUsername() const
{
	return _username;
}
std::string& Client::getBuffer()
{
	return _buffer;
}
const std::string& Client::getBuffer() const
{
	return _buffer;
}
const std::string& Client::getHostname() const
{
	return _hostname;
}
bool Client::isLoggedIn() const
{
	return _loggedIn;
}

// Setters
void Client::setNickname(const std::string &nickname)
{
	_nickname = nickname;
}
void Client::setUsername(const std::string &username)
{
	_username = username;
}
void Client::setLoggedIn(bool loggedIn)
{
	_loggedIn = loggedIn;
}



void Client::appendToBuffer(const std::string &data)
{
	_buffer += data;
}
void Client::clearBuffer()
{
	_buffer.clear();
}

// Network
bool Client::receiveMessage()
{
	char buffer[512];
	std::memset(buffer, 0, sizeof(buffer));
	int bytes = recv(_socketFd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0)
		return false;

	buffer[bytes] = '\0';

	_buffer += buffer;
	return true;
}

bool	Client::hasPassedPassword() const
{
	return _hasPassedPassword;
}

void	Client::setHaspassedPassword(bool value)
{
	_hasPassedPassword = value;
}

void	Client::setHasUser(bool value)
{
	_hasUser = value;
}

bool	Client::hasUser() const
{
	return _hasUser;
}

void	Client::setHasNick(bool value)
{
	_hasNick = value;
}

bool	Client::hasNick() const
{
	return _hasNick;
}

void	Client::logRoutine(Server& server)
{
	(void) server;
	if (hasUser() && hasNick() && hasPassedPassword())
	{
		setLoggedIn(true);
		server.serverMessage(this, "001", "Welcome to the Internet Relay Network PIRC " + _nickname + "!" + _username + "@" + _ip);
		server.serverMessage(this, "002", "Your host is "+ server.getHost() +", running version PIRC_1.0");
		server.serverMessage(this, "003", server.getCreationTime());
		server.serverMessage(this, "004", "PIRC PIRC_1.0 mtikl");
		server.serverMessage(this, "005", "PREFIX=(o)@ CHANTYPES=# MODES=5 NICKLEN=30 :are supported by this server");
		server.serverMessage(this, "422", "MOTD File is missing");
	}
}

void	Client::addInvitedList(std::string chan)
{
	this->_invitedChannel.insert(chan);
}

void	Client::delInvitedList(std::string chan)
{
	std::set<std::string>::iterator it = this->_invitedChannel.find(chan);
	if (it == this->_invitedChannel.end())
		return;
	this->_invitedChannel.erase(chan);
}