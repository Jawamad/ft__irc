#ifndef QUITCOMMAND_HPP
# define QUITCOMMAND_HPP

#include "ICommand.hpp"

class QuitCommand: public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif