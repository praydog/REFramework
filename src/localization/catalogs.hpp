#pragma once
// Generated from languages.json and catalogs; edit JSON sources, not this file.
#include <array>
#include "zh_CN.hpp"
#include "zh_TW.hpp"
#include "ja_JP.hpp"
#include "ko_KR.hpp"
namespace reframework::localization {
// Numeric values are persisted in REFrameworkConfig_UILanguage; append new languages.
enum class Language : int {
    English = 0,
    SimplifiedChinese = 1,
    TraditionalChinese = 2,
    Japanese = 3,
    Korean = 4,
};
inline constexpr auto default_language = Language::English;
using Catalog = std::unordered_map<std::string_view, const char*>;
struct LanguageInfo { const char* tag; const char* name; const Catalog* catalog; };
inline const std::array languages{
    LanguageInfo{"en", "English", nullptr},
    LanguageInfo{"zh-CN", "\xe7\xae\x80\xe4\xbd\x93\xe4\xb8\xad\xe6\x96\x87", &zh_cn},
    LanguageInfo{"zh-TW", "\xe7\xb9\x81\xe9\xab\x94\xe4\xb8\xad\xe6\x96\x87", &zh_tw},
    LanguageInfo{"ja-JP", "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e", &ja_jp},
    LanguageInfo{"ko-KR", "\xed\x95\x9c\xea\xb5\xad\xec\x96\xb4", &ko_kr},
};
// All catalogs share this key set; use it to keep English widget IDs stable too.
inline const auto& source_catalog = zh_cn;
} // namespace reframework::localization
