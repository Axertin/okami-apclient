#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

#include "containermanager.h"

// Get path to test data directory
static std::filesystem::path getTestDataPath()
{
    // Tests run from build directory, test data is in source tree
    return std::filesystem::path(__FILE__).parent_path() / "data";
}

// Fixture to ensure container data is cleared between tests
class ContainerManagerTestFixture
{
  public:
    ContainerManagerTestFixture()
    {
        container_manager::clearContainers();
    }

    ~ContainerManagerTestFixture()
    {
        container_manager::clearContainers();
    }
};

// Pure function tests
TEST_CASE("container_manager::getLocationId", "[containermanager]")
{
    SECTION("Calculates correct location ID")
    {
        CHECK(container_manager::getLocationId(0) == 900000);
        CHECK(container_manager::getLocationId(1) == 900001);
        CHECK(container_manager::getLocationId(100) == 900100);
        CHECK(container_manager::getLocationId(1000) == 901000);
    }

    SECTION("Is constexpr")
    {
        static_assert(container_manager::getLocationId(42) == 900042);
    }
}

// JSON loading tests
TEST_CASE_METHOD(ContainerManagerTestFixture, "container_manager JSON loading", "[containermanager]")
{
    SECTION("Valid JSON loads successfully")
    {
        auto testJson = getTestDataPath() / "test_containers.json";
        REQUIRE(std::filesystem::exists(testJson));

        bool result = container_manager::loadContainersFromPath(testJson);

        CHECK(result == true);
        CHECK(container_manager::getContainerCount() == 3);
    }

    SECTION("Missing file returns false")
    {
        auto missingPath = getTestDataPath() / "nonexistent.json";

        bool result = container_manager::loadContainersFromPath(missingPath);

        CHECK(result == false);
        CHECK(container_manager::getContainerCount() == 0);
    }

    SECTION("Malformed JSON returns false")
    {
        // Create a temporary malformed JSON file
        auto tempPath = getTestDataPath() / "temp_malformed.json";
        {
            std::ofstream file(tempPath);
            file << "{ this is not valid json }";
        }

        bool result = container_manager::loadContainersFromPath(tempPath);

        CHECK(result == false);

        // Clean up
        std::filesystem::remove(tempPath);
    }
}

// Container lookup tests
TEST_CASE_METHOD(ContainerManagerTestFixture, "container_manager::findContainer", "[containermanager]")
{
    auto testJson = getTestDataPath() / "test_containers.json";
    REQUIRE(container_manager::loadContainersFromPath(testJson));

    SECTION("Finds container at known position")
    {
        auto container = container_manager::findContainer(6, 100, 50, -30);

        REQUIRE(container.has_value());
        CHECK(container->id == 0);
        CHECK(container->level_id == 6);
        CHECK(container->x == 100);
        CHECK(container->y == 50);
        CHECK(container->z == -30);
        CHECK(container->original_contents_id == 51);
        CHECK(container->container_type == 0);
    }

    SECTION("Finds different container at different position")
    {
        auto container = container_manager::findContainer(6, 200, 60, -40);

        REQUIRE(container.has_value());
        CHECK(container->id == 1);
        CHECK(container->container_type == 1);
    }

    SECTION("Returns nullopt for unknown position")
    {
        auto container = container_manager::findContainer(6, 999, 999, 999);

        CHECK_FALSE(container.has_value());
    }

    SECTION("Returns nullopt for wrong level")
    {
        // Position exists in level 6, but we're querying level 99
        auto container = container_manager::findContainer(99, 100, 50, -30);

        CHECK_FALSE(container.has_value());
    }
}

TEST_CASE_METHOD(ContainerManagerTestFixture, "container_manager::getContainersForLevel", "[containermanager]")
{
    auto testJson = getTestDataPath() / "test_containers.json";
    REQUIRE(container_manager::loadContainersFromPath(testJson));

    SECTION("Returns all containers for level with multiple containers")
    {
        auto containers = container_manager::getContainersForLevel(6);

        CHECK(containers.size() == 2);
    }

    SECTION("Returns single container for level with one container")
    {
        auto containers = container_manager::getContainersForLevel(10);

        REQUIRE(containers.size() == 1);
        CHECK(containers[0].id == 2);
    }

    SECTION("Returns empty vector for level with no containers")
    {
        auto containers = container_manager::getContainersForLevel(99);

        CHECK(containers.empty());
    }
}

TEST_CASE_METHOD(ContainerManagerTestFixture, "container_manager::clearContainers", "[containermanager]")
{
    auto testJson = getTestDataPath() / "test_containers.json";
    REQUIRE(container_manager::loadContainersFromPath(testJson));
    REQUIRE(container_manager::getContainerCount() == 3);

    container_manager::clearContainers();

    CHECK(container_manager::getContainerCount() == 0);
    CHECK_FALSE(container_manager::findContainer(6, 100, 50, -30).has_value());
}
