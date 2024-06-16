#ifndef NAMEDPIPE
#define NAMEDPIPE
#define PIPE_MODE 0777
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <cstdio>
#include <iostream>
using namespace std;
class NamedPipe
{
public:
    NamedPipe(string path);
    void close_pipe();

protected:
    string path;
    string name;
    int fd;
};
class ReadPipe : public NamedPipe
{
public:
    void reload();
    ReadPipe(string name);
    string read_pipe();
};

class WritePipe : public NamedPipe
{
public:
    void reload();
    WritePipe(string _name);
    void write_pipe(string massage);
};

#endif