#ifndef COMMON
#define COMMON
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;
class Consumer
{
public:
    Consumer(int year, int month, int day, vector<int> consumes);
    int year;
    int month;
    int day;
    vector<int> consumes;
};
Consumer::Consumer(int _year, int _month, int _day, vector<int> _consumes)
{
    year = _year;
    month = _month;
    day = _day;
    consumes = _consumes;
}

vector<string> split(string str, char delim)
{
    vector<string> result;
    string temp;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == delim)
        {

            result.push_back(temp);
            temp.clear();
            while (str[i] == delim)
            {
                i++;
            }
            i--;
        }
        else
        {
            temp.push_back(str[i]);
        }
    }
    result.push_back(temp);
    return result;
}
void append_new_consumer(vector<Consumer> &list, vector<string> elements)
{
    int year = atoi(elements[0].c_str());
    int month = atoi(elements[1].c_str());
    int day = atoi(elements[2].c_str());
    vector<int> consumes;
    for (int i = 3; i < elements.size(); i++)
    {
        consumes.push_back(atoi(elements[i].c_str()));
    }
    list.push_back(Consumer(year, month, day, consumes));
}
vector<Consumer> read_csv(string path)
{
    vector<Consumer> result;
    fstream file;
    file.open(path.c_str(), ios::in);
    if (!file)
    {
        cerr << "file error\n";
    }
    else
    {
        string temp;
        getline(file, temp);
        while (!file.eof())
        {
            string temp_line;
            getline(file, temp_line);
            vector<string> splitted_line = split(temp_line, ',');
            append_new_consumer(result, splitted_line);
        }
    }
    file.close();
    return result;
}
int max_index(vector<int> &vec)
{
    int max = -1;
    int index = -1;
    for (int i = 0; i < vec.size(); i++)
    {
        if (vec[i] > max)
        {
            max = vec[i];
            index = i;
        }
    }
    return index;
}

int calculate_peak_hour(vector<Consumer> &consumers)
{
    vector<int> sums(consumers[0].consumes.size(), 0);
    for (int i = 0; i < consumers.size(); i++)
    {
        for (int j = 0; j < consumers[i].consumes.size(); j++)
        {
            sums[j] += consumers[i].consumes[j];
        }
    }
    int peak_hour = max_index(sums);
    return peak_hour;
}
int sum_of_consumes(Consumer c)
{
    int sum = 0;
    for (int i = 0; i < c.consumes.size(); i++)
    {
        sum += c.consumes[i];
    }
    return sum;
}

int calculate_sum_of_month(vector<Consumer> &consumers, int month)
{
    int sum = 0;
    for (auto a : consumers)
    {
        if (a.month == month)
        {
            sum += sum_of_consumes(a);
        }
    }
    return sum;
}
vector<int> calculate_sum_of_months(vector<Consumer> &consumers)
{
    vector<int> result(12);
    for (int i = 1; i <= 12; i++)
    {
        result[i - 1] = calculate_sum_of_month(consumers, i);
    }
    return result;
}
vector<int> calculate_exclude_peak_consumption(int peak_hour, vector<Consumer> &consumers)
{
    vector<int> result(12);
    for (int i = 1; i <= 12; i++)
    {
        int sum = 0;
        for (int j = 0; j < consumers.size(); j++)
        {
            if (consumers[j].month == i)
            {
                int temp = sum_of_consumes(consumers[j]);
                temp -= consumers[j].consumes[peak_hour];
                sum += temp;
            }
        }
        result[i - 1] = sum;
    }
    return result;
}
#endif