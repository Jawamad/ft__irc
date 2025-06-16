#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ircserv.hpp"
#include <set>

class Client;

class Channel
{
	private:
		std::string _name;
		std::map<int, Client*> _clients;
		std::set<int> _operators;
	public:
		Channel();
		Channel(const std::string &name);
		Channel(const Channel& obj);
		~Channel();
		Channel& operator=(const Channel& obj);

		const std::string&		getName() const;
		const std::map<int, Client*>&	getClients()const;

		void	addClient(Client* client);
		void	removeClient(int clientFd);
		bool	hasClient(int clientFd) const;
		void	broadcast( const std::string &message, int senderFd);
		size_t	getClientCount() const;

		bool	isOperator(int clientFd) const;
		void	addOperator(int clientFd);
		void	removeOperator(int clientFd);

		Client* findClientByNickname(const std::string& nickname);
		void clientGetsKickByOperator(const std::string &nickName, Client &client);
	
};

#endif