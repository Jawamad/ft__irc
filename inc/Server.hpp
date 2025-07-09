#ifndef SERVER_HPP
#define SERVER_HPP
# include "ircserv.hpp"

class Server
{
	private:
		int									_serverFd;
		int									_port;
		std::string							_password;
		std::string							_host;
		std::string							_creationTime;
		std::string							_name;
		std::map<int, Client *>				_clients;
		std::map<std::string, Channel *>	_channels;
		fd_set								_readFds;
		int									_maxFd;
		std::map<std::string, ICommand *>	_commands;
		Server();

		void	setupSocket();
		void	updateFdSet();
		void	processClientData(Client* client);
		void	parseCommand(Client* client, const std::string &msg);

	public:
		Server(int port, const std::string &password);
		Server(const Server &obj);
		Server &operator=(const Server &obj);
		~Server();

		// Getters
		int											getServerFd() const;
		int											getPort();
		const	std::string							&getPassword() const;
		const	std::string							&getHost() const;
		const	std::string							&getCreationTime() const;
		const	std::string							&getName() const;
		std::map<int, Client*>						&getClients();
		const	std::map<int, Client*>				&getClients() const;
		std::map<std::string, Channel*>				&getChannels();
		const	std::map<std::string, Channel*>		&getChannels() const;
		const	std::map<std::string, ICommand*>	&getCommands() const;
		Channel										*getChan(std::string chanName);
		
		void	setPassword(const std::string &pass);
		void	addChannel(std::string chanName);
		void	delChannel(std::string chanName);
		void	delClient(int fd);
		bool	hasChannel(std::string chanName);
		Client*	getClientByNick(std::string nickname);

		// server setup
		bool	start();
		void	run();
		void	acceptNewClient();
		void	handleClientMessage(int clientFd);
		void	removeClient(int clientFd);

		// operator
		Client*	findClientByNickname(const std::string& nickname);
		Client*	findClientByFd(int clientFd);
		
		// MODE
		void	invitationToAccess(int guestFd, int clientFd, const std::string &channelName);

		// utils
		void	serverMessage(Client* client, std::string errorCode,  const std::string& errorMsg);
		void	sendCommandMessage(Client* sender, const std::string& command, const std::string& params, const std::string& trailing);
		void	sendNumericReply(Client* target, int code, const std::string& params, const std::string& trailing);
};

#endif