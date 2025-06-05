#ifndef SERVER_HPP
#define SERVER_HPP
# include "ircserv.hpp"

class Server
{
	private:
		int						_serverFd;
		int						_port;
		std::string				_password;
		std::string				_host;
		std::map<int, Client>	_clients;
		std::map<std::string, Channel> _channels;
		fd_set					_readFds;
		int						_maxFd;
		Server();
	public:
		Server(int port, const std::string &password);
		Server(const Server &obj);
		Server &operator=(const Server &obj);
		~Server();

		// Getters
		int		getServerFd() const;
		const	std::string &getPassword() const;
		const	std::map<int, Client> &getClients() const;
		int		getPort();
		void	setPassword(const std::string &pass);

		//server setup
		bool	start();
		void	run();
		void	acceptNewClient();
		void	handleClientMessage(int clientFd);
		void	removeClient(int clientFd);
	
	private:
		void	setupSocket();
		void	updateFdSet();
		void	processClientData(Client &client);
		void	parseCommand(Client &client, const std::string &msg);
};

#endif