// Server side C/C++ program to demonstrate Socket
// programming
#include "restaurant_owner.h"
#define PORT 8080
void find_inger(int i, char *list)
{
	char *cur = strtok(list, "\"");
	cur = strtok(NULL, "\"");
	int count = 0;
	while (cur)
	{

		memset(recipes[i].ingrediants[count].name, 0, 32);
		strcpy(recipes[i].ingrediants[count].name, cur);
		count++;
		cur = strtok(NULL, "\"");
		cur = strtok(NULL, "\"");
	}
	recipes[i].size = count;
}
void make_quant(char *quant, int i)
{
	char *cur = strtok(quant, ",");
	int count = 0;
	while (cur)
	{
		recipes[i].ingrediants[count].quantity = atoi(cur);
		cur = strtok(NULL, ",");
		count++;
	}
}
void broadcast_massage(char *msg)
{
	sendto(udp_socket, msg, strlen(msg), 0,
		   (const struct sockaddr *)&udp_addr, sizeof(udp_addr));
	char log_buf[64];
	sprintf(log_buf, "\"%s\" sent on %d udp port", msg, udp_port);
	log(user_name, log_buf);
}
void red_json()
{
	int user_name = open(CSV_PATH, O_RDONLY);
	char file_buf[2048];
	read(user_name, file_buf, 2048);
	close(user_name);
	char *first_line = strtok(file_buf, "\n");
	first_line = strtok(NULL, "}");
	int count = 0;
	char foods[10][256];
	while (first_line)
	{
		strcpy(foods[count], first_line);
		count++;
		first_line = strtok(NULL, "}");
	}
	recipes_size = count - 1;
	for (int i = 0; i < count - 1; i++)
	{
		char *food = foods[i];
		char *line1, *line2, *line3;
		char *temp;
		temp = strtok(food, "\n");
		temp = strtok(NULL, "\n");
		line1 = strtok(NULL, "\n");
		line2 = strtok(NULL, "\n");
		line3 = strtok(NULL, "\n");
		char *food_name = line1 + 17;
		char *ings = line2 + 23;
		char *quant = line3 + 21;
		int name_len = strlen(food_name);
		food_name[name_len - 2] = '\0';
		strcpy(recipes[i].name, food_name);
		find_inger(i, ings);
		int quant_len = strlen(quant);
		quant[quant_len - 1] = '\0';
		make_quant(quant, i);
	}
}
void open_broadcast()
{
	char temp_buf[1024];
	sprintf(temp_buf, "%s %s", new_restaurant, user_name);
	sendto(udp_socket, temp_buf, strlen(temp_buf), 0,
		   (const struct sockaddr *)&udp_addr, sizeof(udp_addr));
	char log_buf[64];
	sprintf(log_buf, "\"%s\" sent on %d udp port", temp_buf, udp_port);
	log(user_name, log_buf);
}
void close_broadcast()
{
	char temp_buf[1024];
	sprintf(temp_buf, "%s %s", close_restaurant, user_name);
	sendto(udp_socket, temp_buf, strlen(temp_buf), 0,
		   (const struct sockaddr *)&udp_addr, sizeof(udp_addr));
	char log_buf[64];
	sprintf(log_buf, "\"%s\" sent on %d udp port", temp_buf, udp_port);
	log(user_name, log_buf);
}
short inger_id_finder(char *name)
{
	for (int i = 0; i < size_ingrediants; i++)
	{
		if (!strcmp(name, ingrediants[i].name))
		{
			return i;
		}
	}
	return -1;
}
void add_ingredient(char *name, int quant)
{
	int i = inger_id_finder(name);
	if (i > -1)
	{
		ingrediants[i].quantity += quant;
	}
	else
	{
		ingrediants[size_ingrediants].quantity = quant;
		strcpy(ingrediants[size_ingrediants].name, name);
		size_ingrediants++;
	}
}
int connectServer(int port)
{
	int fd;
	struct sockaddr_in server_address;

	fd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	;

	if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
	{ // checking for errors
		printf("Error in connecting to server\n");
		return -1;
	}

	return fd;
}
int id_py_port(int port)
{
	for (int i = 0; i < clinets_size; i++)
	{
		if (clients[i].port == port)
		{
			return i;
		}
	}
}
short recipe_index(char *name)
{
	for (int i = 0; i < recipes_size; i++)
	{
		if (!strcmp(name, recipes[i].name))
		{
			return i;
		}
	}
	return -1;
}
short get_by_port(int port)
{
	for (int i = 0; i < clinets_size; i++)
	{
		if (clients[i].port == port)
		{
			return i;
		}
	}
	return -1;
}
int command_handler()
{
	char command[MAX_COMMAND_SIZE];
	get_input(command);
	if (!command)
	{
		return 0;
	}
	if (!strcmp(command, START))
	{
		if (status == 0)
		{
			status = 1;
			open_broadcast();
		}
	}
	if (status == 1)
	{
		if (!strcmp(command, END))
		{
			// if trhere is nothing on the tcp port
			if (status == 0)
			{
				print("it has already closed");
			}
			else
			{
				status = 0;
				close_broadcast();
			}
		}
		else if (!strcmp(command, ILIST))
		{
			for (int i = 0; i < size_ingrediants; i++)
			{
				char print_buf[72];
				sprintf(print_buf, "%d--> %s %d", i + 1, ingrediants[i].name, ingrediants[i].quantity);
				print(print_buf);
			}
		}
		else if (!strcmp(command, RECIPES))
		{
			char print_this[2048];
			memset(print_this, 0, 2048);
			for (int i = 0; i < recipes_size; i++)
			{
				char temp[256];
				sprintf(temp, "%d %s", i + 1, recipes[i].name);
				for (int j = 0; j < recipes[i].size; j++)
				{
					char temp2[64];
					sprintf(temp2, "%s : %d", recipes[i].ingrediants[j].name, recipes[i].ingrediants[j].quantity);
					char *pass = (temp ? "\n" : "");
					sprintf(temp, "%s%s%s", temp, pass, temp2);
				}
				sprintf(print_this, "%s\n\n%s", print_this, temp);
			}
			print_n("-->");
			print(print_this);
		}
		else if (!strcmp(command, SUPPLY))
		{
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 300;
			broadcast_massage(give_supliers);
			char log_buf[64];
			sprintf(log_buf, "\"%s\" sent on %d udp port", give_supliers, udp_port);
			log(user_name, log_buf);
			fd_set working, master;
			FD_ZERO(&master);
			FD_ZERO(&working);
			FD_SET(udp_socket, &master);
			int num = 1;
			while ("i hate os")
			{
				working = master;
				select(udp_socket + 1, &working, NULL, NULL, &tv);
				if (FD_ISSET(udp_socket, &working))
				{

					char temp_buffer[1024];
					memset(temp_buffer, 0, 1024);
					recv(udp_socket, temp_buffer, 1024, 0);
					if (!strcmp(temp_buffer, give_supliers))
					{
						continue;
					}
					char *first = strtok(temp_buffer, "|");
					if (!strcmp(first, "$"))
					{
						char *second = strtok(NULL, "|");
						char *third = strtok(NULL, "|");
						char print_buf[64];
						sprintf(print_buf, "%d--> name: %s\tport: %s", num++, second, third);
						print(print_buf);
					}
					else
					{
						continue;
						// break;
					}
					sprintf(log_buf, "\"%s\" received on %d udp port", temp_buffer, udp_port);
					log(user_name, log_buf);
				}
				else if (FD_ISSET(STD_IN, &working))
				{
					continue;
				}
				else
				{
					break;
				}
			}
		}
		else if (!strcmp(command, REQUEST))
		{
			print_n("supplier port: ");
			char port_char[10];
			get_input(port_char);
			int port = get_port(port_char);
			char ing_name[32], number[10];
			print_n("name of ingredient: ");
			get_input(ing_name);
			print_n("number of ingredient: ");
			get_input(number);
			int num_of_ing = atoi(number);
			int server_fd = connectServer(port);
			char log_buf[64];
			sprintf(log_buf, "connect to  %d tcp port", port);
			log(user_name, log_buf);
			if (server_fd < 0)
			{
				return 0;
			}
			print("waiting for supplier answer");
			char send_buff[64];
			sprintf(send_buff, "r|%s|%d|%d", ing_name, num_of_ing, tcp_port);
			send(server_fd, send_buff, strlen(send_buff), 0);
			sprintf(log_buf, "\"%s\" sent to  %d tcp port", send_buff, port);
			log(user_name, log_buf);
			fd_set m;
			FD_ZERO(&m);
			struct timeval tv;
			tv.tv_sec = 90;
			tv.tv_usec = 0;
			FD_SET(server_fd, &m);
			select(server_fd + 1, &m, NULL, NULL, &tv);
			char answer[32];
			if (FD_ISSET(server_fd, &m))
			{
				recv(server_fd, answer, 32, 0);
				if (!strcmp(answer, "yes"))
				{
					add_ingredient(ing_name, num_of_ing);
					print("done");
				}
				else
				{
					print("couldn't get this one");
				}
				sprintf(log_buf, "\"%s\" received on %d udp port", answer, tcp_port);
				log(user_name, log_buf);
			}
			else
			{
				perror("your time is up bitch");
			}
			memset(send_buff, 0, 77);
			sprintf(send_buff, "CLOSE_PORT_R|%d", tcp_port);
			send(server_fd, send_buff, strlen(send_buff), 0);
			sprintf(log_buf, "\"%s\" sent to  %d tcp port", send_buff, port);
			log(user_name, log_buf);
			close(server_fd);
		}
		else if (!strcmp(command, RLIST))
		{
			for (int i = 0; i < clinets_size; i++)
			{
				if (clients[i].status == WATING)
				{
					char print_buff[128];
					sprintf(print_buff, "name: %s\t food: %s", clients[i].user_name, clients[i].name);
					print(print_buff);
				}
			}
		}
		else if (!strcmp(command, ANS))
		{
			char port_char[10];
			print_n(request_port);
			get_input(port_char);
			int port_val = get_port(port_char);
			print_n("your answer (yes/no): ");
			char ans[7];
			get_input(ans);
			int index = get_by_port(port_val);
			char massage[72];
			if (index != -1)
			{
				if (!strcmp(ans, "yes"))
				{
					short flag = 1;
					int recipes_index = recipe_index(clients[index].name);
					for (int j = 0; j < recipes[recipes_index].size; j++)
					{
						int ing_index = inger_id_finder(recipes[recipes_index].ingrediants[j].name);
						if (ing_index == -1 || ingrediants[ing_index].quantity < recipes[recipes_index].ingrediants[j].quantity)
						{
							flag = 0;
						}
					}
					if (flag)
					{
						for (int j = 0; j < recipes[recipes_index].size; j++)
						{
							int ing_index = inger_id_finder(recipes[recipes_index].ingrediants[j].name);
							ingrediants[ing_index].quantity -= recipes[recipes_index].ingrediants[j].quantity;
						}
						clients[index].status = ACCEPT;
						sprintf(massage, "accepted");
					}
					else
					{
						clients[index].status = REJECT;
						sprintf(massage, "reject; we dont have supplies");
						perror("we dont have supplies");
					}
				}
				else if (!strcmp(ans, "no"))
				{
					clients[index].status = REJECT;
					sprintf(massage, "request rejected");
				}
				send(clients[index].soket, massage, strlen(massage), 0);
				char log_buf[64];
				sprintf(log_buf, "\"%s\" sent to  %d tcp port", massage, port_val);
				log(user_name, log_buf);
			}
			else
			{
				perror("we dont have this request in list");
			}
		}
		else if (!strcmp(command, HIST))
		{
			for (int i = 0; i < clinets_size; i++)
			{
				char status_buf[10];

				if (clients[i].status == WATING)
				{
					continue;
				}
				else if (clients[i].status == ACCEPT)
				{
					sprintf(status_buf, "accepted");
				}
				else if (clients[i].status == REJECT)
				{
					sprintf(status_buf, "denied");
				}
				char print_buff[128];
				sprintf(print_buff, "name: %s\t food: %s \t", clients[i].user_name, clients[i].name, status_buf);
				print(print_buff);
			}
		}
	}
}
void handle_broadcast()
{
	char temp_buffer[1024];
	memset(temp_buffer, 0, 1024);
	recv(udp_socket, temp_buffer, 1024, 0);
	char log_buf[64];
	sprintf(log_buf, "\"%s\" sent on %d udp port", temp_buffer, udp_port);
	log(user_name, log_buf);

	if (strcmp(temp_buffer, sign_up) == 0)
	{
		sendto(udp_socket, name, strlen(name), 0, (const struct sockaddr *)&udp_addr, sizeof(udp_addr));
		print(NEW_USER);
		print_n(">> ");
	}
	else if (!strcmp(temp_buffer, give_restaurant))
	{
		char send_buf[64];
		sprintf(send_buf, "&|%s|%d", user_name, tcp_port);
		broadcast_massage(send_buf);
	}
}
short client_tcp(char *buf, int socket, addr adress)
{
	char *C = strtok(buf, "|");
	if (!strcmp(C, "CLOSE_PORT"))
	{

		int port = get_port(strtok(NULL, "|"));
		int index = get_by_port(port);
		print_n("customer :");
		print_n(clients[index].user_name);
		print_n("leaved \n");
		close(clients[index].soket);
		clients[index].status = REJECT;
		return 1;
	}
	else if (!strcmp(C, "c"))
	{
		print("New customer request!");
		char *user_name = strtok(NULL, "|");
		char *food_name = strtok(NULL, "|");
		int port = get_port(strtok(NULL, "|"));
		clients[clinets_size].port = port;
		clients[clinets_size].status = WATING;
		clients[clinets_size].soket = socket;
		clients[clinets_size].adress = adress;
		strcpy(clients[clinets_size].name, food_name);
		strcpy(clients[clinets_size].user_name, user_name);
		clinets_size++;
		return 1;
	}
	return 0;
}
void handle_tcp()
{
	char tcp_buf[77];
	memset(tcp_buf, 0, 77);

	addr client_address;
	int address_len = sizeof(client_address);
	int client_fd = accept(tcp_socket_fd, (struct sockaddr *)&client_address, (socklen_t *)&address_len);
	recv(client_fd, tcp_buf, 77, 0);
	char log_buf[64];
	sprintf(log_buf, "\"%s\" received on %d tcp port", tcp_buf, tcp_port);
	log(user_name, log_buf);
	if (status == 0)
	{
		char massage[72];
		sprintf(massage, "request rejected");
		send(client_fd, massage, strlen(massage), 0);
		char log_buf[64];
		sprintf(log_buf, "\"%s\" sent on %d tcp socket", massage, client_address.sin_port);
		log(user_name, log_buf);
	}
	else
	{
		int a = client_tcp(tcp_buf, client_fd, client_address);
		if (a == 0)
		{
			perror("wrong request");
			char massage[72];
			sprintf(massage, "wrong request");
			send(client_fd, massage, strlen(massage), 0);
			char log_buf[64];
			sprintf(log_buf, "\"%s\" sent on %d tcp socket", massage, client_address.sin_port);
			log(user_name, log_buf);
		}
	}
}
void run_restaurant()
{
	open_broadcast();
	int max_fd = udp_socket;
	if (tcp_socket_fd > udp_socket)
	{
		max_fd = tcp_socket_fd;
	}
	fd_set master, working;
	FD_ZERO(&master);
	FD_ZERO(&working);
	FD_SET(STD_IN, &master);
	FD_SET(udp_socket, &master);
	FD_SET(tcp_socket_fd, &master);
	print_n(START_OF_COMMAND);

	while (1 > 0)
	{
		working = master;
		select(max_fd + 1, &working, NULL, NULL, NULL);
		if (FD_ISSET(udp_socket, &working))
		{
			handle_broadcast();
		}
		if (FD_ISSET(STD_IN, &working))
		{
			command_handler();
			print_n(START_OF_COMMAND);
		}
		if (FD_ISSET(tcp_socket_fd, &working))
		{
			handle_tcp();
			print_n(START_OF_COMMAND);
		}
	}
}
int main(int argc, char const *argv[])
{

	print("please enter TCP port");
	char temp[10];
	get_input(temp);
	tcp_port = get_port(temp);
	tcp_socket_fd = make_tcp_socket(tcp_port, &tcp_address);
	listen(tcp_socket_fd, 20);
	udp_socket = setup_port(argc, argv, user_name, tcp_port, &udp_port, &udp_addr);
	red_json();

	status = 1;
	run_restaurant();
	close(user_name);
}
