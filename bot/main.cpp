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
		signal(SIGINT, handler);
		if (ac == 2)
		{
			Bot bot(&sig, av[1]);
			bot.talk("Je suis VIVANT ! (ac = 2)\n");
			bot.online();
		}
		else if (ac == 3)
		{
			Bot bot(&sig, av[1], atoi(av[2]));
			bot.talk("Je suis VIVANT ! (ac = 3)\n");
			bot.online();
		}
		else if (ac == 4)
		{
			Bot bot(&sig, av[1], atoi(av[2]), av[3]);
			bot.talk("Je suis VIVANT ! (ac = 4)\n");
			bot.online();
		}
		else if (ac == 5)
		{
			Bot bot(&sig, av[1], atoi(av[2]), av[3], av[4]);
			bot.talk("Je suis VIVANT ! (ac = 5)\n");
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
