// For more information see https://okami.speedruns.wiki/Message_Data_(.MSD)_File_Format

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <okami/msd.h>

namespace okami
{
constexpr uint16_t EndDialog = 0x8001;
constexpr uint16_t UnsupportedChar = 201;

static_assert(sizeof(MSDHeader) == 12);

// Translate underscores to spaces outside of this library
// Only works for English
// Indexed by ASCII code point (0-127); entries outside that range use UnsupportedChar.
static constexpr auto buildASCIIToMSDTable() noexcept
{
    std::array<uint16_t, 128> t{};
    for (auto &v : t)
        v = UnsupportedChar;

    t[' '] = 0;
    t['.'] = 1;
    t[','] = 2;
    t['?'] = 3;
    t['!'] = 4;
    t['('] = 5;
    t[')'] = 6;
    t['*'] = 7;
    t['/'] = 8;
    t['\\'] = 8;
    t['"'] = 9;
    t['\''] = 10;
    t['['] = 11;
    t[']'] = 12;
    t['{'] = 11;
    t['}'] = 12;
    t['0'] = 13;
    t['1'] = 14;
    t['2'] = 15;
    t['3'] = 16;
    t['4'] = 17;
    t['5'] = 18;
    t['6'] = 19;
    t['7'] = 20;
    t['8'] = 21;
    t['9'] = 22;
    t['A'] = 23;
    t['B'] = 24;
    t['C'] = 25;
    t['D'] = 26;
    t['E'] = 27;
    t['F'] = 28;
    t['G'] = 29;
    t['H'] = 30;
    t['I'] = 31;
    t['J'] = 32;
    t['K'] = 33;
    t['L'] = 34;
    t['M'] = 35;
    t['N'] = 36;
    t['O'] = 37;
    t['P'] = 38;
    t['Q'] = 39;
    t['R'] = 40;
    t['S'] = 41;
    t['T'] = 42;
    t['U'] = 43;
    t['V'] = 44;
    t['W'] = 45;
    t['X'] = 46;
    t['Y'] = 47;
    t['Z'] = 48;
    t['a'] = 49;
    t['b'] = 50;
    t['c'] = 51;
    t['d'] = 52;
    t['e'] = 53;
    t['f'] = 54;
    t['g'] = 55;
    t['h'] = 56;
    t['i'] = 57;
    t['j'] = 58;
    t['k'] = 59;
    t['l'] = 60;
    t['m'] = 61;
    t['n'] = 62;
    t['o'] = 63;
    t['p'] = 64;
    t['q'] = 65;
    t['r'] = 66;
    t['s'] = 67;
    t['t'] = 68;
    t['u'] = 69;
    t['v'] = 70;
    t['w'] = 71;
    t['x'] = 72;
    t['y'] = 73;
    t['z'] = 74;
    t['^'] = 82;
    t['<'] = 85;
    t['_'] = 94;
    t['~'] = 95;
    t['>'] = 98;
    t['|'] = 107;
    t['+'] = 197;
    t['-'] = 198;
    t[':'] = 203;
    // Best matches for punctuation without a direct MSD equivalent
    t[';'] = 102;
    t['`'] = 121;
    t['$'] = 106;
    t['%'] = 98;
    t['&'] = 108;
    t['#'] = 81;
    t['='] = 209;

    return t;
}
static constexpr std::array<uint16_t, 128> kASCIIToMSDTable = buildASCIIToMSDTable();

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
        // This is how Okami tests for EndDialog internally
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
        auto uc = static_cast<unsigned char>(c);
        result.push_back(uc < kASCIIToMSDTable.size() ? kASCIIToMSDTable[uc] : UnsupportedChar);
    }
    result.push_back(EndDialog);
    return result;
}

uint32_t MSDManager::AddString(const std::string &str)
{
    this->strings.emplace_back(CompileString(str));
    this->MakeDirty();
    return static_cast<uint32_t>(this->strings.size() - 1);
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
    uint32_t newSize = static_cast<uint32_t>(this->Size());

    this->compiledMSD.clear();
    this->compiledMSD.reserve(sizeof(uint32_t) + newSize * sizeof(uint64_t) + newSize * sizeof(uint16_t));

    // Rebuild MSD header
    this->compiledMSD.append(newSize);

    // Offsets
    uint64_t offset = sizeof(uint32_t) + this->strings.size() * sizeof(uint64_t);
    for (auto &str : this->strings)
    {
        this->compiledMSD.append(offset);
        offset += str.size() * sizeof(uint16_t);
    }

    // Strings
    for (auto &str : this->strings)
    {
        this->compiledMSD.append_range(str);
    }
    this->dirty = false;
}

const uint8_t *MSDManager::GetData()
{
    if (this->dirty)
    {
        this->Rebuild();
    }
    return this->compiledMSD.data();
}

void MSDManager::MakeDirty()
{
    this->dirty = true;
}

} // namespace okami
