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
#define MAX_COMMAND_SIZE 1024

#define WATING 0  
#define ACCEPT 1
#define REJECT -1
const string START = "start working";
const string END = "break";
const string ILIST = "show ingredients";
const string RECIPES = "show recipes";
const string SUPPLY = "show supplier";
const string REQUEST = "request ingredient";
const string RLIST = "show request list";
const string ANS = "answer request";
const string HIST = "show sales history";
const string request_port = "port of request: ";
int port;
int status;
int read_command();
char user_name[32];
int udp_port;
int tcp_port;
int udp_socket;
char owner_name[32];
addr udp_addr;
int status = 0; // it could be 0 or 1
typedef struct
{
    char name[32];
    int quantity;
} Ingrediant;
typedef struct
{
    char name[32];
    Ingrediant ingrediants[20];
    int size;

} Recipe;
Ingrediant ingrediants[50];
int size_ingrediants=0;
Recipe recipes[50];
int recipes_size=0;
void red_json();
typedef struct 
{
    int soket; 
    int port;
    addr adress; 
    char name[32];
    char user_name[32];
    int status;
}Client;
Client clients [30]; //you cant have more than 30 clinet at the same time
int clinets_size=0 ;
