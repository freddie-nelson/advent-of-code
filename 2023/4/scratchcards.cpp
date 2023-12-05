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

struct ScratchCard
{
    int id;
    std::vector<int> winningNumbers;
    std::vector<int> numbers;
    int winningNumbersFound = 0;
};

std::string getScratchCardsString(std::string filename)
{
    std::ifstream f(filename);
    if (!f.is_open())
        throw std::runtime_error("Could not open file");

    std::stringstream buffer;
    buffer << f.rdbuf();

    return buffer.str();
}

std::vector<ScratchCard> getScratchCards(std::string cards)
{
    std::vector<std::string> lines = split(cards, "\n");

    std::vector<ScratchCard> scratchCards;

    for (auto line : lines)
    {
        std::vector<std::string> temp = split(line, ":");
        int id = std::stoi(split(temp[0], " ").back());

        std::vector<std::string> numberSections = split(temp[1], "|");
        std::string winningNumbersString = numberSections[0];
        std::string numbersString = numberSections[1];

        // std::cout << winningNumbersString << std::endl;
        // std::cout << numbersString << std::endl;

        std::vector<std::string> winningNumbersStringVector = split(winningNumbersString, " ");
        std::vector<int> winningNumbers;

        for (auto number : winningNumbersStringVector)
        {
            if (number == "")
                continue;

            winningNumbers.push_back(std::stoi(number));
        }

        std::vector<std::string> numbersStringVector = split(numbersString, " ");
        std::vector<int> numbers;

        for (auto number : numbersStringVector)
        {
            if (number == "")
                continue;

            numbers.push_back(std::stoi(number));
        }

        scratchCards.push_back({id, winningNumbers, numbers});
    }

    return scratchCards;
}

std::vector<int> getCardPoints(std::vector<ScratchCard> cards)
{
    std::vector<int> points;

    for (auto card : cards)
    {
        int found = 0;

        for (auto number : card.numbers)
        {
            if (std::find(card.winningNumbers.begin(), card.winningNumbers.end(), number) != card.winningNumbers.end())
            {
                found++;
            }
        }

        if (found == 0)
        {
            points.push_back(0);
        }
        else
        {
            points.push_back(std::pow(2, found - 1));
        }
    }

    return points;
}

int getNumWinningNumbers(ScratchCard card)
{
    int found = 0;

    for (auto number : card.numbers)
    {
        if (std::find(card.winningNumbers.begin(), card.winningNumbers.end(), number) != card.winningNumbers.end())
        {
            found++;
        }
    }

    return found;
}

int getNumScratchCards(std::vector<ScratchCard> cards)
{
    // create card instances count map
    std::vector<int> cardInstancesCount;

    for (auto &card : cards)
    {
        cardInstancesCount.push_back(1);
    }

    // find winning numbers for each card
    for (auto &card : cards)
    {
        card.winningNumbersFound = getNumWinningNumbers(card);
    }

    for (int ci = 0; ci < cards.size(); ci++)
    {
        int id = cards[ci].id;
        int count = cardInstancesCount[ci];

        // std::cout << id << " " << count << std::endl;
        // std::cout << "winning numbers found: " << cards[ci].winningNumbersFound << std::endl;

        ScratchCard &card = cards[ci];

        for (int j = ci + 1; j <= ci + card.winningNumbersFound; j++)
        {
            // std::cout << "j: " << cards[j].id << std::endl;
            cardInstancesCount[j] += count;
        }
    }

    int numScratchCards = 0;
    for (auto count : cardInstancesCount)
    {
        numScratchCards += count;
    }

    return numScratchCards;
}

int main()
{
    std::string cardsString = getScratchCardsString("cards.txt");
    auto cards = getScratchCards(cardsString);

    auto points = getCardPoints(cards);
    int totalPoints = std::accumulate(points.begin(), points.end(), 0);

    std::cout << totalPoints << std::endl;

    auto numScratchCards = getNumScratchCards(cards);
    std::cout << numScratchCards << std::endl;

    return 0;
}