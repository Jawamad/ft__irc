#ifndef WHOCOMMAND_HPP
# define WHOCOMMAND_HPP

#include "ICommand.hpp"

class WhoCommand : public ICommand
{
	private:
		void sendWhoReply(Client *requester, const std::string &channel, Client *target);
		void sendEndOfWho(Client *requester, const std::string &channel);
	public:
		void execute(Server &server, Client *client, std::istringstream &args);
};

#endif