// #include "../inc/ircserv.hpp"

// int main(int ac, char** av)
// {
// 	if (ac != 3)
// 	{
// 		std::cerr << "Error: Wrong number of arguments (/ircserv <port> <password>)" << std::endl;
// 		return (1);
// 	}
// 	int port = atoi(av[1]);
// 	int client_fd, server_fd;
// 	std::string pwd = av[2];
// 	struct sockaddr_in server_addr, client_addr;
// 	socklen_t client_len = sizeof(client_addr);
// 	char temp_buffer[1024];
// 	//socket
// 	server_fd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (server_fd < 0)
// 	{
// 		std::cerr << "Error: socket" << std::endl;
// 		return (1);
// 	}
// 	//reuse port
// 	int opt = 1;
// 	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
// 	//bind and listen setup
// 	std::memset(&server_addr, 0, sizeof(server_addr));
// 	server_addr.sin_family = AF_INET;
// 	server_addr.sin_addr.s_addr = INADDR_ANY;
// 	server_addr.sin_port = htons(port);
// 	if (bind(server_fd, (struct  sockaddr *)&server_addr, sizeof(server_addr)) < 0)
// 	{
// 		std::cerr << "Error: bind" << std::endl;
// 		close(server_fd);
// 		return (1);
// 	}
// 	if (listen(server_fd, 5) < 0)
// 	{
// 		std::cerr << "Error listen" << std::endl;
// 		close(server_fd);
// 		return 1;
// 	}
// 	std::cout << "IRC server: waiting for connection." << std::endl;

// 	while(true)
// 	{
// 		client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
// 		if(client_fd < 0)
// 		{
// 			std::cerr << "Error: accept" << std::endl;
// 			continue;
// 		}
// 		std::cout << "client connected : " << inet_ntoa(client_addr.sin_addr) << std::endl;
// 		std::string message;
// 		std::memset(temp_buffer, 0, sizeof(temp_buffer));
// 		int	bytes_received;
// 		while ((bytes_received= recv(client_fd, temp_buffer, sizeof(temp_buffer) - 1, 0)) > 0)
// 		if (bytes_received > 0)
// 		{
// 			temp_buffer[bytes_received]= '\0';
// 			message += temp_buffer;
// 			std::cout << "Message received : "<< message;
// 			message.clear();
// 		}
// 		std::cout << "Connection lost with client." << std::endl;
// 		close(client_fd);
// 	}
// 	close(server_fd);
// }