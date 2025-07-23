#include "Bot.hpp"

Bot::Bot(bool *sig, std::string addr, int port, std::string pass, std::string nick): sig(sig), addr(addr), pass(pass), nick(nick)
{
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("socket() failed");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	struct addrinfo hints, *servinfo;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	rv = getaddrinfo( addr.c_str() , 0 , &hints , &servinfo);
	(void) rv;

	serv_addr.sin_addr = ((struct sockaddr_in *) servinfo->ai_addr)->sin_addr;
	std::cout << "Connecting to " << addr << ":" << port << std::endl;
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
		throw std::runtime_error("connect() failed");
	freeaddrinfo(servinfo);
}

Bot::~Bot()
{
	std::cout << "So long !" << std::endl;
	send_command("QUIT");
	close(sock);
}

void	Bot::talk(std::string message)
{
	std::cout << message << std::endl;
}

static std::string extractMessageContent(const std::string& line)
{
	size_t pos = line.find(" :");
	if (pos == std::string::npos)
		return "";
	return line.substr(pos + 2);
}

void	Bot::online()
{
	if (!pass.empty())
		send_command("PASS " + pass);
	messages.insert(messages.begin(), "\0");
	std::memset(buf, 0, sizeof(buf));
	connected = false;
	while (!connected)
	{
		try
		{
			send_command("NICK " + nick);
			send_command("USER " + nick + " " + nick + " " + addr + " :" + nick);
			serv_response();

			std::string msg = messages.front();
			messages.erase(messages.begin());
			if (msg.find("433") != std::string::npos)
				throw std::runtime_error("Nickname already in use");
			connected = true;
		}
		catch (std::runtime_error &e)
		{
			std::cerr << e.what() << std::endl;
			nick += "_";
		}
	}
	while (!*sig)
	{
		if (messages.empty())
			serv_response();
		std::string msg = messages.front();
		messages.erase(messages.begin());
		if (msg.find("001 " + nick) != std::string::npos)
			break;
	}
	send_command("JOIN " + nick);
	serv_response();
	std::string msg = messages.front();
	while (!*sig)
	{
		if (messages.empty())
			serv_response();
		std::string msg = extractMessageContent(messages.front());
		messages.erase(messages.begin());
		if (msg.empty())
			continue;
		else if (msg.find("TIME") != std::string::npos)
			print_current_time();
		else if (msg.find("RBANWORD") != std::string::npos)
		{
			std::string toBan = msg.substr(std::string("RBANWORD").length());
			while (!toBan.empty() && toBan[0] == ' ')
				toBan.erase(0, 1);
			if (toBan.empty())
				continue;
			removeBannedWord(toBan, "bot/banlist.txt");
		}
		else if (msg.find("ABANWORD") != std::string::npos)
		{
			std::string toBan = msg.substr(std::string("ABANWORD").length());
			while (!toBan.empty() && toBan[0] == ' ')
			toBan.erase(0, 1);
			if (toBan.empty())
			continue;
			addBannedWord(toBan, "bot/banlist.txt");
		}
		else if (msg.find("BANLIST") != std::string::npos)
		{
			displayBanlist("bot/banlist.txt");
		}
		else
			containsBannedWord(msg, "bot/banlist.txt");
	}
}

void	Bot::send_command(std::string command)
{
	command += "\r\n";
	if (send(sock, command.c_str(), command.size(), 0) < 0)
		throw std::runtime_error("send() failed");
}

void	Bot::serv_response()
{
	struct pollfd pfd;

	pfd.fd = sock;
	pfd.events = POLLIN;
	pfd.revents = 0;
	if (poll(&pfd, 1, -1) == -1)
	{
		if (pfd.revents & POLLERR)
			throw std::runtime_error("poll() failed");
		messages.clear();
		messages.push_back("");
		return;
	}
	ssize_t len = recv(sock, &buf, 4096, 0);
	if (len < 0)
		throw std::runtime_error("recv() failed");
	if (len == 0)
		throw std::runtime_error("Connection closed");
	buf[len] = '\0';
	receive += buf;
	while (receive.find("\r\n") != std::string::npos)
	{
		std::string msg = receive.substr(0, receive.find("\r\n"));
		receive.erase(0, receive.find("\r\n") + 2);
		messages.push_back(msg);
	}
}

