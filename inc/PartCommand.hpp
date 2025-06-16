#ifndef PARTCOMMAND_HPP
# define PARTCOMMAND_HPP

#include "ICommand.hpp"

class PartCommand: public ICommand
{
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif