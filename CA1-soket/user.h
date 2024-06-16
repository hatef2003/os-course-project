#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.h"
#include "defines.h"
typedef struct sockaddr_in addr;
int setup_port(int argc, const char *argv[],char * user_name,int tcp,int * udp_port,addr* addres);
int tcp_socket_fd;
addr tcp_address;
int file_fd;