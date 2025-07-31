#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "okami/blowfish.h"
#include "okami/data/brushtype.hpp"
#include "okami/items.hpp"
#include "okami/memorymap.hpp"

namespace okami
{
using data_t = std::vector<uint8_t>;

void InitBlowfish()
{
    static bool initialized = false;
    if (initialized)
        return;

    Nippon::BlowFish::Create(OKAMI_CIPHER_KEY);
    initialized = true;
}

struct ResourceType
{
    char ext[4];

    bool operator==(const ResourceType &other) const = default;
};

static_assert(sizeof(ResourceType) == 4);

class ResourcePackage
{
  private:
    std::vector<data_t> entryData;
    std::vector<ResourceType> entryTypes;
    bool valid = true;

    static data_t LoadPackageData(const std::string &filename)
    {
        std::ifstream file{filename, std::ios::binary};
        if (!file)
        {
            std::cerr << "Package not found: " << filename << std::endl;
            return {};
        }

        auto fileSize = std::filesystem::file_size(filename);
        data_t result(fileSize);
        file.read(reinterpret_cast<char *>(result.data()), result.size());

        InitBlowfish();
        Nippon::BlowFish::Decrypt(result);
        return result;
    }

  public:
    ResourcePackage()
    {
    }

    ResourcePackage(const std::string &filename) : ResourcePackage(LoadPackageData(filename))
    {
    }

    ResourcePackage(const data_t &data)
    {
        if (data.size() <= 32)
        {
            this->valid = false;
            return;
        }

        const uint32_t *ptr32 = reinterpret_cast<const uint32_t *>(data.data());

        uint32_t numEntries = ptr32[0];
        for (uint32_t i = 0; i < numEntries - 1; i++)
        {
            ResourceType type = *reinterpret_cast<const ResourceType *>(&ptr32[1 + numEntries + i]);
            uint32_t offset = ptr32[1 + i];
            uint32_t next = 0;
            for (uint32_t j = 1; next == 0 && i + j < numEntries; j++)
            {
                next = ptr32[1 + i + j];
            }

            if (offset == 0)
            {
                this->entryData.push_back({});
            }
            else
            {
                this->entryData.emplace_back(&data[offset], &data[next]);
            }
            this->entryTypes.emplace_back(type);
        }
    }

    bool isValid() const
    {
        return this->valid;
    }
    operator bool() const
    {
        return this->valid;
    }

    bool entryValid(uint32_t idx) const
    {
        return this->valid && idx < this->size();
    }

    size_t size() const
    {
        return this->entryData.size();
    }

    std::optional<data_t> getEntryData(ResourceType type, uint32_t relative_index = 0) const
    {
        uint32_t n = 0;
        for (uint32_t i = 0; i < this->size(); i++)
        {
            if (this->entryTypes[i] != type)
                continue;

            if (n++ == relative_index)
            {
                return this->entryData[i];
            }
        }
        return std::nullopt;
    }

    std::optional<data_t> getEntryData(uint32_t idx) const
    {
        if (!this->entryValid(idx))
            return std::nullopt;

        return this->entryData[idx];
    }

    void addBlank()
    {
        if (!this->isValid())
            return;

        this->entryData.push_back({});
        this->entryTypes.push_back({});
    }

    void addEntry(ResourceType type, const data_t &data)
    {
        if (!this->isValid())
            return;

        this->entryData.emplace_back(data);
        this->entryTypes.emplace_back(type);
    }

