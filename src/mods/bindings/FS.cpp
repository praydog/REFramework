#include <filesystem>
#include <regex>

#include "../ScriptRunner.hpp"

#include "FS.hpp"

namespace api::fs {
using namespace std::filesystem;

namespace detail {
path get_datadir() {
    std::string modpath{};

    modpath.resize(1024, 0);
    modpath.resize(GetModuleFileName(nullptr, modpath.data(), modpath.size()));

    auto datadir = path{modpath}.parent_path() / "reframework" / "data";

    create_directories(datadir);

    return datadir;
}
}

sol::table glob(sol::this_state l, const char* filter) {
    sol::state_view state{l};
    std::regex filter_regex{filter};
    auto results = state.create_table();
    auto datadir = detail::get_datadir();
    auto i = 0;

    for (const auto& entry : recursive_directory_iterator{datadir}) {
        if (!entry.is_regular_file()) {
            continue;
        }

        auto relpath = relative(entry.path(), datadir).string();

        if (std::regex_match(relpath, filter_regex)) {
            results[++i] = relpath;
        }
    }

    return results;
}
}

void bindings::open_fs(ScriptState* s) {
    auto& lua = s->lua();
    auto fs = lua.create_table();

    fs["glob"] = api::fs::glob;
    lua["fs"] = fs;
}