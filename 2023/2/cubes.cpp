#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

struct Move
{
    unsigned int redCount = 0;
    unsigned int greenCount = 0;
    unsigned int blueCount = 0;
};

// for string delimiter
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

void printGame(std::pair<int, std::vector<Move>> game)
{
    std::cout << "Game " << game.first << ": ";

    for (auto move : game.second)
    {
        std::string s = "";

        if (move.greenCount > 0)
        {
            s += std::to_string(move.greenCount) + " green, ";
        }

        if (move.blueCount > 0)
        {
            s += std::to_string(move.blueCount) + " blue, ";
        }

        if (move.redCount > 0)
        {
            s += std::to_string(move.redCount) + " red, ";
        }

        s = s.substr(0, s.size() - 2) + "; ";
        std::cout << s;
    }

    std::cout << std::endl;
}

std::pair<int, std::vector<Move>> parseGame(std::string s)
{
    auto temp = split(s, ": ");

    auto gameId = split(temp[0], " ")[1];

    auto movesString = temp[1];
    auto movesAsString = split(movesString, "; ");

    std::vector<Move> moves;

    for (auto move : movesAsString)
    {
        auto moveAsString = split(move, ", ");
        Move m;

        for (int i = 0; i < moveAsString.size(); i++)
        {
            auto s = moveAsString[i];
            auto cube = split(s, " ");
            auto count = std::stoi(cube[0]);
            auto color = cube[1];

            if (color == "red")
            {
                m.redCount = count;
            }
            else if (color == "green")
            {
                m.greenCount = count;
            }
            else if (color == "blue")
            {
                m.blueCount = count;
            }
        }

        // std::cout << "red: " << m.redCount << std::endl;
        // std::cout << "green: " << m.greenCount << std::endl;
        // std::cout << "blue: " << m.blueCount << std::endl;

        moves.push_back(m);
    }

    return {std::stoi(gameId), moves};
}

std::unordered_map<int, std::vector<Move>> parseGames(std::string file)
{
    std::ifstream f(file);
    std::unordered_map<int, std::vector<Move>> games;

    std::string line;
    while (std::getline(f, line))
    {
        auto game = parseGame(line);
        games[game.first] = game.second;
    }

    return games;
}

Move getMinCubesRequiredForGame(std::vector<Move> moves)
{
    Move maxMove;

    for (auto move : moves)
    {
        if (move.redCount > maxMove.redCount)
        {
            maxMove.redCount = move.redCount;
        }

        if (move.greenCount > maxMove.greenCount)
        {
            maxMove.greenCount = move.greenCount;
        }

        if (move.blueCount > maxMove.blueCount)
        {
            maxMove.blueCount = move.blueCount;
        }
    }

    return maxMove;
}

int main()
{
    auto games = parseGames("games.txt");

    int maxRed = 12;
    int maxGreen = 13;
    int maxBlue = 14;

    int validGamesSum = 0;

    for (auto game : games)
    {
        // printGame(game);
        bool isValid = true;

        for (auto move : game.second)
        {
            if (move.redCount > maxRed || move.greenCount > maxGreen || move.blueCount > maxBlue)
            {
                isValid = false;
                break;
            }
        }

        if (isValid)
        {
            validGamesSum += game.first;
        }
    }

    std::cout << validGamesSum << std::endl;

    int powerSum = 0;

    for (auto game : games)
    {
        auto minRequiredCubes = getMinCubesRequiredForGame(game.second);
        int power = minRequiredCubes.redCount * minRequiredCubes.greenCount * minRequiredCubes.blueCount;
        powerSum += power;
    }

    std::cout << powerSum << std::endl;

    return 0;
}