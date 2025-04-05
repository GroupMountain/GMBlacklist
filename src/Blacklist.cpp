#define _CRT_SECURE_NO_WARNINGS
#include "Global.h"
#include "nlohmann/json.hpp"

nlohmann::json mBanList   = nlohmann::json::array();
nlohmann::json mBanIpList = nlohmann::json::array();

std::mutex mtx;

void saveBanFile() {
    // GMLIB::Files::JsonFile::writeFile("./banned-players.json", mBanList);
    std::ofstream("./banned-players.json") << mBanList;
}

void saveBanIpFile() {
    // GMLIB::Files::JsonFile::writeFile("./banned-ips.json", mBanIpList);
    std::ofstream("./banned-ips.json") << mBanIpList;
}

void initDataFile() {
    // mBanList   = GMLIB::Files::JsonFile::initJson("./banned-players.json", nlohmann::json::array());
    // mBanIpList = GMLIB::Files::JsonFile::initJson("./banned-ips.json", nlohmann::json::array());
    try {
        mBanList = nlohmann::json::parse(std::ifstream("./banned-players.json"));
    } catch (...) {
        try {
            saveBanFile();
        } catch (...) {}
    }
    try {
        mBanIpList = nlohmann::json::parse(std::ifstream("./banned-ips.json"));
    } catch (...) {
        try {
            saveBanFile();
        } catch (...) {}
    }
}

std::string getIP(const std::string& ipAndPort) {
    auto pos = ipAndPort.find(':');
    return ipAndPort.substr(0, pos);
}

std::time_t convertStringToTime(const std::string& timeString) {
    std::tm            tm = {};
    std::istringstream iss(timeString);
    iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return std::mktime(&tm);
}

bool isExpired(const std::string& targetTimeStr) {
    if (targetTimeStr == "forever") {
        return false;
    }
    std::time_t                           targetTime   = convertStringToTime(targetTimeStr);
    std::chrono::system_clock::time_point currentTime  = std::chrono::system_clock::now();
    std::time_t                           currentTimeT = std::chrono::system_clock::to_time_t(currentTime);
    if (targetTime > currentTimeT) {
        return false;
    }
    return true;
}

std::string getExpiredTime(int minutes) {
    std::chrono::system_clock::time_point now        = std::chrono::system_clock::now();
    std::time_t                           timestamp  = std::chrono::system_clock::to_time_t(now);
    timestamp                                       += minutes * 60;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S %z");
    return ss.str();
}

bool isBanned(const std::string& uuid, const std::string& realname) {
    for (auto& key : mBanList) {
        if (key.contains("uuid")) {
            if (key["uuid"].get<std::string>() == uuid) {
                return true;
            }
        } else {
            if (key["name"].get<std::string>() == realname) {
                key["uuid"] = uuid;
                saveBanFile();
                return true;
            }
        }
    }
    return false;
}

bool isNameBanned(const std::string& realname) {
    for (auto& key : mBanList) {
        if (key.contains("name")) {
            if (key["name"].get<std::string>() == realname) {
                return true;
            }
        }
    }
    return false;
}

bool isUuidBanned(const std::string& uuid) {
    for (auto& key : mBanList) {
        if (key.contains("uuid")) {
            if (key["uuid"].get<std::string>() == uuid) {
                return true;
            }
        }
    }
    return false;
}

bool isIpBanned(const std::string& ip) {
    for (auto& key : mBanIpList) {
        if (key["ip"].get<std::string>() == ip) {
            return true;
        }
    }
    return false;
}

std::pair<std::string, std::string> getBannedInfo(const std::string& uuid) {
    for (auto& key : mBanList) {
        if (key.contains("uuid")) {
            if (key["uuid"].get<std::string>() == uuid) {
                auto reason  = key["reason"].get<std::string>();
                auto expires = key["expires"].get<std::string>();
                return {reason, expires};
            }
        }
    }
    return {"", ""};
}

std::pair<std::string, std::string> getBannedIpInfo(const std::string& ip) {
    for (auto& key : mBanIpList) {
        if (key["ip"].get<std::string>() == ip) {
            auto reason  = key["reason"].get<std::string>();
            auto expires = key["expires"].get<std::string>();
            return {reason, expires};
        }
    }
    return {"", ""};
}

bool banPlayer(const std::string& name, const std::string& opSource, int time, const std::string& reason) {
    if (isNameBanned(name)) {
        return false;
    }
    auto key       = nlohmann::json::object();
    auto endTime   = time < 0 ? "forever" : getExpiredTime(time);
    key["name"]    = name;
    key["reason"]  = reason;
    key["source"]  = opSource;
    key["expires"] = endTime;
    key["created"] = getExpiredTime();
    mBanList.push_back(key);
    saveBanFile();
    return true;
}

bool banIP(const std::string& ip, const std::string& opSource, int time, const std::string& reason) {
    if (!isIpBanned(ip)) {
        auto key       = nlohmann::json::object();
        auto endTime   = time < 0 ? "forever" : getExpiredTime(time);
        key["ip"]      = ip;
        key["reason"]  = reason;
        key["source"]  = opSource;
        key["expires"] = endTime;
        key["created"] = getExpiredTime();
        mBanIpList.push_back(key);
        saveBanIpFile();
        auto lastTime = endTime == "forever" ? tr("disconnect.forever") : endTime;
        auto msg      = tr("disconnect.ipIsBanned", {reason, lastTime});
        ll::service::getLevel()->forEachPlayer([&](Player& player) -> bool {
            auto ipAddress = getIP(player.getIPAndPort());
            if (ipAddress == ip) {
                player.disconnect(msg);
            }
            return true;
        });
        return true;
    }
    return false;
}

