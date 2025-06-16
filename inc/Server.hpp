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
		std::map<int, Client *>				_clients;
		std::map<std::string, Channel *>	_channels;
		fd_set								_readFds;
		int									_maxFd;
		std::map<std::string, ICommand *>	_commands;
		Server();
	public:
		Server(int port, const std::string &password);
		Server(const Server &obj);
		Server &operator=(const Server &obj);
		~Server();

		// Getters
		int		getServerFd() const;
		const	std::string &getPassword() const;
		const	std::map<int, Client*> &getClients() const;
		std::map<std::string, Channel*> &getChannels();
		const	std::map<std::string, Channel*> &getChannels() const;
		const	std::map<std::string, ICommand*> &getCommands() const;
		Channel	*getChan(std::string chanName);
		int		getPort();
		void	setPassword(const std::string &pass);
		void	addChannel(std::string chanName);
		void	delChannel(std::string chanName);
		void	delClient(int fd);

		//server setup
		bool	start();
		void	run();
		void	acceptNewClient();
		void	handleClientMessage(int clientFd);
		void	removeClient(int clientFd);

		// operator
		Client* findClientByNickname(const std::string& nickname);

	private:
		void	setupSocket();
		void	updateFdSet();
		void	processClientData(Client* client);
		void	parseCommand(Client* client, const std::string &msg);
};

#endif