void	Bot::print_current_time()
{
	std::time_t now = std::time(NULL);
	std::tm* local = std::localtime(&now);

	char buffer[9];
	std::strftime(buffer, sizeof(buffer), "%H:%M:%S", local);

	std::cout << "Il est " << buffer << std::endl;
	send_command("PRIVMSG #" + nick + " Il est " + buffer + ".");
}



static std::string toLower(const std::string& str)
{
	std::string result = str;
	for (size_t i = 0; i < result.length(); ++i)
		result[i] = std::tolower(result[i]);
	return result;
}

static std::set<std::string> loadBanWordsFromFile(const std::string& filename)
{
	std::set<std::string> result;
	std::ifstream infile(filename.c_str());
	std::string line;

	if (!infile)
	{
		std::cerr << "[ERROR] Could not open banlist file: " << filename << std::endl;
		return result;
	}

	while (std::getline(infile, line))
	{
		if (!line.empty())
			result.insert(toLower(line));
	}
	infile.close();
	return result;
}

bool Bot::containsBannedWord(const std::string& message, const std::string& filename)
{
	std::set<std::string> banned = loadBanWordsFromFile(filename);
	std::istringstream iss(message);
	std::string word;

	while (iss >> word)
	{
		if (banned.find(toLower(word)) != banned.end())
		{
			send_command("PRIVMSG #" + nick + " WARNING ! BAD LANGUAGE DETECTED !");
			std::cout << "WARNING ! BAD LANGUAGE DETECTED !" << std::endl;
			return true;
		}
	}
	return false;
}

void Bot::addBannedWord(const std::string& word, const std::string& filename)
{
	std::set<std::string> banned = loadBanWordsFromFile(filename);
	std::string lower = toLower(word);
	std::istringstream iss(lower);
	std::string words;

	while (iss >> words)
	{
		if (banned.find(words) != banned.end())
		{
			send_command("PRIVMSG #" + nick + " Word already in banlist !");
			std::cout << "Word already in banlist !" << std::endl;
			return;
		}
		send_command("PRIVMSG #" + nick + " Word added in banlist !");
		std::cout << "Word added in banlist !" << std::endl;

		std::ofstream outfile(filename.c_str(), std::ios::app);
		if (!outfile)
		{
			std::cerr << "[ERROR] Could not open file to append: " << filename << std::endl;
			return;
		}

		outfile << words << std::endl;
		outfile.close();
	}
	return;
}

void Bot::removeBannedWord(const std::string& word, const std::string& filename)
{
	std::set<std::string> banned = loadBanWordsFromFile(filename);
	std::string lower = toLower(word);
	std::istringstream iss(lower);
	std::string words;

	while (iss >> words)
	{
		std::set<std::string>::iterator it = banned.find(words);
		if (it == banned.end())
		{
			send_command("PRIVMSG #" + nick + " Word absent from banlist !");
			std::cout << "Word absent form banlist !" << std::endl;
			return;
		}
		send_command("PRIVMSG #" + nick + " Word removed from banlist !");
		std::cout << "Word removed from banlist !" << std::endl;

		banned.erase(it);

		std::ofstream outfile(filename.c_str());
		if (!outfile)
		{
			std::cerr << "[ERROR] Could not open file to rewrite: " << filename << std::endl;
			return;
		}

		for (std::set<std::string>::iterator it = banned.begin(); it != banned.end(); ++it)
			outfile << *it << std::endl;

		outfile.close();
	}
	return;
}

void Bot::displayBanlist(const std::string& filename)
{
	std::set<std::string> banned = loadBanWordsFromFile(filename);
	std::set<std::string>::iterator it = banned.begin();

	while (it != banned.end())
	{
		std::cout << *it << std::endl;
		send_command("PRIVMSG #" + nick + " " + *it);
		it++;
	}
	return;
}