#include <fstream>
#include <iostream>

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

        for (auto c : line)
        {

            if (c >= '0' && c <= '9')
            {
                if (first == -1)
                {
                    first = c - '0';
                }

                last = c - '0';
            }
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