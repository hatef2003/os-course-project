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
#include "user.c"
#include "log.c"
#include "defines.h"


const string FOOD_NAME= "name of food: ";
const string PORT = "port of restaurant: ";
const string RESTAURANT_LIST= "show restaurants";
const string ORDER = "order food";
const string MENU = "show menu";

char user_name[32];
int udp_port;
int tcp_port;
int udp_socket;

char owner_name[32];
addr udp_addr;