#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <math.h>
#include <numeric>
#include <unordered_map>

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
    if (!file.is_open())
    {
        throw std::invalid_argument("Invalid file name");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

std::vector<std::string> splitByNewLine(std::string s)
{
    return split(s, "\n");
}

int getCardValue(char card)
{
    static const std::unordered_map<char, int> cardValues = {
        {'2', 1},
        {'3', 2},
        {'4', 3},
        {'5', 4},
        {'6', 5},
        {'7', 6},
        {'8', 7},
        {'9', 8},
        {'T', 9},
        {'J', 10},
        {'Q', 11},
        {'K', 12},
        {'A', 13},
    };

    if (cardValues.count(card) == 0)
    {
        throw std::invalid_argument("Invalid card");
    }

    return cardValues.at(card);
}

enum HAND_TYPE
{
    HIGH_CARD = 0,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    FIVE_OF_A_KIND,
};

struct Hand
{
    std::vector<char> cards;
    HAND_TYPE type;
    int bid;
};

HAND_TYPE getHandType(std::vector<char> cards)
{
    std::unordered_map<char, int> cardCounts;

    int maxCount = 0;
    int numOfCards = 0;

    for (char card : cards)
    {
        if (cardCounts.count(card) == 0)
        {
            cardCounts[card] = 0;
            numOfCards++;
        }

        cardCounts[card]++;

        if (cardCounts[card] > maxCount)
        {
            maxCount = cardCounts[card];
        }
    }

    switch (maxCount)
    {
    case 5:
        return FIVE_OF_A_KIND;

    case 4:
        return FOUR_OF_A_KIND;

    case 3:
        if (numOfCards == 2)
        {
            // there must be a three of a kind and a pair since only 2 cards make up 5 cards
            return FULL_HOUSE;
        }

        return THREE_OF_A_KIND;

    case 2:
        // there must 2 pairs, pair + pair + 1 card
        if (cardCounts.size() == 3)
        {
            return TWO_PAIR;
        }
        // pair + 3 cards
        else
        {
            return ONE_PAIR;
        }

    default:
        return HIGH_CARD;
    }
}

/**
 * True if h1 is better than h2
 */
bool compareHands(const Hand &h1, const Hand &h2)
{
    if (h1.type != h2.type)
    {
        return h1.type > h2.type;
    }

    for (int i = 0; i < h1.cards.size(); i++)
    {
        if (h1.cards[i] != h2.cards[i])
        {
            return getCardValue(h1.cards[i]) > getCardValue(h2.cards[i]);
        }
    }

    return false;
}

std::vector<Hand> getHands(const std::vector<std::string> &lines)
{
    std::vector<Hand> hands;

    for (auto &l : lines)
    {
        std::vector<std::string> splitLine = split(l, " ");
        std::string cardsString = splitLine[0];
        std::string bidString = splitLine[1];

        std::vector<char> cards;
        for (auto &c : cardsString)
        {
            cards.push_back(c);
        }

        hands.push_back({cards, getHandType(cards), std::stoi(bidString)});
    }

    return hands;
}

std::vector<Hand> getRankedHands(std::vector<Hand> hands)
{
    std::sort(hands.begin(), hands.end(), compareHands);

    return hands;
}

long long getTotalWinnings(std::vector<Hand> rankedHands)
{
    long long totalWinnings = 0;

    for (int i = 0; i < rankedHands.size(); i++)
    {
        totalWinnings += rankedHands[i].bid * (rankedHands.size() - i);
        // std::cout << rankedHands.size() - i << std::endl;
    }

    return totalWinnings;
}

int main()
{
    auto input = splitByNewLine(getFileAsString("hands.txt"));

    auto hands = getHands(input);
    // for (auto &h : hands)
    // {
    //     std::cout << h.type << " " << h.bid << std::endl;
    // }

    auto rankedHands = getRankedHands(hands);
    for (auto &h : rankedHands)
    {

        std::string cards = "";
        for (auto &c : h.cards)
        {
            cards += c;
        }

        std::cout << cards << " " << h.bid << std::endl;
    }

    auto totalWinnings = getTotalWinnings(rankedHands);

    std::cout << "Total winnings: " << totalWinnings << "\n";

    return 0;
}