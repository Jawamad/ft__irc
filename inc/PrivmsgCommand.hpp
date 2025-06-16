#ifndef PRIVMSGCOMMAND_HPP
# define PRIVMSGCOMMAND_HPP

#include "ICommand.hpp"

class PrivmsgCommand: public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif