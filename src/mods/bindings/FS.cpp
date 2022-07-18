#include <filesystem>
#include <regex>
#include <fstream>

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

void write(sol::this_state l, const std::string& filepath, const std::string& data) {
    if (filepath.find("..") != std::string::npos) {
        throw std::runtime_error{"fs.write does not allow access to parent directories"};
    }

    auto path = detail::get_datadir() / filepath;

    std::ofstream file{path};

    file << data;
}

std::string read(sol::this_state l, const std::string& filepath) {
    if (filepath.find("..") != std::string::npos) {
        throw std::runtime_error{"fs.read does not allow access to parent directories"};
    }

    auto path = detail::get_datadir() / filepath;

    if (!exists(path)) {
        return "";
    }

    std::ifstream file{path};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
}

void bindings::open_fs(ScriptState* s) {
    auto& lua = s->lua();
    auto fs = lua.create_table();

    fs["glob"] = api::fs::glob;
    fs["write"] = api::fs::write;
    fs["read"] = api::fs::read;
    lua["fs"] = fs;
}