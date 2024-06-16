
int tcp_port;
int udp_port;
int udp_socket;
char user_name[32];

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include"defines.h"
const string ANSWER_REQUEST="answer request";
struct sockaddr_in udp_addr;
typedef struct 
{
    int port ;
    int number;
    char ingredient_name[32];
    int socket;
}Rclinet;
Rclinet rclinets[50];
int r_size=0;