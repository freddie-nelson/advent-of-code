#include <fstream>
#include <iostream>

int getNumberAsChar(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }

    return -1;
}

int getNumberAsWord(std::string s, int i)
{
    std::string nums[] = {
        "zero",
        "one",
        "two",
        "three",
        "four",
        "five",
        "six",
        "seven",
        "eight",
        "nine",
    };

    for (int j = 0; j < 10; j++)
    {
        if (s.substr(i, nums[j].length()) == nums[j])
        {
            return j;
        }
    }

    return -1;
}

int main()
{
    std::ifstream file("calibration.txt");

    if (!file.is_open())
    {
        std::cout << "File not found!" << std::endl;
        return 1;
    }

    std::string line;

    int total = 0;

    while (std::getline(file, line))
    {
        int first = -1;
        int last = -1;

        // std::cout << line << std::endl;
        for (size_t i = 0; i < line.length(); i++)
        {
            const char c = line[i];

            // parse number as char
            int num = getNumberAsChar(c);

            // try to get number as word
            if (num == -1)
            {
                num = getNumberAsWord(line, i);
            }

            // if number found update first and last
            if (num != -1)
            {
                if (first == -1)
                {
                    first = num;
                }

                last = num;
            }

            // std::cout << c << " " << num << std::endl;
            // std::cout << first << " " << last << std::endl;
        }

        if (first == -1 || last == -1)
        {
            std::cout << "No numbers found!" << std::endl;
            return 1;
        }

        total += (first * 10) + last;
    }

    std::cout << "Total: " << total << std::endl;

    return 0;
}