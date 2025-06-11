#include "../inc/Client.hpp"

Client::Client(): _loggedIn(false)
{

}

Client::Client(int fd, const std::string username, const std::string &ip): _socketFd(fd), _username(username), _loggedIn(false), _ip(ip)
{
}

Client::Client(const Client& obj)
{
	*this = obj;
}

Client::~Client()
{}

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

// getters
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
const std::string& Client::getBuffer() const
{
	return _buffer;
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
std::string Client::receiveMessage()
{
	char buffer[512];
	std::memset(buffer, 0, sizeof(buffer));
	int bytes = recv(_socketFd, buffer, sizeof(buffer) - 1, 0);

	//a voir si on a ds erreurs a gerer ici
	if (bytes <= 0)
		return "";

	buffer[bytes] = '\0';
	std::string msg(buffer);

	_buffer += msg;
	//sert a debug
	return msg;
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
