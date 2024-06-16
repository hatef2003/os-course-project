#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "customer.h"
void time_out()
{
    print("restaurant didn't answered in 120s ");
}
int connectServer(int port)
{
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    { // checking for errors
        printf("Error in connecting to server\n");
        return -1;
    }

    return fd;
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
    int recipes_size = 0;
    int file_fd = open(CSV_PATH, O_RDONLY);
    char file_buf[2048];
    read(file_fd, file_buf, 2048);
    close(file_fd);
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
    for (int i = 0; i < count - 2; i++)
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
        print(food_name);
    }
}
void restaurant_list()
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500;
    broadcast_massage(give_restaurant);
    fd_set working, master;
    FD_ZERO(&master);
    FD_ZERO(&working);
    FD_SET(udp_socket, &master);
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
            if (!strcmp(first, "&"))
            {
                char *second = strtok(NULL, "|");
                char *third = strtok(NULL, "|");
                char print_buf[42];
                sprintf(print_buf, "%s %s", second, third);
                print(print_buf);
            }
            else
            {
                // break;
                continue;
            }
            char log_buf[64];
            sprintf(log_buf, "\"%s\" sent on %d udp port", temp_buffer, udp_port);
            log(user_name, log_buf);
        }
        else
        {
            break;
        }
    }
}
void handle_commands()
{
    char command[256];
    get_input(command);
    if (!strcmp(command, ORDER))
    {
        char massage[77];
        memset(massage, 0, 77);
        char food_name[32];
        char port_char[10];
        print_n(FOOD_NAME);
        get_input(food_name);
        print_n(PORT);
        get_input(port_char);
        int r_port = get_port(port_char);

        sprintf(massage, "c|%s|%s|%d", user_name, food_name, tcp_port);
        int restaurant_fd = connectServer(r_port);
        if (restaurant_fd < 0)
        {
            return;
        }
        send(restaurant_fd, massage, strlen(massage), 0);
        char log_buf[64];
        sprintf(log_buf, "\"%s\" sent on %d tcp port", massage, r_port);
        log(user_name, log_buf);
        fd_set m;
        FD_ZERO(&m);
        struct timeval tv;
        tv.tv_sec = 120;
        tv.tv_usec = 0;
        FD_SET(restaurant_fd, &m);
        select(restaurant_fd + 1, &m, NULL, NULL, &tv);
        char answer[32];
        if (FD_ISSET(restaurant_fd, &m))
        {
            recv(restaurant_fd, answer, 32, 0);
            char log_buf[64];
            sprintf(log_buf, "\"%s\" recieved on %d tcp port", answer, tcp_port);
            log(user_name, log_buf);
            print(answer);
        }
        else
        {
            time_out();
        }
        memset(massage, 0, 77);
        sprintf(massage, "CLOSE_PORT|%d", tcp_port);
        send(restaurant_fd, massage, strlen(massage), 0);
        close(restaurant_fd);
    }
    else if (!strcmp(command, RESTAURANT_LIST))
    {
        restaurant_list();
    }
    else if (!strcmp(command, MENU))
    {
        red_json();
    }
}
void handle_broadcast()
{
    char temp_buffer[1024];
    memset(temp_buffer, 0, 1024);
    recv(udp_socket, temp_buffer, 1024, 0);
    if (strcmp(temp_buffer, sign_up) == 0)
    {
        sendto(udp_socket, name, strlen(name), 0, (const struct sockaddr *)&udp_addr, sizeof(udp_addr));
        print(NEW_USER);
        print_n(">> ");
    }
    else
    {
        char *first;
        first = strtok(temp_buffer, " ");
        if (!strcmp(first, new_restaurant))
        {
            char *r_name;
            r_name = strtok(NULL, "|");
            print_n(r_name);
            print_n(" restaurant opened\n");
        }
        else if (!strcmp(first, close_restaurant))
        {
            char *r_name;
            r_name = strtok(NULL, "|");
            print_n(r_name);
            print_n(" restaurant closed\n");
            print(START_OF_COMMAND);
        }
    }
}
void run_customer()
{
    fd_set master, working;
    FD_ZERO(&master);
    FD_ZERO(&working);
    FD_SET(udp_socket, &master);
    FD_SET(STD_IN, &master);
    print_n(START_OF_COMMAND);
    while (1)
    {
        working = master;
        select(udp_socket + 1, &working, NULL, NULL, NULL);
        if (FD_ISSET(STD_IN, &working))
        {
            handle_commands();
            print_n(START_OF_COMMAND);
        }
        if (FD_ISSET(udp_socket, &working))
        {
            handle_broadcast();
        }
    }
}
int main(int argc, const char *argv[])
{
    print("please enter TCP port");
    char temp[10];
    get_input(temp);
    tcp_port = get_port(temp);
    udp_socket = setup_port(argc, argv, user_name, tcp_port, &udp_port, &udp_addr);
    run_customer();
}