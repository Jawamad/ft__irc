#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ircserv.hpp"
#include <set>

class Client;

class Channel
{
	private:
		std::string				_name;
		std::string				_topic;
		std::string				_creationTime;
		std::map<int, Client*>	_clients;
		std::set<int>			_operators;

		// MODE 
		bool			_inviteOnly;
		bool			_topicOperatorOnly;
		bool			_accessPwdOnly;
		bool			_hasUserLimit;
		std::string		_chanPassword;
		int				_userLimit;

	public:
		Channel();
		Channel(const std::string &name);
		Channel(const Channel& obj);
		~Channel();
		Channel& operator=(const Channel& obj);

		const std::string&				getName() const;
		const std::string&				getChanPassword();
		const std::string&				getTopic() const;
		const std::map<int, Client*>&	getClients()const;
		int								getUserLimit() const;
		const std::string&				getCreationTime() const;

		//
		void	setTopic(const std::string &topic);
		void	setChanPassword(const std::string &chanPassword);

		// MODE
		bool	isInviteOnly() const;
		void	setInviteOnly(bool status);

		bool	topicIsOperatorModOnly() const;
		void	setTopicStatus(bool status);

		bool	isPasswordOnly() const;
		void	setPasswordStatus(bool status);

		void 	setLimitedNbUser(bool status);
		bool	isLimitedNbUser() const;
		void	setUserLimit(int limit);

		//

		void	addClient(Client* client);

		//void	getClientbyFd(int clientFd);
		void	removeClient(int clientFd);
		bool	hasClient(int clientFd) const;
		void	broadcast( const std::string &message, int senderFd);
		int		getClientCount() const;

		bool	isOperator(int clientFd) const;
		void	addOperator(int clientFd);
		void	removeOperator(int clientFd);

		// operator
		void	clientGetsKickByOperator(const std::string &nickName, Client &client);
		void	clientGetsInviteByOperator(const std::string &nickName, Client &client); 
		Client*	searchMember(const std::string& nickname) const;
};

#endif