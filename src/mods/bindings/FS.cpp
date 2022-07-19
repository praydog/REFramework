#include <filesystem>
#include <regex>
#include <fstream>
#include <filesystem>

#include "../ScriptRunner.hpp"

#include "FS.hpp"

namespace fs = std::filesystem;

namespace api::fs {
namespace detail {
::fs::path get_datadir() {
    std::string modpath{};

    modpath.resize(1024, 0);
    modpath.resize(GetModuleFileName(nullptr, modpath.data(), modpath.size()));

    auto datadir = ::fs::path{modpath}.parent_path() / "reframework" / "data";

    ::fs::create_directories(datadir);

    return datadir;
}
}

sol::table glob(sol::this_state l, const char* filter) {
    sol::state_view state{l};
    std::regex filter_regex{filter};
    auto results = state.create_table();
    auto datadir = detail::get_datadir();
    auto i = 0;

    for (const auto& entry : ::fs::recursive_directory_iterator{datadir}) {
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
        //throw sol::error{"fs.write does not allow access to parent directories"};
        lua_pushstring(l, "fs.write does not allow access to parent directories");
        lua_error(l);
    }

    if (std::filesystem::path(filepath).is_absolute()) {
        lua_pushstring(l, "fs.write does not allow the use of absolute paths");
        lua_error(l);
    }

    auto path = detail::get_datadir() / filepath;

    ::fs::create_directories(path.parent_path());

    std::ofstream file{path};

    file << data;
}

std::string read(sol::this_state l, const std::string& filepath) {
    if (filepath.find("..") != std::string::npos) {
        //throw sol::error{"fs.read does not allow access to parent directories"};
        lua_pushstring(l, "fs.read does not allow access to parent directories");
        lua_error(l);
    }

    if (std::filesystem::path(filepath).is_absolute()) {
        lua_pushstring(l, "fs.read does not allow the use of absolute paths");
        lua_error(l);
    }

    auto path = detail::get_datadir() / filepath;

    if (!exists(path)) {
        return "";
    }

    ::fs::create_directories(path.parent_path());

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

    lua.open_libraries(sol::lib::io);

    // Replace the io functions with safe versions that can't be used to access parent directories and can't be used to open files outside of the data directory.
    auto io = lua["io"];
    sol::function old_open = io["open"];

    io["open"] = [=](sol::this_state l, const std::string& filepath, sol::object mode) -> sol::object {
        if (filepath.find("..") != std::string::npos) {
            lua_pushstring(l, "io.open does not allow access to parent directories");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        if (std::filesystem::path(filepath).is_absolute()) {
            lua_pushstring(l, "io.open does not allow the use of absolute paths");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        auto path = api::fs::detail::get_datadir() / filepath;

        fs::create_directories(path.parent_path());

        return old_open(path.string().c_str(), mode);
    };

    /*sol::function old_popen = io["popen"];

    io["popen"] = [=](sol::this_state l, const std::string& filepath, sol::object mode) -> sol::object {
        if (filepath.find("..") != std::string::npos) {
            lua_pushstring(l, "io.popen does not allow access to parent directories");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        auto path = api::fs::detail::get_datadir() / filepath;
        auto warning = (std::stringstream{} << "A script wants to run an executable named \"reframework/data/" << filepath << "\". Allow this?").str();

        auto ret = MessageBoxA(nullptr, warning.c_str(), "REFramework", MB_YESNO | MB_ICONWARNING);

        if (ret == IDYES) {
            return old_popen(path.string().c_str(), mode);
        }

        return sol::make_object(l, sol::nil);
    };*/

    io["popen"] = sol::make_object(lua, sol::nil); // on second thought, I don't want to allow this. If someone really wants this functionality, they can just make a C++ plugin and use the C++ API.

    // These functions can take nil as the first argument and they will return the default filehandle associated with stdin, stdout, or stderr.
    // So they should be safe in that respect.
    sol::function old_lines = io["lines"];

    io["lines"] = [=](sol::this_state l, sol::object filepath_or_nil) -> sol::object {
        if (filepath_or_nil.is<sol::nil_t>()) {
            return old_lines(l, filepath_or_nil);
        }

        if (!filepath_or_nil.is<std::string>()) {
            lua_pushstring(l, "io.lines: expected a string or nil as the first argument");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        auto filepath = filepath_or_nil.as<std::string>();

        if (filepath.find("..") != std::string::npos) {
            //throw sol::error{"io.lines does not allow access to parent directories"};
            lua_pushstring(l, "io.lines does not allow access to parent directories");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        if (std::filesystem::path(filepath).is_absolute()) {
            lua_pushstring(l, "io.lines does not allow the use of absolute paths");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        auto path = api::fs::detail::get_datadir() / filepath;

        ::fs::create_directories(path.parent_path());

        return old_lines(path.string().c_str());
    };

    sol::function old_input = io["input"];

    io["input"] = [=](sol::this_state l, sol::object filepath_or_nil) -> sol::object {
        if (filepath_or_nil.is<sol::nil_t>()) {
            return old_input(l, filepath_or_nil);
        }

        if (!filepath_or_nil.is<std::string>()) {
            lua_pushstring(l, "io.input: expected a string or nil as the first argument");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        auto filepath = filepath_or_nil.as<std::string>();

        if (filepath.find("..") != std::string::npos) {
            lua_pushstring(l, "io.input: does not allow access to parent directories");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        if (std::filesystem::path(filepath).is_absolute()) {
            lua_pushstring(l, "io.input does not allow the use of absolute paths");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        auto path = api::fs::detail::get_datadir() / filepath;

        ::fs::create_directories(path.parent_path());

        return old_input(path.string().c_str());
    };

    sol::function old_output = io["output"];

    io["output"] = [=](sol::this_state l, sol::object filepath_or_nil) -> sol::object {
        if (filepath_or_nil.is<sol::nil_t>()) {
            return old_output(l, filepath_or_nil);
        }

        if (!filepath_or_nil.is<std::string>()) {
            lua_pushstring(l, "io.output: expected a string or nil as the first argument");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        auto filepath = filepath_or_nil.as<std::string>();

        if (filepath.find("..") != std::string::npos) {
            lua_pushstring(l, "io.output: does not allow access to parent directories");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        if (std::filesystem::path(filepath).is_absolute()) {
            lua_pushstring(l, "io.output does not allow the use of absolute paths");
            lua_error(l);
            return sol::make_object(l, sol::nil);
        }

        auto path = api::fs::detail::get_datadir() / filepath;

        ::fs::create_directories(path.parent_path());

        return old_output(path.string().c_str());
    };
}