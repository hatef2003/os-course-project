#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>
#include "common.cpp"
#include "named_pipe.hpp"
#define BILL_CSV "buildings/bills.csv"
using namespace std;
const int average_limit = 50;
// sum of all consumes
// peak hours
struct Price
{
    int gas;
    int power;
    int water;
    int month;
};
vector<Price> reading_bill_price()
{
    vector<Price> result;
    fstream fs;
    fs.open(BILL_CSV, ios::in);
    if (!fs)
    {
        cerr << "file error\n";
    }
    else
    {
        string temp;

        getline(fs, temp);
        while (!fs.eof())
        {
            Price p;
            string temp_line;
            getline(fs, temp_line);
            vector<string> elements = split(temp_line, ',');
            p.month = atoi(elements[1].c_str());
            p.water = atoi(elements[2].c_str());
            p.gas = atoi(elements[3].c_str());
            p.power = atoi(elements[4].c_str());
            result.push_back(p);
        }
    }
    return result;
}
vector<int> calculate_water(string water, vector<Price> &prices)
{
    vector<int> result(12);
    vector<string> splitted = split(water, '|');
    int peak = atoi(splitted[0].c_str());
    vector<string> sum_of_month = split(splitted[1], ' ');
    vector<string> sum_of_exclude = split(splitted[2], ' ');
    for (int i = 0; i < 12; i++)
    {
        int a = atoi(sum_of_exclude[i].c_str());
        int b = (atoi(sum_of_month[i].c_str()));
        int price = (a + (((b - a) * 5 / 4))) * prices[i].water;
        result[i] = price;
    }
    return result;
}
vector<int> calculate_power(string power, vector<Price> &prices)
{
    vector<int> result(12);
    vector<string> splitted = split(power, '|');
    int peak = atoi(splitted[0].c_str());
    vector<string> sum_of_month = split(splitted[1], ' ');
    vector<string> sum_of_exclude = split(splitted[2], ' ');
    for (int i = 0; i < 12; i++)
    {
        int a = atoi(sum_of_exclude[i].c_str());
        int b = (atoi(sum_of_month[i].c_str()));
        int price = (a + (((b - a) * 5 / 4))) * prices[i].power;
        if (b / 30 < average_limit)
        {
            price *= 3;
            price /= 4;
        }
        result[i] = price;
    }
    return result;
}
vector<int> calculate_gas(string gas, vector<Price> &prices)
{
    vector<int> result(12);
    vector<string> splitted = split(gas, '|');
    int peak = atoi(splitted[0].c_str());
    vector<string> sum_of_month = split(splitted[1], ' ');
    vector<string> sum_of_exclude = split(splitted[2], ' ');
    for (int i = 0; i < 12; i++)
    {
        int b = (atoi(sum_of_month[i].c_str()));
        int price = b * prices[i].gas;
        result[i] = price;
    }
    return result;
}
string join_vector(vector<int> vec)
{

    string res = "";
    for (int i = 0; i < vec.size(); i++)
    {
        res.append(to_string(vec[i]));
        if (i != vec.size() - 1)
            res.append(" ");
    }
    return res;
}
void calculate_price(string massage, vector<Price> &prices)
{

    vector<string> parts = split(massage, '?');
    cout<<"calculating of:"<<parts[0]<<endl;
    vector<int> water_price = calculate_water(parts[1], prices);
    string water = join_vector(water_price);
    vector<int> power_price = calculate_water(parts[2], prices);
    string power = join_vector(power_price);
    vector<int> gas_price = calculate_water(parts[3], prices);
    string gas = join_vector(gas_price);
    char send_buf[1024];
    sprintf(send_buf, "%s|%s|%s", water.c_str(), power.c_str(), gas.c_str());
    WritePipe write_pipe(parts[0]);
    write_pipe.write_pipe(string(send_buf));
    write_pipe.close_pipe();
}
int main(int argc, char *argv[])
{

    vector<Price> prices = reading_bill_price();
    ReadPipe read_pipe("pipe");
    while (true)
    {
        string massage = read_pipe.read_pipe();
        // if (massage != "")
        // {
        //     cout << "reading done" << endl;
        //     cout << massage << endl;
        // }
        if (massage == "")
        {
            // read_pipe.reload();
        }
        else if (massage == "CLOSE")
        {
            read_pipe.close_pipe();
            break;
        }
        else
        {
            vector<string> massages = split(massage, 'D');
            for (int i = 0; i < massages.size()-1; i++)
            {
                calculate_price(massages[i], prices);
            }
        }
    }
}