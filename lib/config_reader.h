#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace tape_sorter {

    class ConfigReader {
    public:
        class Value {
        public:
            Value() = default;

            explicit Value(std::string value);

            [[nodiscard]] std::chrono::milliseconds AsMilliseconds() const;
            [[nodiscard]] std::chrono::seconds AsSeconds() const;
            [[nodiscard]] std::filesystem::path AsPath() const;
            [[nodiscard]] std::string AsString() const;
            [[nodiscard]] int AsInt() const;
            [[nodiscard]] long long AsLongLong() const;
            [[nodiscard]] long AsLong() const;
            [[nodiscard]] double AsDouble() const;

        private:
            std::string value_;
        };

        explicit ConfigReader(const char *path);
        explicit ConfigReader(const std::string &path);

        void ReadConfig();

        Value operator[](const std::string &field_name);

    private:
        std::filesystem::path path_;
        std::unordered_map<std::string, Value> fields_;
    };

}  // namespace tape_sorter