#include "../inc/Channel.hpp"

Channel::Channel() : _inviteOnly(false) {}
Channel::Channel(const std::string &name) : _name(name), _inviteOnly(false), _topicOperatorOnly(false), _accessPwdOnly(false), _hasUserLimit(false) {}
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
		// _clients = obj._clients;
		// _operators = obj._operators;
		_inviteOnly = obj._inviteOnly; 
		_topicOperatorOnly = obj._topicOperatorOnly;
	}
	return *this;
}

/// GETTERS ///////// 
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

int Channel::getUserLimit() const {
    return _userLimit;
}

/// SETTERS  ///////////

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

///////////////////////////////////////


bool Channel::isInviteOnly() const {
	return _inviteOnly;
}

void Channel::setInviteOnly(bool status) {
	_inviteOnly = status;
}

///////////////////////////////////////

bool Channel::topicIsOperatorModOnly()  const {
	return _topicOperatorOnly ;
}

void Channel::setTopicStatus(bool status) {
	_topicOperatorOnly = status;
}

////////////////////////////////////////

bool Channel::isPasswordOnly()  const {
	return _accessPwdOnly;
}

void Channel::setPasswordStatus(bool status) {
	_accessPwdOnly = status;
}

///////////////////////////////////////

bool Channel::isLimitedNbUser() const {
    return _hasUserLimit;
}

///////////////////////////////////////

void Channel::setLimitedNbUser(bool status) {
    _hasUserLimit = status;
}

///////////////////////////////////////


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
			send(it->first, message.c_str(), message.length(), 0);
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
		// int last = *(_operators.rbegin());
	    // std::cout << "Last operator: " << last << std::endl;
	}
}

void Channel::removeOperator(int clientFd) {
	_operators.erase(clientFd);
}


void Channel::clientGetsKickByOperator(const std::string &nickName, Client &client) 
{
	std::cout << "operator " << nickName << " has kick the user " << client.getNickname() << std::endl;
	removeClient(client.getSocketFd());
}


void Channel::clientGetsInviteByOperator(const std::string &nickName, Client &client) 
{
	std::cout << "operator " << nickName << " has invite the user " << client.getNickname() << std::endl;
	addClient(&client);
}

