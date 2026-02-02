#pragma once

#include <charconv>
#include <optional>
#include <string>

namespace version_utils
{

struct Version
{
    int major;
    int minor;
    int patch;
};

// Parse a semver string like "1.2.3" or "0.6.0-dev.21" into Version.
// Pre-release suffixes (e.g., "-dev.21") and build metadata (e.g., "+build.456") are ignored.
inline std::optional<Version> parseVersion(const std::string &str)
{
    if (str.empty())
    {
        return std::nullopt;
    }

    size_t firstDot = str.find('.');
    if (firstDot == std::string::npos)
    {
        return std::nullopt;
    }

    size_t secondDot = str.find('.', firstDot + 1);
    if (secondDot == std::string::npos)
    {
        return std::nullopt;
    }

    // Find where patch ends (either end of string or start of suffix like "-dev" or "+build")
    size_t patchEnd = str.find_first_of("-+", secondDot + 1);
    if (patchEnd == std::string::npos)
    {
        patchEnd = str.length();
    }

    auto parseComponent = [](const std::string &s, size_t start, size_t end) -> std::optional<int>
    {
        if (start >= end)
        {
            return std::nullopt;
        }
        int value = 0;
        auto result = std::from_chars(s.data() + start, s.data() + end, value);
        if (result.ec != std::errc{} || result.ptr != s.data() + end)
        {
            return std::nullopt;
        }
        return value;
    };

    auto major = parseComponent(str, 0, firstDot);
    auto minor = parseComponent(str, firstDot + 1, secondDot);
    auto patch = parseComponent(str, secondDot + 1, patchEnd);

    if (!major || !minor || !patch)
    {
        return std::nullopt;
    }

    return Version{*major, *minor, *patch};
}

enum class Compatibility
{
    Compatible,    // Client meets or exceeds server requirements
    ClientTooOld,  // Client is missing features server expects
    MajorMismatch, // Different major versions (breaking change)
};

// Check if client version is compatible with server's expected version using semver rules.
// - Major 0.x.y: Unstable API, require exact major.minor match
// - Major 1+: Same major required, client minor must be >= server minor
inline Compatibility checkCompatibility(const Version &client, const Version &server)
{
    if (client.major != server.major)
    {
        return Compatibility::MajorMismatch;
    }

    // For major version 0, minor version changes can break compatibility
    if (client.major == 0)
    {
        if (client.minor != server.minor)
        {
            return Compatibility::MajorMismatch;
        }
        return Compatibility::Compatible;
    }

    // For major version 1+, client minor must be >= server minor
    if (client.minor < server.minor)
    {
        return Compatibility::ClientTooOld;
    }

    return Compatibility::Compatible;
}

} // namespace version_utils
