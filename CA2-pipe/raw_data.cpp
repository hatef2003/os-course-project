#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.cpp"
using namespace std;

int main(int argc, char *argv[])
{
    string path = "", name(argv[1]);
    path.append(name);
    auto consumption = read_csv(path);
    int peak_hour = calculate_peak_hour(consumption);
    vector<int> sums_of_months = calculate_sum_of_months(consumption);
    vector<int> exclude_sum = calculate_exclude_peak_consumption(peak_hour, consumption);
    cout<<peak_hour<<'|';
    for (int i = 0 ; i < 12 ; i++)
    {
        cout<<sums_of_months[i]<<' ';
    }
    cout<<'|';
    for (int i = 0 ; i < 12 ; i++)
    {
        cout<<exclude_sum[i]<<' ';
    }

}