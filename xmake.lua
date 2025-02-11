add_rules("mode.debug", "mode.release", "mode.releasedbg")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")
add_repositories("groupmountain-repo https://github.com/GroupMountain/xmake-repo.git")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

add_requires("levilamina")
add_requires("levibuildscript")
add_requires("gmlib")

target("GMBlacklist") -- Change this to your plugin name.
    add_cxflags(
        "/EHa",
        "/utf-8"
    )
    add_defines(
        "_HAS_CXX23=1" -- To enable C++23 features
    )
    add_files(
        "src/**.cpp"
    )
    add_includedirs(
        "src"
    )
    add_packages(
        "levilamina",
		"gmlib"
    )
    add_defines(
        "NOMINMAX", 
        "UNICODE", 
        "_HAS_CXX17",
        "_HAS_CXX20",
        "_HAS_CXX23"
    )
    add_rules("@levibuildscript/linkrule")
    add_rules("@levibuildscript/modpacker")
    set_exceptions("none") -- To avoid conflicts with /EHa
    set_kind("shared")
    set_languages("cxx23")
    set_symbols("debug")