    void write(std::filesystem::path filename)
    {
        std::filesystem::create_directories(filename.parent_path());

        data_t result;
        auto writeResult = [&]<class T>(const T &value)
        {
            const uint8_t *valueRaw = reinterpret_cast<const uint8_t *>(&value);
            result.insert(result.end(), valueRaw, valueRaw + sizeof(T));
        };

        // +1 to add ROF
        uint32_t numElems = this->size() + 1;
        writeResult(numElems);

        // Save these to write the ROF section later
        std::vector<uint32_t> entryOffsets;

        // Write offsets
        uint32_t offset = (1 + numElems + numElems) * sizeof(uint32_t);
        for (auto &data : this->entryData)
        {
            entryOffsets.emplace_back(offset);
            writeResult(offset);
            offset += data.size();
        }
        // ROF offset
        entryOffsets.emplace_back(offset);
        writeResult(offset);

        // Write types
        for (auto &type : this->entryTypes)
        {
            writeResult(type);
        }
        writeResult(ResourceType{"ROF"});

        // Write data
        for (auto &data : this->entryData)
        {
            result.insert(result.end(), data.begin(), data.end());
        }

        // Write ROF
        const uint8_t *rofHead = reinterpret_cast<const uint8_t *>("RUNOFS64");
        result.insert(result.end(), rofHead, rofHead + 8);

        for (uint32_t entryOffset : entryOffsets)
        {
            uint64_t entryOffset64 = entryOffset;
            writeResult(entryOffset64);
        }

        // Align to 32 bytes to be safe
        result.insert(result.end(), 32 - result.size() % 32, '\0');

        InitBlowfish();
        Nippon::BlowFish::Encrypt(result);

        std::ofstream out{filename, std::ios::binary};
        out.write(reinterpret_cast<char *>(result.data()), result.size());
    }
};

std::string GetItemFilename(uint32_t itemId)
{
    return std::format("it/it{:02x}.dat", itemId);
}

std::optional<data_t> GetItemIcon(uint32_t itemId)
{
    ResourcePackage pkg{"data_pc/" + GetItemFilename(itemId)};
    if (!pkg)
        return std::nullopt;

    auto icoData = pkg.getEntryData({"ICO"});
    if (!icoData)
    {
        std::cerr << "ICO not found in item package for item id " << itemId << std::endl;
    }
    return icoData;
}

const std::unordered_map<uint32_t, uint32_t> FishItemIconMapping = {
    {ItemTypes::SupremeTuna, 1},
    {ItemTypes::MountainTrout, 2},
    {ItemTypes::RedSnapper, 3},
    {ItemTypes::StripedSnapper, 4},
    {ItemTypes::Salmon, 5},
    {ItemTypes::Koi, 6},
    {ItemTypes::Huchen, 7},
    {ItemTypes::Robalo, 8},
    {ItemTypes::BlackBass, 9},
    {ItemTypes::Clownfish, 10},
    {ItemTypes::Bonito, 11},
    {ItemTypes::Yellowtail, 12},
    {ItemTypes::Sweetfish, 13},
    {ItemTypes::Trout, 14},
    {ItemTypes::Smelt, 15},
    {ItemTypes::Killifish, 16},
    {ItemTypes::FlyingFish, 17},
    {ItemTypes::Sturgeon, 18},
    {ItemTypes::Sunfish, 19},
    {ItemTypes::FreshwaterEel, 20},
    {ItemTypes::Loach, 21},
    {ItemTypes::Moray, 22},
    {ItemTypes::Oarfish, 23},
    {ItemTypes::Monkfish, 24},
    {ItemTypes::Catfish, 25},
    {ItemTypes::GiantCatfish, 26},
    {ItemTypes::Goby, 27},
    {ItemTypes::Lobster, 28},
    {ItemTypes::Crawfish, 29},
    {ItemTypes::Scallop, 30},
    {ItemTypes::Nautilus, 31},
    {ItemTypes::Manta, 32},
    {ItemTypes::Blowfish, 33},
    {ItemTypes::RiverCrab, 34},
    {ItemTypes::Starfish, 35},
    {ItemTypes::Marlin, 36},
    {ItemTypes::LoggerheadTurtle, 37},
    {ItemTypes::SeaHorse, 38},
    {ItemTypes::Octopus, 39},
    {ItemTypes::Squid, 40},
    {ItemTypes::Whopper, 41},
    {ItemTypes::CutlassFish, 42},
    {ItemTypes::GiantSalmon, 43},
};
std::optional<data_t> GetFishIcon(uint32_t itemId)
{
    static ResourcePackage pkg{"data_pc/id/ItemShopSellFishIcon.dat"};
    if (!pkg)
        return std::nullopt;

    auto icoData = pkg.getEntryData(FishItemIconMapping.at(itemId) - 1);
    if (!icoData)
    {
        std::cerr << "DDS not found in fish package for item id " << itemId << std::endl;
    }
    return icoData;
}

const std::unordered_map<uint32_t, uint32_t> InventoryItemIconMapping = {
    {ItemTypes::FeedbagSeeds, 1},
    {ItemTypes::FeedbagHerbs, 2},
    {ItemTypes::FeedbagMeat, 3},
    {ItemTypes::FeedbagFish, 4},
    {ItemTypes::HolyBoneS, 5},
    {ItemTypes::HolyBoneM, 6},
    {ItemTypes::HolyBoneL, 7},
    {ItemTypes::ExorcismSlipS, 8},
    {ItemTypes::ExorcismSlipM, 9},
    {ItemTypes::ExorcismSlipL, 10},
    {ItemTypes::VengeanceSlip, 11},
    {ItemTypes::InkfinityStone, 12},
    {ItemTypes::MermaidCoin, 13},
    {ItemTypes::TravelersCharm, 14},
    {ItemTypes::SteelFistSake, 15},
    {ItemTypes::SteelSoulSake, 16},
    {ItemTypes::GoldDust, 17},
    {ItemTypes::GodlyCharm, 18},
    {ItemTypes::GoldenPeach, 19},
    {ItemTypes::KarmicReturner, 20},
    {ItemTypes::KarmicTransformer3, 21},
    {ItemTypes::KarmicTransformer8, 22},
    {ItemTypes::KarmicTransformer7, 23},
    {ItemTypes::KarmicTransformer9, 24},
    {ItemTypes::KarmicTransformer1, 25},
    {ItemTypes::KarmicTransformer2, 26},
    {ItemTypes::KarmicTransformer6, 27},
    {ItemTypes::KarmicTransformer5, 28},
    {ItemTypes::KarmicTransformer4, 29},
    {ItemTypes::CanineTracker, 30},
    {ItemTypes::LuckyMallet, 31},
    {ItemTypes::BorderKey, 32},
    {ItemTypes::DragonOrb, 33},
    {ItemTypes::FoxRods, 34},
    {ItemTypes::ThunderBrew, 35},
    {ItemTypes::ShellAmulet, 36},
    {ItemTypes::Mask, 37},
    {ItemTypes::OgreLiver, 38},
    {ItemTypes::LipsOfIce, 39},
    {ItemTypes::EyeballOfFire, 40},
    {ItemTypes::BlackDemonHorn, 41},
    {ItemTypes::LoyaltyOrb, 42},
    {ItemTypes::JusticeOrb, 43},
    {ItemTypes::DutyOrb, 44},
    {ItemTypes::GoldenMushroom, 46},
    {ItemTypes::GimmickGear, 47},
    {ItemTypes::PurificationSake, 48},
    {ItemTypes::Sewaprolo, 49},
    {ItemTypes::Charcoal, 50},
    {ItemTypes::BlindingSnow, 51},
    {ItemTypes::MarlinRod, 52},
    {ItemTypes::TreasureBox, 53},
    {ItemTypes::HerbalMedicine, 54},
    {ItemTypes::Pinwheel, 55},
    {ItemTypes::DivineRetribution, 56},
    {ItemTypes::SnarlingBeast, 57},
    {ItemTypes::InfinityJudge, 58},
    {ItemTypes::TrinityMirror, 59},
    {ItemTypes::SolarFlare, 60},
    {ItemTypes::DevoutBeads, 61},
    {ItemTypes::LifeBeads, 62},
    {ItemTypes::ExorcismBeads, 63},
    {ItemTypes::ResurrectionBeads, 64},
    {ItemTypes::TundraBeads, 65},
    {ItemTypes::Tsumugari, 66},
    {ItemTypes::SevenStrike, 67},
    {ItemTypes::BladeOfKusanagi, 68},
    {ItemTypes::EighthWonder, 69},
    {ItemTypes::ThunderEdge, 70},
    {ItemTypes::FireTablet, 71},
    {ItemTypes::WaterTablet, 72},
    {ItemTypes::PeaceBell, 73},
    {ItemTypes::GoldenLuckyCat, 74},
    {ItemTypes::ThiefsGlove, 75},
    {ItemTypes::WoodMat, 76},
    {ItemTypes::GoldenInkPot, 77},
    {ItemTypes::StringOfBeads, 78},
};
std::optional<data_t> GetInventoryIcon(uint32_t itemId)
{
    static ResourcePackage pkg{"data_pc/id/idsgitemcore.idd"};
    if (!pkg)
        return std::nullopt;

    static ResourcePackage binPkg{pkg.getEntryData({"TBL"}).value_or({})};
    if (!binPkg)
        return std::nullopt;

    static ResourcePackage datPkg{binPkg.getEntryData({"DAT"}).value_or({})};
    if (!datPkg)
        return std::nullopt;

    auto icoData = datPkg.getEntryData(InventoryItemIconMapping.at(itemId) - 1);
    if (!icoData)
    {
        std::cerr << "DDS not found in inventory package for item id " << itemId << std::endl;
    }
    return icoData;
}

const std::unordered_map<uint32_t, uint32_t> FangShopItemIconMapping = {
    {ItemTypes::TravelersCharm, 1}, {ItemTypes::FogPot, 2},      {ItemTypes::WaterTablet, 3}, {ItemTypes::GodlyCharm, 4},     {ItemTypes::GoldenPeach, 5},
    {ItemTypes::PeaceBell, 6},      {ItemTypes::ThiefsGlove, 7}, {ItemTypes::WoodMat, 8},     {ItemTypes::GoldenLuckyCat, 9}, {ItemTypes::GoldenInkPot, 10},
};
std::optional<data_t> GetFangShopIcon(uint32_t itemId)
{
    static ResourcePackage pkg{"data_pc/id/idkibashop.idd"};
    if (!pkg)
        return std::nullopt;

    static ResourcePackage binPkg{pkg.getEntryData({"TBL"}).value_or({})};
    if (!binPkg)
        return std::nullopt;

    static ResourcePackage datPkg{binPkg.getEntryData({"DAT"}).value_or({})};
    if (!datPkg)
        return std::nullopt;

    auto icoData = datPkg.getEntryData(FangShopItemIconMapping.at(itemId) - 1);
    if (!icoData)
    {
        std::cerr << "DDS not found in inventory package for item id " << itemId << std::endl;
    }
    return icoData;
}

std::optional<data_t> GetPraiseIcon()
{
    static ResourcePackage pkg{"data_pc/id/idsgstatuscore.idd"};
    if (!pkg)
        return std::nullopt;

    static ResourcePackage texPkg{pkg.getEntryData({"TBL"}, 1).value_or({})};
    if (!texPkg)
        return std::nullopt;

    auto icoData = texPkg.getEntryData(15);
    if (!icoData)
    {
        std::cerr << "DDS not found for Praise" << std::endl;
    }
    return icoData;
}

std::optional<data_t> GetItemGraphic(uint32_t itemId)
{
    std::optional<data_t> iconTexData = std::nullopt;

    if (FishItemIconMapping.contains(itemId))
    {
        iconTexData = GetFishIcon(itemId);
    }
    else if (InventoryItemIconMapping.contains(itemId))
    {
        // For Karmic Transformers
        iconTexData = GetInventoryIcon(itemId);
    }
    else if (FangShopItemIconMapping.contains(itemId))
    {
        // For Fog Pot
        iconTexData = GetFangShopIcon(itemId);
    }
    else if (itemId == ItemTypes::Praise)
    {
        iconTexData = GetPraiseIcon();
    }

    if (!iconTexData)
    {
        iconTexData = GetItemIcon(itemId);
    }

    // TODO replacements for: 10 yen, 50 yen, 100 yen, 150 yen, 500 yen, Spirit Globe S + Ink Bottle

    // TODO ??? for dojo moves
    // TODO satome orbs

    return iconTexData;
}

const std::unordered_map<uint32_t, uint32_t> BrushIconMapping = {
    {BrushTypes::PowerSlash, 1},    {BrushTypes::Galestorm, 2},    {BrushTypes::Bloom, 3},    {BrushTypes::VeilOfMist, 4}, {BrushTypes::Sunrise, 5},
    {BrushTypes::Inferno, 6},       {BrushTypes::Rejuvenation, 7}, {BrushTypes::Blizzard, 8}, {BrushTypes::Waterspout, 9}, {BrushTypes::Crescent, 10},
    {BrushTypes::ThunderStorm, 11}, {BrushTypes::CherryBomb, 12},  {BrushTypes::Catwalk, 13}, {BrushTypes::WaterLily, 21}, {BrushTypes::VineBase, 22},
};
std::optional<data_t> GetBrushGraphic(uint32_t brushId)
{
    // TODO: IceStorm, MistWarp, Fountain, Deluge, Fireburst, Thunderbolt, Whirlwind, DotTrees, Greensprout
    static ResourcePackage pkg{"data_pc/id/idsgfudecore.idd"};
    if (!pkg)
        return std::nullopt;

    static ResourcePackage texPkg{pkg.getEntryData({"TBL"}, 1).value_or({})};
    if (!texPkg)
        return std::nullopt;

    auto icoData = texPkg.getEntryData(BrushIconMapping.at(brushId) - 1);
    if (!icoData)
    {
        std::cerr << "DDS not found for brush " << brushId << std::endl;
    }
    return icoData;
}

void WriteNewPackage(const std::vector<data_t> &pkgData, const std::filesystem::path &filename)
{
    ResourcePackage pkg;
    for (const data_t &data : pkgData)
    {
        pkg.addEntry({"DDS"}, data);
    }
    pkg.write(filename);
}

std::optional<data_t> LoadAPIcon()
{
    std::string apIconFilename = "mods/apclient/game-data/icons/ap.dds";
    std::ifstream apIconIn{apIconFilename, std::ios::binary};
    if (!apIconIn)
        return std::nullopt;

    size_t apIconSize = std::filesystem::file_size(apIconFilename);
    data_t apIcon(apIconSize);

    apIconIn.read(reinterpret_cast<char *>(apIcon.data()), apIcon.size());
    return apIcon;
}

void RecompileItemGraphics()
{
    std::vector<data_t> newPkgTextures;

    // TODO: icon dump is temporary, disable in 1.0/RELEASE
    std::filesystem::create_directories("dump/");
    for (unsigned i = 0; i < ItemTypes::NUM_ITEM_TYPES; i++)
    {
        // FIXME: this is temporary until suspend process hooks are fixed
        if (i == okami::ItemTypes::Unused_52)
        {
            std::optional<data_t> apIcon = LoadAPIcon();
            if (apIcon)
            {
                newPkgTextures.emplace_back(*apIcon);
                continue;
            }
        }

        std::optional<data_t> iconTexData = GetItemGraphic(i);
        if (iconTexData)
        {
            std::ofstream out{std::format("dump/item {:03} ({}).dds", i, okami::ItemTypes::GetName(i)), std::ios::binary};
            out.write(reinterpret_cast<char *>(iconTexData->data()), iconTexData->size());

            newPkgTextures.emplace_back(*iconTexData);
        }
        else
        {
            std::ofstream out{std::format("dump/item {:03} ({}) NOT FOUND.dds", i, okami::ItemTypes::GetName(i))};
            out << "oof" << std::endl;

            if (!newPkgTextures.empty())
            {
                newPkgTextures.emplace_back(newPkgTextures[0]);
            }
        }
    }

    for (unsigned i = 0; i < BrushTypes::NUM_BRUSH_TYPES; i++)
    {
        std::optional<data_t> iconTexData = std::nullopt;
        if (BrushIconMapping.contains(i))
        {
            iconTexData = GetBrushGraphic(i);
        }

        if (iconTexData)
        {
            std::ofstream out{std::format("dump/brush {:03} ({}).dds", i, okami::BrushTypes::GetName(i)), std::ios::binary};
            out.write(reinterpret_cast<char *>(iconTexData->data()), iconTexData->size());
        }
        else
        {
            std::ofstream out{std::format("dump/brush {:03} ({}) NOT FOUND.dds", i, okami::BrushTypes::GetName(i))};
            out << "oof" << std::endl;
        }
    }

    WriteNewPackage(newPkgTextures, "data_pc/archipelago/ItemPackage.dat");
}

} // namespace okami
