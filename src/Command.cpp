#include "Entry.h"
#include "Global.h"

struct BanParam {
    std::string                        name;
    ll::command::Optional<int>         time;
    ll::command::Optional<std::string> reason;
};

void banExecute(CommandOrigin const& origin, CommandOutput& output, BanParam const& param) {
    std::string source = "Console";
    int         time   = -1;
    std::string reason = "disconnect.defaultReason"_tr();
    auto        type   = origin.getOriginType();
    if (type == CommandOriginType::DedicatedServer || type == CommandOriginType::Player) {
        if (type == CommandOriginType::Player) {
            auto pl = (Player*)origin.getEntity();
            source  = pl->getRealName();
        }
        if (param.time.has_value()) {
            time = param.time;
            if (time < 1) {
                return output.error("command.error.invalidTime"_tr());
            }
        }
        if (param.reason.has_value()) {
            reason = param.reason;
            if (reason.empty()) {
                reason = "disconnect.defaultReason"_tr();
            }
        }
        auto name = param.name;
        auto pl   = ll::service::getLevel()->getPlayer(name);
        bool res  = false;
        if (pl) {
            res = banOnlinePlayer(pl, source, time, reason);
        } else {
            res = banPlayer(name, source, time, reason);
        }
        if (res) {
            std::string endTime = time < 0 ? "disconnect.forever"_tr() : getExpiredTime(time);
            return output.success("command.ban.success"_tr(name, endTime));
        }
        return output.error("command.ban.isBanned"_tr(name));
    }
    return output.error("command.error.invalidCommandOrigin"_tr());
}

void RegBanCmd() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        "ban",
        "command.ban.desc"_tr(),
        (CommandPermissionLevel)GMBlacklist::Entry::getInstance().getConfig().CommandPermissionLevel
    );
    cmd.overload<BanParam>().required("name").optional("time").optional("reason").execute(
        [](CommandOrigin const& origin, CommandOutput& output, BanParam const& param) {
            return banExecute(origin, output, param);
        }
    );
    cmd.overload<BanParam>().required("name").optional("reason").execute(
        [](CommandOrigin const& origin, CommandOutput& output, BanParam const& param) {
            return banExecute(origin, output, param);
        }
    );
}

struct UnbanParam {
    std::string name;
};

void RegUnbanCmd() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        "unban",
        "command.unban.desc"_tr(),
        (CommandPermissionLevel)GMBlacklist::Entry::getInstance().getConfig().CommandPermissionLevel
    );
    cmd.overload<UnbanParam>().required("name").execute(
        [](CommandOrigin const& origin, CommandOutput& output, UnbanParam const& param) {
            auto type = origin.getOriginType();
            if (type == CommandOriginType::DedicatedServer || type == CommandOriginType::Player) {
                auto name = param.name;
                auto res  = unbanPlayer(name);
                if (res) {
                    return output.success("command.unban.success"_tr(name));
                }
                return output.error("command.unban.notBanned"_tr(name));
            }
            return output.error("command.error.invalidCommandOrigin"_tr());
        }
    );
}

struct BanIpParam {
    std::string                        ip;
    ll::command::Optional<int>         time;
    ll::command::Optional<std::string> reason;
};

void banIpExecute(CommandOrigin const& origin, CommandOutput& output, BanIpParam const& param) {
    std::string source = "Console";
    int         time   = -1;
    std::string reason = "disconnect.defaultReason"_tr();
    auto        type   = origin.getOriginType();
    if (type == CommandOriginType::DedicatedServer || type == CommandOriginType::Player) {
        if (type == CommandOriginType::Player) {
            auto pl = (Player*)origin.getEntity();
            source  = pl->getRealName();
        }
        if (param.time.has_value()) {
            time = param.time;
            if (time < 1) {
                return output.error("command.error.invalidTime"_tr());
            }
        }
        if (param.reason.has_value()) {
            reason = param.reason;
            if (reason.empty()) {
                reason = "disconnect.defaultReason"_tr();
            }
        }
        auto ip  = param.ip;
        auto res = banIP(ip, source, time, reason);
        if (res) {
            std::string endTime = time < 0 ? "disconnect.forever"_tr() : getExpiredTime(time);
            return output.success("command.banip.success"_tr(ip, endTime));
        }
        return output.error("command.banip.isBanned"_tr(ip));
    }
    return output.error("command.error.invalidCommandOrigin"_tr());
}

void RegBanIpCmd() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        "banip",
        "command.banip.desc"_tr(),
        (CommandPermissionLevel)GMBlacklist::Entry::getInstance().getConfig().CommandPermissionLevel
    );
    ll::service::getCommandRegistry()->registerAlias("banip", "ban-ip");
    cmd.overload<BanIpParam>().required("ip").optional("time").optional("reason").execute(
        [](CommandOrigin const& origin, CommandOutput& output, BanIpParam const& param) {
            return banIpExecute(origin, output, param);
        }
    );
    cmd.overload<BanIpParam>().required("ip").optional("reason").execute(
        [](CommandOrigin const& origin, CommandOutput& output, BanIpParam const& param) {
            return banIpExecute(origin, output, param);
        }
    );
}

struct UnbanIpParam {
    std::string ip;
};

void RegUnbanipCmd() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        "unbanip",
        "command.unbanip.desc"_tr(),
        (CommandPermissionLevel)GMBlacklist::Entry::getInstance().getConfig().CommandPermissionLevel
    );
    cmd.overload<UnbanIpParam>().required("ip").execute(
        [](CommandOrigin const& origin, CommandOutput& output, UnbanIpParam const& param) {
            auto type = origin.getOriginType();
            if (type == CommandOriginType::DedicatedServer || type == CommandOriginType::Player) {
                auto ip  = param.ip;
                auto res = unbanIP(ip);
                if (res) {
                    return output.success("command.unbanip.success"_tr(ip));
                }
                return output.error("command.unbanip.notBanned"_tr(ip));
            }
            return output.error("command.error.invalidCommandOrigin"_tr());
        }
    );
}

struct BanListParam {
    enum class Mode { players, ips };
    ll::command::SoftEnum<Mode> mode;
};

void RegBanlistCmd() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        "banlist",
        "command.banlist.desc"_tr(),
        (CommandPermissionLevel)GMBlacklist::Entry::getInstance().getConfig().CommandPermissionLevel
    );
    cmd.overload<BanListParam>().optional("mode").execute(
        [](CommandOrigin const& origin, CommandOutput& output, BanListParam const& param) {
            auto type = origin.getOriginType();
            if (type == CommandOriginType::DedicatedServer || type == CommandOriginType::Player) {
                if (param.mode == "ips") {
                    return showBanIpsList(output);
                }
                return showBanPlayersList(output);
            }
            return output.error("command.error.invalidCommandOrigin"_tr());
        }
    );
}

void RegisterCommands() {
    RegBanCmd();
    RegUnbanCmd();
    RegBanIpCmd();
    RegUnbanipCmd();
    RegBanlistCmd();
}