#ifndef LOG
#define LOG

#define STD_IN 0
#define STD_OUT 1
#define STD_ERR 2
#include <string.h>
#include <unistd.h>
void print(const char * massage);
void perror(const char * msg);
void get_input(char * buf);




#endif