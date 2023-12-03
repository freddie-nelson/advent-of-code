#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <unordered_set>

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

std::string getEngineString(std::string filename)
{
    std::ifstream f(filename);
    std::stringstream buffer;
    buffer << f.rdbuf();

    return buffer.str();
}

std::pair<int, int> getEngineSize(std::string engine)
{
    std::vector<std::string> lines = split(engine, "\n");
    int width = lines[0].size();
    int height = lines.size();

    return {width, height};
}

char **getEngine(std::string engine, std::pair<int, int> size)
{
    char **engineArray = new char *[size.second];

    for (int i = 0; i < size.second; i++)
    {
        engineArray[i] = new char[size.first];
    }

    std::vector<std::string> lines = split(engine, "\n");

    for (int i = 0; i < size.second; i++)
    {
        for (int j = 0; j < size.first; j++)
        {
            engineArray[i][j] = lines[i][j];
        }
    }

    return engineArray;
}

struct EngineChar
{
    char symbol;
    int x;
    int y;
};

std::vector<EngineChar> getNeighbours(char **engine, std::pair<int, int> size, int x, int y)
{
    std::vector<EngineChar> neighbours;

    // left
    if (x > 0)
    {
        neighbours.push_back({engine[y][x - 1], x - 1, y});
    }

    // right
    if (x < size.first - 1)
    {
        neighbours.push_back({engine[y][x + 1], x + 1, y});
    }

    // top
    if (y > 0)
    {
        neighbours.push_back({engine[y - 1][x], x, y - 1});
    }

    // bottom
    if (y < size.second - 1)
    {
        neighbours.push_back({engine[y + 1][x], x, y + 1});
    }

    // top left
    if (x > 0 && y > 0)
    {
        neighbours.push_back({engine[y - 1][x - 1], x - 1, y - 1});
    }

    // bottom right
    if (x < size.first - 1 && y < size.second - 1)
    {
        neighbours.push_back({engine[y + 1][x + 1], x + 1, y + 1});
    }

    // bottom left
    if (x > 0 && y < size.second - 1)
    {
        neighbours.push_back({engine[y + 1][x - 1], x - 1, y + 1});
    }

    // top right
    if (x < size.first - 1 && y > 0)
    {
        neighbours.push_back({engine[y - 1][x + 1], x + 1, y - 1});
    }

    return neighbours;
}

std::vector<EngineChar> getSymbols(char **engine, std::pair<int, int> size)
{
    std::vector<EngineChar> symbols;

    for (int i = 0; i < size.second; i++)
    {
        for (int j = 0; j < size.first; j++)
        {
            // get all symbols that are not numbers or dots
            if (engine[i][j] != '.' && (engine[i][j] < '0' || engine[i][j] > '9'))
            {
                symbols.push_back({engine[i][j], j, i});
            }
        }
    }

    return symbols;
}

struct PartNumber
{
    int number;
    int x;
    int y;

    EngineChar adjacentSymbol;
};

PartNumber getNumber(char **engine, std::pair<int, int> size, int x, int y, EngineChar adjacentSymbol)
{
    char initial = engine[y][x];
    if (initial < '0' || initial > '9')
    {
        throw std::invalid_argument("initial is not a number");
    }

    int startX = x;
    std::string number(1, initial);

    // search left
    // std::cout << "left" << std::endl;
    for (int i = x - 1; i >= 0; i--)
    {
        // std::cout << "i = " << i << ": " << engine[y][i] << std::endl;
        if (engine[y][i] >= '0' && engine[y][i] <= '9')
        {
            startX = i;
            number = engine[y][i] + number;
        }
        else
        {
            break;
        }
    }

    // search right
    // std::cout << "right" << std::endl;
    for (int i = x + 1; i < size.first; i++)
    {
        // std::cout << "i = " << i << ": " << engine[y][i] << std::endl;
        if (engine[y][i] >= '0' && engine[y][i] <= '9')
        {
            number += engine[y][i];
        }
        else
        {
            break;
        }
    }

    // std::cout << "number: " << number << std::endl;

    return {std::stoi(number), startX, y, adjacentSymbol};
}

std::vector<PartNumber> getPartNumbers(char **engine, std::pair<int, int> size, std::vector<EngineChar> symbols)
{

    std::vector<PartNumber> partNumbers;

    for (auto symbol : symbols)
    {
        // std::cout << "symbol: " << symbol.symbol << std::endl;
        auto neighbours = getNeighbours(engine, size, symbol.x, symbol.y);

        for (auto neighbour : neighbours)
        {
            if (neighbour.symbol >= '0' && neighbour.symbol <= '9')
            {
                // std::cout << "neighbour: " << neighbour.symbol << std::endl;
                auto number = getNumber(engine, size, neighbour.x, neighbour.y, symbol);
                partNumbers.push_back(number);
            }
        }
    }

    // remove part numbers that were counted twice
    std::vector<PartNumber> partNumbersUnique;

    for (auto partNumber : partNumbers)
    {
        bool found = false;

        for (auto partNumberUnique : partNumbersUnique)
        {
            if (partNumber.number == partNumberUnique.number && partNumber.x == partNumberUnique.x && partNumber.y == partNumberUnique.y)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            partNumbersUnique.push_back(partNumber);
        }
    }

    return partNumbersUnique;
}

struct Gear
{
    EngineChar gear;
    PartNumber part1;
    PartNumber part2;
};

std::vector<Gear> getGears(std::vector<EngineChar> symbols, std::vector<PartNumber> partNumbers)
{
    std::vector<EngineChar> potentialGears;
    for (auto symbol : symbols)
    {
        if (symbol.symbol == '*')
        {
            potentialGears.push_back(symbol);
        }
    }

    std::vector<Gear> gears;
    for (auto potentialGear : potentialGears)
    {
        std::vector<PartNumber> gearPartNumbers;

        for (auto partNumber : partNumbers)
        {
            if (partNumber.adjacentSymbol.symbol == potentialGear.symbol && partNumber.adjacentSymbol.x == potentialGear.x && partNumber.adjacentSymbol.y == potentialGear.y)
            {
                gearPartNumbers.push_back(partNumber);
            }
        }

        if (gearPartNumbers.size() == 2)
        {
            gears.push_back({potentialGear, gearPartNumbers[0], gearPartNumbers[1]});
        }
    }

    return gears;
}

int main()
{
    std::string engine = getEngineString("engine.txt");
    std::pair<int, int> engineSize = getEngineSize(engine);

    char **engineArray = getEngine(engine, engineSize);
    auto symbols = getSymbols(engineArray, engineSize);

    auto partNumbers = getPartNumbers(engineArray, engineSize, symbols);

    int partNumberSum = 0;
    for (auto partNumber : partNumbers)
    {
        partNumberSum += partNumber.number;
    }

    std::cout << "Part number sum: " << partNumberSum << std::endl;

    auto gears = getGears(symbols, partNumbers);

    int gearRatioSum = 0;
    for (auto gear : gears)
    {
        gearRatioSum += gear.part1.number * gear.part2.number;
    }

    std::cout << "Gear ratio sum: " << gearRatioSum << std::endl;

    return 0;
}