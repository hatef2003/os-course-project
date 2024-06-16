#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include "named_pipe.hpp"
#include "common.cpp"
const int WATER = 0;
const int POWER = 1;
const int GAS = 2;
using namespace std;
struct Child
{
    int read;
    int pid;
    string name;
    bool status;
    string massage;
};

vector<string> read_all_folders_name(string path)
{
    vector<string> result;
    DIR *dr;
    struct dirent *en;
    dr = opendir(path.c_str());
    if (dr)
    {
        while ((en = readdir(dr)) != NULL)
        {
            if (en->d_type == DT_DIR && en->d_name[0] != '.')
            {
                string temp(en->d_name);
                result.push_back(temp);
            }
        }
        closedir(dr);
    }
    return result;
}
bool is_in_vec(int pid, vector<Child> &childs)
{
    for (int i = 0; i < childs.size(); i++)
    {
        if (childs[i].pid == pid)
        {
            return false;
        }
    }
    return true;
}
int calculate_price(string prices, int resource, int month)
{
    vector<string> splitted = split(prices, '|');
    int price = atoi(split(splitted[resource], ' ')[month - 1].c_str());
    return price;
}

vector<int> calculate_bill(const Child &child, int month, int resource)
{
    vector<int>answer(5);
    int price, peak, sum_of_month, sum_of_peak, exclude_sum;
    vector<string> splitted_massage = split(child.massage, '#');
    price = calculate_price(splitted_massage[0], resource, month);
    string result = splitted_massage[resource + 1];
    vector<string> split_result = split(result, '|');
    peak = atoi(split_result[0].c_str());
    sum_of_month=atoi(split(split_result[1],' ')[month-1].c_str());
    exclude_sum = atoi(split(split_result[2],' ')[month-1].c_str());
    sum_of_peak = sum_of_month-exclude_sum;
    answer[0]=price;
    answer[1]=peak;
    answer[2]=sum_of_month/30;
    answer[3]=exclude_sum/30;
    answer[4]=sum_of_peak/30;
    return answer;
}
vector<int> get_bill(vector<Child> &childs, int month,int resource, string name)
{
    for (int i = 0; i < childs.size(); i++)
    {
        if (childs[i].name == name)
        {
            return calculate_bill(childs[i],month,resource);
        }
    }
}
int main(int argc, char *argv[])
{
    int bill_pid;
    string path(argv[1]);

    vector<Child> childs;
    vector<string> builds = read_all_folders_name(path);
    for (int i = 0; i < builds.size(); i++)
    {
        int fd[2];
        pipe(fd);
        int id;
        id = fork();
        if (id == 0)
        {
            path.push_back('/');
            path.append(builds[i]);
            close(fd[0]);
            close(STDERR_FILENO);
            dup(fd[1]);
            execl("./building.out", "./building.out", path.c_str(), NULL);
        }
        else
        {
            close(fd[1]);
            Child temp;
            temp.name = builds[i];
            temp.pid = id;
            temp.status = 1;
            temp.read = fd[0];
            childs.push_back(temp);
        }
    }
    bill_pid = fork();
    if (bill_pid == 0)
    {
        execl("./bill.out", "./bill.out", NULL);
    }

    for (int i = 0; i < childs.size(); i++)
    {
        char temp[1024];
        int n = read(childs[i].read, temp, 1024);
        temp[n] = '\0';
        childs[i].massage = string(temp);
        childs[i].status = 0;
    }
    WritePipe wr("pipe");
    wr.write_pipe("CLOSE");
    wr.close_pipe();
    cout<<"Done"<<endl;
    while(true)
    {
        string command;
        getline(cin , command);
        vector<string>splitted = split(command , ' ');
        if(splitted[0] == "get")
        {
            string name = splitted[1];
            int month = atoi(splitted[2].c_str());
            int resource = (splitted[3]== "water")? WATER:
                            (splitted[3]=="power")? POWER:
                                                    GAS;
            vector<int>ans= get_bill(childs,month,resource,name);
            cout<<"price: "<<ans[0]<<endl <<"peak : "<<ans[1]<<endl<<"sum: "<<ans[2]<<endl<<"sum without peak hours: "
                    << ans[3]<<endl<<"sum in peak hours: "<<ans[4]<<endl;
        }
        else if (command == "close")
        {
            break;
        }
    }
}
