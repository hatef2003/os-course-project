#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common.cpp"
#include "named_pipe.hpp"
using namespace std;
int main(int argc, char *argv[])
{
    string name(argv[1]);
    cout<<"process of "<<name<<" built"<<endl;
    int gas_fd[2], water_fd[2], power_fd[2];
    pipe(gas_fd);
    pipe(water_fd);
    pipe(power_fd);
    int power_pid, water_pid, gas_pid;
    gas_pid = fork();

    if (gas_pid != 0)
    {
        close(gas_fd[1]);
        power_pid = fork();
        if (power_pid != 0)
        {
            close(power_fd[1]);
            water_pid = fork();
            if (water_pid == 0)
            {
                cout << "water process of "<<name<<" built"<<endl;
                close(water_fd[0]);
                close(STDOUT_FILENO);
                dup(water_fd[1]);
                name.append("/Water.csv");
                execl("./raw_data", "./raw_data", name.c_str(), NULL);
            }
            else
            {
                close(water_fd[1]);
            }
        }
        else
        {
            cout << "power process of"<<name<<" built"<<endl;
            close(power_fd[0]);
            close(STDOUT_FILENO);
            dup(power_fd[1]);
            name.append("/Electricity.csv");
            execl("./raw_data", "./raw_data", name.c_str(), NULL);
        }
    }
    else
    {
        cout << "gas process of"<<name<<" built"<<endl;
        close(gas_fd[0]);
        close(STDOUT_FILENO);
        dup(gas_fd[1]);
        name.append("/Gas.csv");
        execl("./raw_data", "./raw_data", name.c_str(), NULL);
    }
    if (power_pid != 0 && water_pid != 0 && gas_pid != 0)
    {
        name = split(name , '/')[1];
        char water_buf[128],gas_buf[128],power_buf[128];
        int n = read(water_fd[0], water_buf, 128);
        water_buf[n] = '\0';
        cout<<"data of water,"<<name<<":\n"<<water_buf<<endl;
        n = read(power_fd[0], gas_buf, 128);
        gas_buf[n] = '\0';
        cout<<"data of gas,"<<name<<":\n"<<gas_buf<<endl;
        n = read(gas_fd[0], power_buf, 128);
        power_buf[n] = '\0';
        cout<<"data of power,"<<name<<":\n"<<power_buf<<endl;
        char send_buf[2048];
        sprintf(send_buf,"%s?%s?%s?%s%c",name.c_str(),water_buf,power_buf,gas_buf,'D');
        WritePipe write_pipe("pipe");
        write_pipe.write_pipe(string(send_buf));
        cout<<name<<": sent to bill app"<<endl;

        ReadPipe rd(name);
        string result =rd.read_pipe();
        cout<<name<<": received from bill app done\n";
        write_pipe.close_pipe();
        rd.close_pipe();
        cerr<<result<<"#"<<water_buf<<"#"<<power_buf<<"#"<<gas_buf;
        
        cout<<name<<": sending to main app done"<<endl;
    }
}