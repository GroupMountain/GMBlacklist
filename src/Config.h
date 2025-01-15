#pragma once
#include <string>

namespace GMBlacklist {
struct Config {
    int version = 1;

    std::string language = "en_US";

    int CommandPermissionLevel = 4;
};
}// namespace GMBlacklist