#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "ircserv.hpp"

class Client
{
	private:
		int			_socketFd;
		std::string	_nickname;
		std::string	_username;
		bool 		_loggedIn;
		std::string _ip;
		std::string	_buffer;
	public:
		Client();
		Client(int fd, const std::string username, const std::string &ip);
		Client(const Client& obj);
		~Client();
		Client& operator=(const Client& obj);

		// getters
		int		getSocketFd() const;
		const	std::string &getIp() const;
		const	std::string &getNickname() const;
		const	std::string &getUsername() const;
		const	std::string &getBuffer() const;
		bool	isLoggedIn() const;

		// Setters
		void setNickname(const std::string &nickname);
		void setUsername(const std::string &username);
		void setLoggedIn(bool loggedIn);
		void appendToBuffer(const std::string &data);
		void clearBuffer();

		// network
		std::string receiveMessage();
};

#endif