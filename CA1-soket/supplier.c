#include "user.c"
#include "supplier.h"
#include "log.c"
#include "defines.h"
void broadcast_massage(char *msg)
{
    sendto(udp_socket, msg, strlen(msg), 0,
           (const struct sockaddr *)&udp_addr, sizeof(udp_addr));
    char log_buf[64];
    sprintf(log_buf, "\"%s\" sent on %d udp port", msg, udp_port);
    log(user_name, log_buf);
}
void handle_broadcast()
{
    char temp_buffer[1024];
    memset(temp_buffer, 0, 1024);
    recv(udp_socket, temp_buffer, 1024, 0);
    if (strcmp(temp_buffer, sign_up) == 0)
    {
        sendto(udp_socket, name, strlen(name), 0, (const struct sockaddr *)&udp_addr, sizeof(udp_addr));
        char log_buf[64];
        sprintf(log_buf, "\"%s\" sent on %d udp port", name, udp_port);
        log(user_name, log_buf);
        print(NEW_USER);
        print_n(">> ");
    }
    else if (!strcmp(temp_buffer, give_supliers))
    {
        char send_buf[40];
        sprintf(send_buf, "$|%s|%d", user_name, tcp_port);
        broadcast_massage(send_buf);
        char log_buf[64];
        sprintf(log_buf, "\"%s\" sent on %d udp port", send_buf, udp_port);
        log(user_name, log_buf);
    }
}
short restaurant_tcp(char *buf, int client_fd)
{
    char *R = strtok(buf, "|");
    if (!strcmp(R, "CLOSE_PORT_R"))
    {
        int port = get_port(strtok(NULL, "|"));

        close(rclinets[r_size - 1].socket);
        return 1;
    }
    else if (!strcmp(R, "r"))
    {
        char *ing_name = strtok(NULL, "|");
        int number = atoi(strtok(NULL, "|"));
        int port = get_port(strtok(NULL, "|"));
        rclinets[r_size].port = port;
        strcmp(rclinets[r_size].ingredient_name, ing_name);
        rclinets[r_size].number = number;
        rclinets[r_size].socket = client_fd;
        r_size++;
        return 1;
    }
    return 0;
}
void tcp_handler()
{
    char tcp_buf[48];
    memset(tcp_buf, 0, 48);
    addr client_address;
    int address_len = sizeof(client_address);
    int client_fd = accept(tcp_socket_fd, (struct sockaddr *)&client_address, (socklen_t *)&address_len);
    recv(client_fd, tcp_buf, 48, 0);
    char log_buf[64];
    sprintf(log_buf, "\"%s\" revieved on %d tcp port", tcp_buf, tcp_port);
    log(user_name, log_buf);
    int a = restaurant_tcp(tcp_buf, client_fd);
    if (a == 0)
    {
        char massage[72];
        sprintf(massage, "wrong request");
        char log_buf[64];
        sprintf(log_buf, "\"%s\" sent on %d tcp port", massage, tcp_port);
        log(user_name, log_buf);
        send(client_fd, massage, strlen(massage), 0);
        close(client_fd);
    }
}
void command_handler()
{
    char command[1024];
    get_input(command);
    if (!command)
    {
        return;
    }
    else if (!strcmp(command, ANSWER_REQUEST))
    {
        print_n("your answer(yes/no): ");
        char ans[10];
        get_input(ans);
        if (!strcmp(ans, "yes"))
        {
            if (r_size != 0)
            {
                send(rclinets[r_size - 1].socket, "yes", 4, 0);
            }
            else
            {
                perror("systems get fucked");
            }
        }
        else if (!strcmp(ans, "no"))
        {
            send(rclinets[r_size - 1].socket, "no", 3, 0);
        }
    }
}
void run_supplier()
{
    fd_set working, master;
    FD_ZERO(&working);
    int max = udp_socket;
    if (tcp_socket_fd > udp_socket)
    {
        max = tcp_socket_fd;
    }
    FD_ZERO(&master);
    FD_SET(udp_socket, &master);
    FD_SET(tcp_socket_fd, &master);
    FD_SET(STD_IN, &master);

    while ("fuck")
    {
        working = master;
        select(max + 1, &working, NULL, NULL, NULL);
        if (FD_ISSET(STD_IN, &working))
        {
            command_handler();
            print_n(START_OF_COMMAND);
        }
        else if (FD_ISSET(udp_socket, &working))
        {
            handle_broadcast();
        }
        else if (FD_ISSET(tcp_socket_fd, &working))
        {

            tcp_handler();
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
    udp_socket = setup_port(argc, argv, user_name, tcp_port, &udp_port, &udp_addr);
    tcp_socket_fd = make_tcp_socket(tcp_port, &tcp_address);
    listen(tcp_socket_fd, 4);
    run_supplier();
}