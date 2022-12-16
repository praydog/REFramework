#include <filesystem>
#include <fstream>

#include <json.hpp>
#include <spdlog/spdlog.h>

#include "../ScriptRunner.hpp"

#include "Json.hpp"

namespace api::json {
using json = nlohmann::json;
namespace fs = std::filesystem;

namespace detail {
json encode_any(sol::object obj) {
    switch (obj.get_type()) {
    case sol::type::nil:
        return json{};
    case sol::type::boolean:
        return obj.as<bool>();
    case sol::type::number: {
        obj.push();
        if (lua_isinteger(obj.lua_state(), -1)) {
            obj.pop();
            return obj.as<int64_t>();
        }
        
        obj.pop();
        
        return obj.as<double>();
    }
    case sol::type::string:
        return obj.as<std::string>();
    case sol::type::table: {
        auto table = obj.as<sol::table>();
        bool is_array = true;
        auto i = 1;

        for (auto& kvp : table) {
            if (kvp.first.get_type() != sol::type::number || kvp.first.as<int>() != i++) {
                is_array = false;
                break;
            }
        }

        json j{};

        for (auto& kvp : table) {
            if (is_array) {
                j.push_back(encode_any(kvp.second));
            } else {
                j[kvp.first.as<std::string>()] = encode_any(kvp.second);
            }
        }

        return j;
    }
    default:
        return json{};
    }
}

sol::object decode_any(sol::this_state l, const json& j) {
    using value_t = nlohmann::detail::value_t;

    auto state = sol::state_view{l};

    switch (j.type()) {
    case value_t::null:
        return sol::nil;
    case value_t::boolean:
        return sol::make_object(l, j.get<bool>());
    case value_t::number_integer:
        return sol::make_object(l, j.get<int64_t>());
    case value_t::number_unsigned:
        return sol::make_object(l, j.get<uint64_t>());
    case value_t::number_float:
        return sol::make_object(l, j.get<double>());
    case value_t::string:
        return sol::make_object(l, j.get<std::string>());
    case value_t::array: {
        sol::table t = state.create_table();
        for (size_t i = 0; i < j.size(); ++i) {
            t[i + 1] = decode_any(l, j[i]);
        }
        return t;
    }
    case value_t::object: {
        sol::table t = state.create_table();
        for (auto it = j.begin(); it != j.end(); ++it) {
            t[it.key()] = decode_any(l, it.value());
        }
        return t;
    }
    default:
        return sol::nil;
    }
}

fs::path get_datadir() {
    return REFramework::get_persistent_dir() / "reframework" / "data";
}
} // namespace detail

sol::object load_string(sol::this_state l, const std::string& s) try {
    const auto j = json::parse(s);
    return detail::decode_any(l, j);
} catch (const std::exception& e) {
    return sol::nil;
}

std::string dump_string(sol::object obj, sol::object indent_obj) try {
    int indent = -1;

    if (indent_obj.get_type() == sol::type::number) {
        indent = indent_obj.as<int>();
    }

    return detail::encode_any(obj).dump(indent);
} catch (const std::exception& e) {
    return "";
}

sol::object load_file(sol::this_state l, const std::string& filepath) try {
    if (filepath.find("..") != std::string::npos) {
        throw std::runtime_error{"json.load_file does not allow access to parent directories"};
    }

    if (std::filesystem::path(filepath).is_absolute()) {
        throw std::runtime_error{"json.load_file does not allow absolute paths"};
    }

    const auto j = json::parse(std::ifstream{detail::get_datadir() / filepath});
    return detail::decode_any(l, j);
} catch (const json::exception& e) {
    spdlog::error("[JSON] Failed to load file {}: {}", filepath, e.what());
    return sol::nil;
}

bool dump_file(const std::string& filepath, sol::object obj, sol::object indent_obj) try {
    int indent = 4;

    if (indent_obj.get_type() == sol::type::number) {
        indent = indent_obj.as<int>();
    }

    if (filepath.find("..") != std::string::npos) {
        throw std::runtime_error{"json.dump_file does not allow access to parent directories"};
    }

    if (std::filesystem::path(filepath).is_absolute()) {
        throw std::runtime_error{"json.dump_file does not allow absolute paths"};
    }

    auto path = detail::get_datadir() / filepath;

    fs::create_directories(path.parent_path());

    std::ofstream f{path};
    f << detail::encode_any(obj).dump(indent);
    return true;
} catch (const std::exception& e) {
    spdlog::error("[JSON] Failed to dump file {}: {}", filepath, e.what());
    return false;
}

} // namespace api::json

void bindings::open_json(ScriptState* s) {
    auto& lua = s->lua();
    auto json = lua.create_table();

    json["load_string"] = api::json::load_string;
    json["dump_string"] = api::json::dump_string;
    json["load_file"] = api::json::load_file;
    json["dump_file"] = api::json::dump_file;
    lua["json"] = json;
}