#pragma once
// #include <include_all.h>
#include "gmlib/include_lib.h" // IWYU pragma: export
#include "gmlib/include_ll.h"  // IWYU pragma: export
#include "ila/include_all.h"   // IWYU pragma: export
#include <sstream>             // IWYU pragma: export

#define PLUGIN_NAME "GMBlacklist"

extern nlohmann::json mBanList;
extern nlohmann::json mBanIpList;
extern nlohmann::json mUserCache;

std::string getExpiredTime(int offsetMinutes = 0);

void initDataFile();

void RegisterCommands();
void checkBanTimeTask();

bool banPlayer(const std::string& name, const std::string& opSource, int time, const std::string& reason);
bool banIP(const std::string& ip, const std::string& opSource, int time, const std::string& reason);
bool banOnlinePlayer(Player* pl, const std::string& opSource, int time, const std::string& reason);
bool unbanPlayer(const std::string& name);
bool unbanIP(const std::string& ip);

void showBanPlayersList(CommandOutput& output);
void showBanIpsList(CommandOutput& output);

void listenEvent();
