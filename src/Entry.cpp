#include "Entry.h"
#include "Global.h"
#include "resource.h"

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
        getSelf().getLogger().error("permission.error.invalidLevel"_tr());
    }
    if (commandPermissionLevel == 0) {
        getSelf().getLogger().warn("permission.warning.dangerousLevel"_tr());
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
    getSelf().getLogger().info("Author: KobeBryant114514");
    getSelf().getLogger().info("Repository: https://github.com/GroupMountain/GMBlacklist");
    return true;
}

bool Entry::disable() /*NOLINT*/ { return true; }

Config& Entry::getConfig() { return mConfig.value(); }

JsonI18n& Entry::getI18n() { return mI18n.value(); }

} // namespace GMBlacklist

LL_REGISTER_MOD(GMBlacklist::Entry, GMBlacklist::Entry::getInstance());
