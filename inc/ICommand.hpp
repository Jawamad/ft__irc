#ifndef ICOMMAND_HPP
# define ICOMMAND_HPP

# include "ircserv.hpp"

class Server;
class ICommand
{
	public:
		ICommand();
		ICommand(ICommand& obj);
		ICommand& operator=(ICommand& obj);
		virtual ~ICommand();
		virtual void execute(Server &server, Client *client, std::istringstream &args) = 0;
};
#endif