#pragma once
#include "Config.h"
#include "Global.h"

namespace GMBlacklist {

using namespace GMLIB::Files::I18n;

class Entry {

public:
    static std::unique_ptr<Entry>& getInstance();

    Entry(ll::mod::NativeMod& self) : mSelf(self) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    /// @return True if the plugin is loaded successfully.
    bool load();

    /// @return True if the plugin is enabled successfully.
    bool enable();

    /// @return True if the plugin is disabled successfully.
    bool disable();

    // TODO: Implement this method if you need to unload the plugin.
    // /// @return True if the plugin is unloaded successfully.
    // bool unload();

    Config& getConfig();

    JsonI18n& getI18n();

private:
    ll::mod::NativeMod&     mSelf;
    std::optional<Config>   mConfig;
    std::optional<JsonI18n> mI18n;
};

} // namespace GMBlacklist
