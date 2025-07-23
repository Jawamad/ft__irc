#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "ircserv.hpp"

class Server;
class Client
{
	private:
		int						_socketFd;
		std::string				_nickname;
		std::string				_username;
		bool					_loggedIn;
		bool					_hasPassedPassword;
		bool					_hasUser;
		bool					_hasNick;
		std::string				_hostname;
		std::string				_ip;
		std::string				_buffer;
		std::set<std::string>	_invitedChannel;

	public:
		Client();
		Client(int fd, const std::string &hostname, const std::string &ip);
		Client(const Client& obj);
		~Client();
		Client& operator=(const Client& obj);

		// getters
		int			getSocketFd() const;
		const		std::string &getIp() const;
		const		std::string &getNickname() const;
		const		std::string &getUsername() const;
		const		std::string &getBuffer() const;
		std::string	&getBuffer();
		const		std::string &getHostname() const;
		bool		isLoggedIn() const;
		bool		hasPassedPassword() const;
		bool		hasUser() const;
		bool		hasNick() const;

		// Setters
		void	setNickname(const std::string &nickname);
		void	setUsername(const std::string &username);
		void	setHaspassedPassword(bool value);
		void	setLoggedIn(bool loggedIn);
		void	appendToBuffer(const std::string &data);
		void	clearBuffer();
		void	setHasUser(bool value);
		void	setHasNick(bool value);
	
		// network
		bool	receiveMessage();

		//other
		void	logRoutine(Server& server);

		void	addInvitedList(std::string chan);
		void	delInvitedList(std::string chan);
};

#endif