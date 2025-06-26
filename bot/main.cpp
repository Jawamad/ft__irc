#include "Bot.hpp"

bool sig = false;

void handler(int)
{
	sig = true;
}

int	main(int ac, char **av)
{
	try
	{
		if (ac == 5)
		{
			signal(SIGINT, handler);
			Bot bot(&sig, av[1], atoi(av[2]), av[3], av[4]);
			bot.talk("Je suis VIVANT !\n");
			bot.online();
			return EXIT_SUCCESS;
		}
		else
		{
			std::cout << "HOW TO USE : ./bot [host] [port] [password] [nickname]" << std::endl;
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		if (std::string(e.what()) == "Connection closed")
			return EXIT_SUCCESS;
		return EXIT_FAILURE;
	}
}