bool banOnlinePlayer(Player* pl, const std::string& opSource, int time, const std::string& reason) {
    auto uuid = pl->getUuid().asString();
    if (!isUuidBanned(uuid)) {
        auto info       = nlohmann::json::object();
        auto endTime    = time < 0 ? "forever" : getExpiredTime(time);
        info["uuid"]    = uuid;
        info["name"]    = pl->getRealName();
        info["source"]  = opSource;
        info["reason"]  = reason;
        info["expires"] = endTime;
        info["created"] = getExpiredTime();
        mBanList.push_back(info);
        saveBanFile();
        auto lastTime = endTime == "forever" ? tr("disconnect.forever") : endTime;
        auto msg      = tr("disconnect.isBanned", {reason, lastTime});
        pl->disconnect(msg);
        return true;
    }
    return false;
}

bool unbanPlayer(const std::string& name) {
    for (auto it = mBanList.begin(); it != mBanList.end(); ++it) {
        if (it.value()["name"].get<std::string>() == name) {
            mBanList.erase(it);
            --it;
            saveBanFile();
            return true;
        }
    }
    return false;
}

bool unbanIP(const std::string& ip) {
    for (auto it = mBanIpList.begin(); it != mBanIpList.end(); ++it) {
        if (it.value()["ip"].get<std::string>() == ip) {
            mBanIpList.erase(it);
            --it;
            saveBanIpFile();
            return true;
        }
    }
    return false;
}

void checkBanTime() {
    mtx.lock();
    for (auto it = mBanList.begin(); it != mBanList.end(); ++it) {
        auto endTime = it.value().at("expires").get<std::string>();
        if (isExpired(endTime)) {
            mBanList.erase(it);
            --it;
        }
    }
    saveBanFile();
    for (auto it = mBanIpList.begin(); it != mBanIpList.end(); ++it) {
        auto endTime = it.value().at("expires").get<std::string>();
        if (isExpired(endTime)) {
            mBanIpList.erase(it);
            --it;
        }
    }
    saveBanIpFile();
    mtx.unlock();
}

void checkBanTimeTask() {
    std::thread([&] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::minutes(1));
            checkBanTime();
        }
    }).detach();
}

void showBanPlayersList(CommandOutput& output) {
    if (mBanList.empty()) {
        return output.success(tr("command.banlist.noBans"));
    }
    for (auto& key : mBanList) {
        auto name        = key["name"].get<std::string>();
        auto source_key  = key["source"].get<std::string>();
        auto reason      = key["reason"].get<std::string>();
        auto endTime_key = key["expires"].get<std::string>();
        auto source      = source_key == "Console" ? tr("command.source.console") : source_key;
        auto endTime     = endTime_key == "forever" ? tr("disconnect.forever") : endTime_key;
        output.success(tr("command.banlist.players.showInfo", {name, source, reason, endTime}));
    }
}

void showBanIpsList(CommandOutput& output) {
    if (mBanIpList.empty()) {
        return output.success(tr("command.banlist.noBans"));
    }
    for (auto& key : mBanIpList) {
        auto ip          = key["ip"].get<std::string>();
        auto source_key  = key["source"].get<std::string>();
        auto reason      = key["reason"].get<std::string>();
        auto endTime_key = key["expires"].get<std::string>();
        auto source      = source_key == "Console" ? tr("command.source.console") : source_key;
        auto endTime     = endTime_key == "forever" ? tr("disconnect.forever") : endTime_key;
        output.success(tr("command.banlist.ips.showInfo", {ip, source, reason, endTime}));
    }
}

void listenEvent() {
    auto& eventBus = ll::event::EventBus::getInstance();
    eventBus.emplaceListener<ila::mc::ClientLoginAfterEvent>([](ila::mc::ClientLoginAfterEvent& event) {
        const auto& clientXuid = event.clientAuthXuid();
        if (clientXuid.empty()) {
            std::string msg = tr("disconnect.clientNotAuth");
            event.disConnectClient(msg);
        }
        auto uuid     = event.uuid().asString();
        auto realName = event.realName();
        if (isBanned(uuid, realName)) {
            auto info    = getBannedInfo(uuid);
            auto endtime = info.second == "forever" ? tr("disconnect.forever") : info.second;
            if (isExpired(info.second)) {
                unbanPlayer(realName);
            } else {
                auto msg = tr("disconnect.isBanned", {info.first, endtime});
                event.disConnectClient(msg);
            }
        }
        auto ipAddress = event.ip();
        if (isIpBanned(ipAddress)) {
            auto info    = getBannedIpInfo(ipAddress);
            auto endtime = info.second == "forever" ? tr("disconnect.forever") : info.second;
            if (isExpired(info.second)) {
                unbanIP(ipAddress);
            } else {
                auto msg = tr("disconnect.ipIsBanned", {info.first, endtime});
                event.disConnectClient(msg);
            }
        }
    });
}