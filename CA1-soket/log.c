#include "log.h"
void print(const char *massage)
{
    write(STD_OUT, massage, strlen(massage));
    write(STD_OUT, "\n", 1);
}
void print_n(const char *massage)
{
    write(STD_OUT, massage, strlen(massage));
}
void perror(const char *msg)
{
    write(STD_ERR, msg, strlen(msg));
    write(STD_ERR, "\n", 1);
}
void get_input(char * buf)
{
    char temp[1024];
    memset(temp,0,1024);
    int n = read(STD_IN,temp,1024);
    temp[n-1]='\0';
    strcpy(buf,temp);
}
void log(char *name ,char * massage)
{
    int fd =open(name,O_APPEND);
    write(file_fd,massage,strlen(massage));
    close(fd);
}