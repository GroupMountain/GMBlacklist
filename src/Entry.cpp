#include "Entry.h"
#include "Global.h"
#include "Language.h"

namespace GMBlacklist {

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

bool Entry::load() /*NOLINT*/ { return true; }

bool Entry::enable() {
    mConfig.emplace();
    if (!ll::config::loadConfig(*mConfig, getSelf().getConfigDir() / u8"config.json")) {
        ll::config::saveConfig(*mConfig, getSelf().getConfigDir() / u8"config.json");
    }
    auto& commandPermissionLevel = getConfig().CommandPermissionLevel;
    if (commandPermissionLevel < 0 || commandPermissionLevel > 4) {
        commandPermissionLevel = 4;
        getSelf().getLogger().error(tr("permission.error.invalidLevel"));
    }
    if (commandPermissionLevel == 0) {
        getSelf().getLogger().warn(tr("permission.warning.dangerousLevel"));
    }
    initDataFile();
    mI18n.emplace(getSelf().getLangDir(), mConfig->language);
    mI18n->updateOrCreateLanguage("en_US", en_US);
    mI18n->updateOrCreateLanguage("zh_CN", zh_CN);
    mI18n->loadAllLanguages();
    mI18n->chooseLanguage(mConfig->language); // maybe
    checkBanTimeTask();
    RegisterCommands();
    listenEvent();
    getSelf().getLogger().info("GMBlacklist Loaded!");
    getSelf().getLogger().info("Author: Tsubasa6848");
    getSelf().getLogger().info("Repository: https://github.com/GroupMountain/GMBlacklist");
    return true;
}

bool Entry::disable() /*NOLINT*/ { return true; }

Config& Entry::getConfig() { return mConfig.value(); }

JsonI18n& Entry::getI18n() { return mI18n.value(); }

} // namespace GMBlacklist

LL_REGISTER_MOD(GMBlacklist::Entry, GMBlacklist::Entry::getInstance());

std::string tr(std::string const& key, std::vector<std::string> const& data) {
    return GMBlacklist::Entry::getInstance().getI18n().get(key, data);
}
