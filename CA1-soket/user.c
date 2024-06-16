#include "user.h"
struct sockaddr_in send_addres, recieve_addr;
char name[32];
char buf[1024];
char names_array[12][32];
int get_port(const char *portStr)
{
    char *endPtr;
    long port = strtol(portStr, &endPtr, 10);
    if (port < 1024 || port > 65535 || *endPtr != '\0')
    {
        printf("port_making failed\n");
        return -1;
    }
    return (int)port;
}
void get_name()
{
    memset(name, 0, 32);

    int n = read(STDIN_FILENO, name, 32);
    name[n - 1] = '\0';
}
short is_valid_name(int counter)
{
    for (int i = 0; i < counter; i++)
    {
        if (strcmp(name, names_array[i]) == 0)
        {
            return 0;
        }
    }
    return 1;
}
int make_socket()
{
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    int broadcast = 1, opt = 1;
    if (setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
    {
        perror("binding failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(udp_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        perror("binding failed");
        exit(EXIT_FAILURE);
    }
    return udp_socket;
}
void make_log_file()
{
    file_fd = creat(name,O_WRONLY);
}
int setup_port(int argc, const char *argv[], char *user_name, int tcp, int *udp_port , addr * addres)
{
    if (argc != 2)
    {
        perror("NO PORT");
        exit(EXIT_FAILURE);
    }
    int udp_socket = make_socket();
    int port = get_port(argv[1]);
    if (udp_socket < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&send_addres, 0, sizeof(send_addres));
    send_addres.sin_family = AF_INET; // IPv4

    send_addres.sin_addr.s_addr = inet_addr("10.0.2.255");
    send_addres.sin_port = htons(port);

    if (bind(udp_socket, (const struct sockaddr *)&send_addres, sizeof(send_addres)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    sendto(udp_socket, sign_up, 5, 0, (const struct sockaddr *)&send_addres, sizeof(send_addres));
    write(STDOUT_FILENO, "Enter your name: ", 17);
    fd_set select_fd, working;
    FD_ZERO(&working);
    FD_ZERO(&select_fd);
    FD_SET(STD_IN, &select_fd);
    FD_SET(udp_socket, &select_fd);
    int max = -1;
    if (udp_socket > max)
    {
        max = udp_socket;
    }

    memset(names_array, 0, 32 * 12);
    int counter = 0;
    while (1 == 1)
    {

        working = select_fd;
        select(max + 1, &working, NULL, NULL, NULL);
        if (FD_ISSET(udp_socket, &working))
        {

            memset(buf, 0, 1024);
            recv(udp_socket, buf, 1024, 0);
            if (strcmp(buf, sign_up))
            {
                strcpy(names_array[counter], buf);
                counter++;
            }
        }
        if (FD_ISSET(STD_IN, &working))
        {
            get_name();
            break;
        }
    }
    if (!is_valid_name(counter))
    {
        perror("your name was used please re run app");
        exit(EXIT_FAILURE);
    }
    char send_buf[96];
    sprintf(send_buf, "new %s %d", name, tcp);
    sendto(udp_socket, (const char *)name, strlen(name),
           0, (const struct sockaddr *)&send_addres, sizeof(send_addres));
    strcpy(user_name, name);
    *udp_port = port;
    *addres = send_addres;
    make_log_file();
    return udp_socket;
}
int make_tcp_socket(int port,addr * address)
{
    int tcp_socket= socket(AF_INET, SOCK_STREAM, 0);
        int opt = 1;
    setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);
    bind(tcp_socket, (struct sockaddr *)address, sizeof(*address));
    return tcp_socket;
    
}