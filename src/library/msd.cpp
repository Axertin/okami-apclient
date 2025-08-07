// For more information see https://okami.speedruns.wiki/Message_Data_(.MSD)_File_Format

#include "okami/msd.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace okami
{
constexpr uint16_t EndDialog = 0x8001;
constexpr uint16_t UnsupportedChar = 201;

static_assert(sizeof(MSDHeader) == 12);

// Translate underscores to spaces outside of this library
// Only works for English
static std::unordered_map<char, uint16_t> ASCIIToMSDMap = {
    {' ', 0},
    {'.', 1},
    {',', 2},
    {'?', 3},
    {'!', 4},
    {'(', 5},
    {')', 6},
    {'*', 7},
    {'/', 8},
    {'\\', 8},
    {'"', 9},
    {'\'', 10},
    {'[', 11},
    {']', 12},
    {'{', 11},
    {'}', 12},
    {'0', 13},
    {'1', 14},
    {'2', 15},
    {'3', 16},
    {'4', 17},
    {'5', 18},
    {'6', 19},
    {'7', 20},
    {'8', 21},
    {'9', 22},
    {'A', 23},
    {'B', 24},
    {'C', 25},
    {'D', 26},
    {'E', 27},
    {'F', 28},
    {'G', 29},
    {'H', 30},
    {'I', 31},
    {'J', 32},
    {'K', 33},
    {'L', 34},
    {'M', 35},
    {'N', 36},
    {'O', 37},
    {'P', 38},
    {'Q', 39},
    {'R', 40},
    {'S', 41},
    {'T', 42},
    {'U', 43},
    {'V', 44},
    {'W', 45},
    {'X', 46},
    {'Y', 47},
    {'Z', 48},
    {'a', 49},
    {'b', 50},
    {'c', 51},
    {'d', 52},
    {'e', 53},
    {'f', 54},
    {'g', 55},
    {'h', 56},
    {'i', 57},
    {'j', 58},
    {'k', 59},
    {'l', 60},
    {'m', 61},
    {'n', 62},
    {'o', 63},
    {'p', 64},
    {'q', 65},
    {'r', 66},
    {'s', 67},
    {'t', 68},
    {'u', 69},
    {'v', 70},
    {'w', 71},
    {'x', 72},
    {'y', 73},
    {'z', 74},
    {'^', 82},
    {'<', 85},
    {'_', 94},
    {'~', 95},
    {'>', 98},
    {'|', 107},
    {'.', 124},
    {',', 125},
    {'+', 197},
    {'-', 198},
    {':', 203},
    // Best matches
    {';', 102},
    {'`', 121},
    {'$', 106},
    {'%', 98},
    {'&', 108},
    {'#', 81},
    {'=', 209},
};

MSDManager::MSDManager()
{
}

// IMPORTANT:
void MSDManager::ReadMSD(const void *pData)
{
    const MSDHeader *pHead = reinterpret_cast<const MSDHeader *>(pData);
    const uint8_t *pDataPtr = reinterpret_cast<const uint8_t *>(pData);
    for (uint32_t i = 0; i < pHead->numEntries; i++)
    {
        std::vector<uint16_t> str;

        const uint16_t *pStrOffset = reinterpret_cast<const uint16_t *>(&pDataPtr[pHead->offsets[i]]);
        const uint16_t *pStr = pStrOffset;
        for (; (*pStr & 0xFF00) != 0x8000; pStr++)
        {
            str.push_back(*pStr);
        }
        str.push_back(*pStr); // include terminator
        this->strings.push_back(str);
    }

    this->MakeDirty();
}

std::vector<uint16_t> MSDManager::CompileString(const std::string &str)
{
    std::vector<uint16_t> result;
    for (char c : str)
    {
        if (auto it = ASCIIToMSDMap.find(c); it != ASCIIToMSDMap.end())
        {
            result.push_back(it->second);
        }
        else
        {
            result.push_back(UnsupportedChar);
        }
    }
    result.push_back(EndDialog);
    return result;
}

uint32_t MSDManager::AddString(const std::string &str)
{
    this->strings.emplace_back(CompileString(str));
    this->MakeDirty();
    return this->strings.size() - 1;
}
void MSDManager::OverrideString(uint32_t index, const std::string &str)
{
    if (index >= this->strings.size())
        return;

    this->strings[index] = CompileString(str);
    this->MakeDirty();
}

size_t MSDManager::Size() const
{
    return this->strings.size();
}

void MSDManager::Rebuild()
{
    uint32_t newSize = this->Size();

    this->compiledMSD.clear();
    this->compiledMSD.reserve(sizeof(uint32_t) + newSize * sizeof(uint64_t) + newSize * sizeof(uint16_t));

    auto appendData = [&](auto &value)
    {
        std::uint8_t *valBytes = reinterpret_cast<std::uint8_t *>(&value);
        this->compiledMSD.insert(this->compiledMSD.end(), valBytes, valBytes + sizeof(value));
    };

    // Rebuild MSD header
    appendData(newSize);

    // Offsets
    uint64_t offset = sizeof(uint32_t) + this->strings.size() * sizeof(uint64_t);
    for (auto &str : this->strings)
    {
        appendData(offset);
        offset += str.size() * sizeof(uint16_t);
    }

    // Strings
    for (auto &str : this->strings)
    {
        for (uint16_t c : str)
        {
            appendData(c);
        }
    }
    this->dirty = false;
}
const std::vector<uint8_t> &MSDManager::GetData()
{
    if (this->dirty)
    {
        this->Rebuild();
    }
    return this->compiledMSD;
}

void MSDManager::MakeDirty()
{
    this->dirty = true;
}

} // namespace okami