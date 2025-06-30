#include <string>
#include <unordered_map>

namespace okami
{

struct MapDesc
{
    std::unordered_map<unsigned, std::string> worldStateBits;
    std::unordered_map<unsigned, std::string> userIndices;
    std::unordered_map<unsigned, std::string> collectedObjects;
    std::unordered_map<unsigned, std::string> areasRestored;
    std::unordered_map<unsigned, std::string> treesBloomed;
    std::unordered_map<unsigned, std::string> cursedTreesBloomed;
    std::unordered_map<unsigned, std::string> fightsCleared;
    std::unordered_map<unsigned, std::string> npcHasMoreToSay;
    std::unordered_map<unsigned, std::string> npcUnknown;
    std::unordered_map<unsigned, std::string> mapsExplored;
    std::unordered_map<unsigned, std::string> field_DC;
    std::unordered_map<unsigned, std::string> field_E0;
};

extern const std::unordered_map<unsigned, MapDesc> mapDataDesc;
} // namespace okami
