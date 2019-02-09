#pragma once

#include <map>
#include <optional>
#include <string>
#include <type_traits>

namespace utility {
    class Config {
    public:
        Config(const std::string& filePath = "");

        bool load(const std::string& filePath);
        bool save(const std::string& filePath);

        // Helper for differentiating between boolean and arithmetic values.
        template <typename T>
        using is_arithmetic_not_bool = std::bool_constant<std::is_arithmetic_v<T> && !std::is_same_v<T, bool>>;

        template <typename T>
        static constexpr bool is_arithmetic_not_bool_v = is_arithmetic_not_bool<T>::value;

        // get method for arithmetic types.
        template <typename T>
        std::optional<typename std::enable_if_t<is_arithmetic_not_bool_v<T>, T>> get(const std::string& key) const {
            auto value = get(key);

            if (!value) {
                return {};
            }

            // Use the correct conversion function based on the type.
            if constexpr (std::is_integral_v<T>) {
                if constexpr (std::is_unsigned_v<T>) {
                    return (T)std::stoul(*value);
                }
                else {
                    return (T)std::stol(*value);
                }
            }
            else {
                return (T)std::stod(*value);
            }
        }

        // get method for boolean types.
        template <typename T>
        std::optional<typename std::enable_if_t<std::is_same_v<T, bool>, T>> get(const std::string& key) const {
            auto value = get(key);

            if (!value) {
                return {};
            }

            if (*value == "true") {
                return true;
            }

            if (*value == "false") {
                return false;
            }

            return {};
        }

        // get method for strings.
        std::optional<std::string> get(const std::string& key) const;

        // set method for arithmetic types.
        template <typename T>
        void set(const std::string& key, typename std::enable_if_t<is_arithmetic_not_bool_v<T>, T> value) {
            set(key, std::to_string(value));
        }

        // set method for boolean types.
        template <typename T>
        void set(const std::string& key, typename std::enable_if_t<std::is_same_v<T, bool>, T> value) {
            if (value) {
                set(key, "true");
            }
            else {
                set(key, "false");
            }
        }

        // set method for strings.
        void set(const std::string& key, const std::string& value);

        auto& getKeyValues() {
            return m_keyValues;
        }

        const auto& getKeyValues() const {
            return m_keyValues;
        }

    private:
        std::map<std::string, std::string> m_keyValues;
    };
}
