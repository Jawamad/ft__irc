#include "../inc/Channel.hpp"
#include "../inc/Client.hpp"

Channel::Channel() : _inviteOnly(false) {}
Channel::Channel(const std::string &name) : _name(name), _inviteOnly(false), _topicOperatorOnly(false), _accessPwdOnly(false), _hasUserLimit(false)
{
	time_t now = std::time(NULL);
	_creationTime = now;
}
Channel::Channel(const Channel& obj)
{
	*this = obj;
}
Channel::~Channel() {}

Channel& Channel::operator=(const Channel& obj)
{
	if (this != &obj)
	{
		_name = obj._name;
		_clients = obj._clients;
		_inviteOnly = obj._inviteOnly; 
		_topicOperatorOnly = obj._topicOperatorOnly;
	}
	return *this;
}

const std::string&	Channel::getName() const
{
	return _name;
}

const std::string&		Channel::getChanPassword() 
{
	return _chanPassword;
}

const std::string&	Channel::getTopic() const
{
	return _topic;
}

const std::map<int, Client*>&	Channel::getClients()const
{
	return _clients;
}

int Channel::getUserLimit() const
{
	return _userLimit;
}

time_t Channel::getCreationTime() const
{
	return _creationTime;
}

void Channel::setUserLimit(int limit) {
	_userLimit = limit;
 }

void  Channel::setTopic(const std::string &topic)
{
	this->_topic = topic;
}

void  Channel::setChanPassword(const std::string &chanPassword)
{
	this->_chanPassword = chanPassword;
}

bool Channel::isInviteOnly() const {
	return _inviteOnly;
}

void Channel::setInviteOnly(bool status) {
	_inviteOnly = status;
}

bool Channel::topicIsOperatorModOnly()  const {
	return _topicOperatorOnly ;
}

void Channel::setTopicStatus(bool status) {
	_topicOperatorOnly = status;
}

bool Channel::isPasswordOnly()  const {
	return _accessPwdOnly;
}

void Channel::setPasswordStatus(bool status) {
	_accessPwdOnly = status;
}

bool Channel::isLimitedNbUser() const {
	return _hasUserLimit;
}

void Channel::setLimitedNbUser(bool status) {
	_hasUserLimit = status;
}

void	Channel::addClient(Client* client)
{
	if (client)
		_clients[client->getSocketFd()] = client;
}

void	Channel::removeClient(int clientFd)
{
	_clients.erase(clientFd);
	_operators.erase(clientFd);
}

bool	Channel::hasClient(int clientFd) const
{
	return _clients.find(clientFd) != _clients.end();
}
void	Channel::broadcast( const std::string &message, int senderFd)
{
	for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->first != senderFd)
		{
			send(it->first, message.c_str(), message.length(), 0);
			std::cout << "Message send to " << it->second->getNickname() << " by broadcast! Message:" << message;
		}
	}
}

int	Channel::getClientCount() const
{
	return _clients.size();
}

bool Channel::isOperator(int clientFd) const { 
	for(std::set<int>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (clientFd == *it)
			return true;
	}
	return false;
}

void Channel::addOperator(int clientFd) {
	if (hasClient(clientFd))
	{
		_operators.insert(clientFd);
	}
}

void Channel::removeOperator(int clientFd) {
	_operators.erase(clientFd);
}


void Channel::clientGetsKickByOperator(const std::string &nickName, Client &client) 
{
	(void) nickName;
	removeClient(client.getSocketFd());
}


void Channel::clientGetsInviteByOperator(const std::string &nickName, Client &client) 
{
	(void) nickName;
	_inviteList.insert(client.getSocketFd());
	client.addInvitedList(this->_name);
}

Client* Channel::searchMember(const std::string& nickname) const
{
	std::map<int, Client*>::const_iterator it;
	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
		{
			return it->second;
		}
	}
	return NULL;
}

bool Channel::isInvite(Client *client)
{
	if (_inviteList.find(client->getSocketFd()) == _inviteList.end())
		return false;
	return true;
}

void	Channel::delInviteList(Client *client)
{
	std::set<int>::iterator it = this->_inviteList.find(client->getSocketFd());
	if (it == this->_inviteList.end())
		return;
	this->_inviteList.erase(client->getSocketFd());
}