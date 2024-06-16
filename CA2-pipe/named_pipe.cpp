#include "named_pipe.hpp"
NamedPipe::NamedPipe(string _name)
{
    name = _name;
    path = "FIFO/";
    path.append(name);
    if (mkfifo(path.c_str(), PIPE_MODE) == -1 && errno != EEXIST)
    {
        cerr << "pipe doesn't exist\n";
        return;
    }
}
void NamedPipe::close_pipe()
{
    close(fd);
}
ReadPipe::ReadPipe(string _name) : NamedPipe(_name)
{
    fd = open(path.c_str(), O_RDONLY);
    if (fd == -1)
    {
        cerr << "could not open pipe\n";
    }
}
void ReadPipe::reload()
{
    close(fd);
    fd = open(path.c_str(), O_RDONLY);
    if (fd == -1)
    {
        cerr << "could not open pipe";
        return;
    }
}
string ReadPipe::read_pipe()
{
    char buf[2048];
    int n = read(fd, buf, 2048);
    buf[n] = '\0';
    return string(buf);
}
WritePipe::WritePipe(string _name) : NamedPipe(_name)
{
    fd = open(path.c_str(), O_WRONLY);
    if (fd == -1)
    {
        cerr << "could not open pipe";
        return;
    }
}
void WritePipe::reload()
{
    
    close(fd);
    fd = open(path.c_str(), O_WRONLY);
    if (fd == -1)
    {
        cerr << "could not open pipe";
        return;
    }
}
void WritePipe::write_pipe(string massage)
{
    write(fd, massage.c_str(), massage.size());
}