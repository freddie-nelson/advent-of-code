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

std::vector<long long> getSeeds(std::string filename)
{

    std::ifstream f(filename);
    if (!f.is_open())
        throw std::runtime_error("Could not open file");

    std::stringstream buffer;
    buffer << f.rdbuf();

    std::string s = buffer.str();

    std::string seedsLine = split(s, "\n").front();
    std::vector<std::string> seedsString = split(split(seedsLine, ": ").back(), " ");

    std::vector<long long> seeds;
    for (auto seed : seedsString)
    {
        seeds.push_back(std::stoll(seed));
    }

    return seeds;
}

std::unordered_map<std::string, std::string> getAllMapStrings(std::string filename)
{
    std::ifstream f(filename);
    if (!f.is_open())
        throw std::runtime_error("Could not open file");

    std::stringstream buffer;
    buffer << f.rdbuf();

    std::string s = buffer.str();
    std::vector<std::string> lines = split(s, "\n");

    // to force last map to be added
    lines.push_back("");

    std::unordered_map<std::string, std::string> mapStrings;

    bool inMap = false;
    std::string mapName = "";
    std::string mapString = "";

    for (auto line : lines)
    {
        if (line == "" && inMap)
        {
            inMap = false;
            mapStrings[mapName] = mapString;

            mapName = "";
            mapString = "";
            continue;
        }

        if (line.find("map:") != std::string::npos)
        {
            mapName = split(line, " ").front();
            inMap = true;
            continue;
        }

        if (inMap)
        {
            mapString += line + "\n";
        }
    }

    return mapStrings;
}

struct Rule
{
    long long destinationRangeStart;
    long long sourceRangeStart;
    long long length;
};

typedef std::vector<Rule> Rules;

std::unordered_map<std::string, Rules> createForwardsMaps(std::unordered_map<std::string, std::string> mapStrings)
{
    std::unordered_map<std::string, Rules> maps;

    for (auto &[mapName, mapString] : mapStrings)
    {
        auto rules = Rules();
        auto lines = split(mapString, "\n");

        for (auto line : lines)
        {
            if (line == "")
                continue;

            auto numbers = split(line, " ");
            // std::cout << numbers[0] << ", " << numbers[1] << ", " << numbers[2] << std::endl;
            // std::cout << std::stoi(numbers[0]) << ", " << std::stoi(numbers[1]) << ", " << std::stoi(numbers[2]) << std::endl;

            auto destinationRangeStart = std::stoll(numbers[0]);
            auto sourceRangeStart = std::stoll(numbers[1]);
            auto length = std::stoll(numbers[2]);

            rules.push_back({destinationRangeStart, sourceRangeStart, length});
        }

        maps[mapName] = rules;
    }

    return maps;
}

std::unordered_map<std::string, Rules> createMaps(std::unordered_map<std::string, std::string> mapStrings, bool backwards = false)
{
    auto forwardsMaps = createForwardsMaps(mapStrings);
    return forwardsMaps;
}

std::vector<std::string> getAvailableDestinations(std::string source, const std::unordered_map<std::string, Rules> &maps)
{
    std::vector<std::string> destinations;

    for (auto &[mapName, map] : maps)
    {
        if (mapName.find(source) == 0)
        {
            destinations.push_back(split(mapName, "-").back());
        }
    }

    return destinations;
}

long long convertValue(Rules rules, long long sourceValue)
{
    for (auto rule : rules)
    {
        if (sourceValue >= rule.sourceRangeStart && sourceValue < rule.sourceRangeStart + rule.length)
        {
            return rule.destinationRangeStart + (sourceValue - rule.sourceRangeStart);
        }
    }

    return sourceValue;
}

long long convert(std::string source, std::string destination, long long sourceValue, const std::unordered_map<std::string, Rules> &maps)
{
    auto destinations = getAvailableDestinations(source, maps);
    auto it = std::find(destinations.begin(), destinations.end(), destination);

    // found destination
    if (it != destinations.end())
    {
        auto map = maps.at(source + "-to-" + destination);
        return convertValue(map, sourceValue);
    }

    for (auto d : destinations)
    {
        auto map = maps.at(source + "-to-" + d);
        auto value = convertValue(map, sourceValue);

        long long destinationValue = convert(d, destination, value, maps);
        if (destinationValue != -1)
            return destinationValue;
    }

    return -1;
}

std::vector<std::pair<long long, long long>> convertSeedsToPart2Format(std::vector<long long> seeds)
{
    std::vector<std::pair<long long, long long>> pairs;

    for (int i = 0; i < seeds.size(); i += 2)
    {
        auto startSeed = seeds[i];
        auto length = seeds[i + 1];

        auto endSeed = startSeed + length - 1;

        pairs.push_back({startSeed, endSeed});
    }

    return pairs;
}

int main()
{
    std::string filename = "almanac.txt";
    auto seeds = getSeeds(filename);
    auto seedPairs = convertSeedsToPart2Format(seeds);

    std::cout << "number of seed pairs: " << seeds.size() << std::endl;

    // for (auto seed : seeds)
    // {
    //     std::cout << seed << std::endl;
    // }

    auto mapStrings = getAllMapStrings(filename);
    // for (auto map : mapStrings)
    // {
    //     std::cout << map.first << std::endl;
    //     std::cout << map.second << std::endl;
    // }

    auto maps = createMaps(mapStrings);
    // for (auto map : maps)
    // {
    //     std::cout << map.first << std::endl;
    //     for (auto &[source, destination] : map.second)
    //     {
    //         std::cout << source << " -> " << destination << std::endl;
    //     }
    // }

    // long long minLocation = LONG_LONG_MAX;
    // for (auto seed : seeds)
    // {
    //     auto location = convert("seed", "location", seed, maps);
    //     if (location < minLocation)
    //         minLocation = location;
    // }

    // std::cout << "minLocation: " << minLocation << std::endl;

    long long minLocation = LONG_LONG_MAX;
    int i = 0;

    for (auto [startSeed, endSeed] : seedPairs)
    {
        std::cout << "finding location for seed pair: " << i++ << std::endl;
        std::cout << "seeds to check: " << endSeed - startSeed + 1 << std::endl;

        for (long long seed = startSeed; seed <= endSeed; seed++)
        {
            if (seed - startSeed % 1000000 == 0)
            {
                std::cout << "seed " << seed - startSeed << " of " << endSeed - startSeed << std::endl;
            }

            auto location = convert("seed", "location", seed, maps);
            if (location < minLocation)
                minLocation = location;
        }
    }

    std::cout << "minLocation: " << minLocation << std::endl;

    return 0;
}