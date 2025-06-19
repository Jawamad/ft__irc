#include "../inc/Channel.hpp"
Channel::Channel() {}
Channel::Channel(const std::string &name): _name(name)
{}
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
	}
	return *this;
}

const std::string&	Channel::getName() const
{
	return _name;
}

const std::string&	Channel::getTopic() const
{
	return _topic;
}

void  Channel::setTopic(const std::string &topic)
{
	this->_topic = topic;
}

const std::map<int, Client*>&	Channel::getClients()const
{
	return _clients;
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
			send(it->first, message.c_str(), message.length(), 0);
	}
}
size_t	Channel::getClientCount() const
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
		int last = *(_operators.rbegin()); // reverse begin -> dernier élément
	    std::cout << "Last operator: " << last << std::endl;
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

bool Channel::invitationToAccess(int clientFd) {
	if (channel.isOperator(clientFd)) {
		channels[channelName]->addClient(client);
	}
}