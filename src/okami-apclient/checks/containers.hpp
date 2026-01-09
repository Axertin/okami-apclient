#pragma once

#include <cstdint>
#include <functional>
#include <unordered_set>

// Forward declarations
class ISocket;

namespace checks
{

// Container hook constants
constexpr uintptr_t SPAWN_TABLE_POPULATOR_OFFSET = 0x49e570;
constexpr uintptr_t SPAWN_TABLE_OFFSET = 0xB66800;
constexpr uintptr_t CURRENT_MAP_ID_OFFSET = 0xB6B240;
constexpr uint8_t DUMMY_ITEM_ID = 0x83; // Chestnut

/**
 * @brief Handler for container-based checks
 *
 * Manages the spawn table hook for replacing container items with dummies,
 * and polls for container collection to send checks.
 */
class ContainerMan
{
  public:
    using CheckCallback = std::function<void(int64_t)>;

    /**
     * @brief Construct a ContainerMan
     * @param socket Reference to socket for connection status checks
     * @param checkCallback Callback to invoke when a container is collected
     */
    explicit ContainerMan(ISocket &socket, CheckCallback checkCallback);

    ~ContainerMan();

    // Non-copyable, non-movable (owns hook)
    ContainerMan(const ContainerMan &) = delete;
    ContainerMan &operator=(const ContainerMan &) = delete;
    ContainerMan(ContainerMan &&) = delete;
    ContainerMan &operator=(ContainerMan &&) = delete;

    /**
     * @brief Initialize the spawn table hook
     */
    void initialize();

    /**
     * @brief Shutdown and cleanup hook
     */
    void shutdown();

    /**
     * @brief Reset tracking state (called on level change)
     */
    void reset();

    /**
     * @brief Poll for collected containers
     */
    void poll();

    /**
     * @brief Check if a container location is part of randomization
     * @param locationId The container location ID
     * @return true if randomized
     */
    [[nodiscard]] bool isContainerInRando(int64_t locationId) const;

  private:
    void onSpawnTablePopulate(void *spawnTable);

    // Hook function (needs to be static for function pointer)
    static void hookSpawnTablePopulator(void *spawnTable);

    ISocket &socket_;
    CheckCallback checkCallback_;

    std::unordered_set<int> trackedContainerIndices_;
    uint16_t currentLevelId_ = 0;

    // Hook state
    using SpawnTablePopulatorFn = void (*)(void *);
    static SpawnTablePopulatorFn originalSpawnTablePopulator_;
    static ContainerMan *activeInstance_;

    bool initialized_ = false;
};

} // namespace checks
