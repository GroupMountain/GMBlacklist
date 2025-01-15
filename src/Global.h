#pragma once
#include <include_all.h>
#include <sstream>

#define PLUGIN_NAME "GMBlacklist"

extern nlohmann::json mBanList;
extern nlohmann::json mBanIpList;
extern nlohmann::json mUserCache;

extern std::string tr(std::string const& key, std::vector<std::string> const& data = {});
extern std::string getExpiredTime(int offsetMinutes = 0);

extern void initDataFile();

extern void RegisterCommands();
extern void checkBanTimeTask();

extern bool banPlayer(std::string& name, std::string& opSource, int time, std::string& reason);
extern bool banIP(std::string& ip, std::string& opSource, int time, std::string& reason);
extern bool banOnlinePlayer(Player* pl, std::string& opSource, int time, std::string& reason);
extern bool unbanPlayer(std::string& name);
extern bool unbanIP(std::string& ip);

extern void showBanPlayersList(CommandOutput& output);
extern void showBanIpsList(CommandOutput& output);

extern void listenEvent();

extern void RegisterCommands();