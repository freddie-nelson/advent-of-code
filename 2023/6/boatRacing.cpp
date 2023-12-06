#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <math.h>
#include <numeric>
#include <map>

std::string trim(const std::string &str, char whitespace = ' ')
{
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first)
    {
        return str;
    }

    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> split(std::string s, std::string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

std::string getFileAsString(std::string fileName)
{
    std::ifstream file(fileName);
    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

struct Race
{
    long long length;
    long long record;
};

inline int getBoatSpeed(int holdLength)
{
    return holdLength;
}

std::vector<Race> getRaces(std::string input)
{
    std::vector<std::string> lines = split(input, "\n");
    std::string timesLine = lines[0];
    std::string recordsLine = lines[1];

    std::vector<std::string> times = split(trim(split(timesLine, ":").back()), " ");
    std::vector<std::string> records = split(trim(split(recordsLine, ":").back()), " ");

    // remove empty strings
    for (int i = times.size(); i >= 0; i--)
    {
        if (times[i] == "")
        {
            times.erase(times.begin() + i);
        }
    }

    for (int i = records.size(); i >= 0; i--)
    {
        if (records[i] == "")
        {
            records.erase(records.begin() + i);
        }
    }

    // PART 1
    // std::vector<Race> races;
    // for (int i = 0; i < times.size(); i++)
    // {
    //     races.push_back({std::stoi(times[i]),
    //                      std::stoi(records[i])});
    // }

    // PART 2
    std::string time = "";
    for (auto &t : times)
    {
        time += t;
    }

    std::string record = "";
    for (auto &r : records)
    {
        record += r;
    }

    std::vector<Race> races{{std::stoll(time),
                             std::stoll(record)}};

    return races;
}

std::vector<long long> getWinningGameAmounts(std::vector<Race> races)
{
    std::vector<long long> winningGameAmounts;

    for (auto &r : races)
    {
        int winningGames = 0;

        for (long long l = 1; l <= r.length; l++)
        {
            long long boatSpeed = getBoatSpeed(l);

            long long timeLeft = r.length - l;
            long long distanceTravelled = boatSpeed * timeLeft;

            if (distanceTravelled > r.record)
            {
                winningGames++;
            }
        }

        winningGameAmounts.push_back(winningGames);
    }

    return winningGameAmounts;
}

int getWinningMargin(std::vector<Race> races)
{
    auto winningGamesCounts = getWinningGameAmounts(races);
    if (winningGamesCounts.size() == 0)
        return 0;

    int winningMargin = winningGamesCounts[0];
    for (int i = 1; i < winningGamesCounts.size(); i++)
    {
        winningMargin *= winningGamesCounts[i];
    }

    return winningMargin;
}

int main()
{
    std::string input = getFileAsString("races.txt");

    std::vector<Race> races = getRaces(input);
    for (auto &r : races)
    {
        std::cout << r.length << " " << r.record << std::endl;
    }

    // std::vector<int> winningGamesCounts = getWinningGameAmounts(races);

    long long winningMargin = getWinningMargin(races);
    std::cout << "winning margin: " << winningMargin << std::endl;

    return 0;
}