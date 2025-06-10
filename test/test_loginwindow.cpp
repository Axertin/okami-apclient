#pragma warning(push, 0)
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <filesystem>
#include "loginwindow.h"
#pragma warning(pop)

TEST_CASE("LoginWindow::saveLoginData and loadLoginData - Greenpath", "[login]")
{
    LoginWindow window(false);
    std::string testFile = "./connection_test.json";

    std::string testServer = "example.org";
    std::string testSlot = "PlayerOne";
    std::string testPassword = "supersecret";

    // Save data
    window.saveLoginData(testFile, testServer, testSlot, testPassword);

    // Load data
    std::string loadedServer, loadedSlot, loadedPassword;
    bool result = window.loadLoginData(testFile, loadedServer, loadedSlot, loadedPassword);

    REQUIRE(result == true);
    CHECK(loadedServer == testServer);
    CHECK(loadedSlot == testSlot);
    CHECK(loadedPassword == testPassword);

    // Cleanup
    std::filesystem::remove(testFile);
}

TEST_CASE("LoginWindow::loadLoginData - Redpath (missing file)", "[login]")
{
    LoginWindow window(false);
    std::string testFile = "./nonexistent_file.json";

    std::string server, slot, password;
    bool result = window.loadLoginData(testFile, server, slot, password);

    REQUIRE(result == false);
}

TEST_CASE("LoginWindow::loadLoginData - Redpath (invalid JSON)", "[login]")
{
    LoginWindow window(false);
    std::string testFile = "./invalid_json.json";

    // Write malformed JSON
    std::ofstream out(testFile);
    out << "not a valid json { this is garbage";
    out.close();

    std::string server, slot, password;
    bool result = window.loadLoginData(testFile, server, slot, password);

    REQUIRE(result == false);

    // Cleanup
    std::filesystem::remove(testFile);
}
