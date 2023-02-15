#include "config_reader.h"

#include <fstream>
#include <utility>

namespace tape_sorter {

    ConfigReader::ConfigReader(const char *path)
        : path_(path) {}

    ConfigReader::ConfigReader(const std::string &path)
        : path_(path) {}


    void ConfigReader::ReadConfig() {
        std::ifstream fin(path_);

        if (!fin.is_open()) {
            throw std::runtime_error("Could not open config file_");
        }

        std::string field_name;
        std::string dummy;
        while (fin >> field_name) {
            std::string value;
            fin >> dummy >> value;

            fields_[field_name] = Value(value);
        }

        fin.close();
    }

    ConfigReader::Value ConfigReader::operator[](const std::string &field_name) {
        return fields_[field_name];
    }

    ConfigReader::Value::Value(std::string value) : value_(std::move(value)) {}

    [[nodiscard]] std::chrono::milliseconds ConfigReader::Value::AsMilliseconds() const {
        return std::chrono::milliseconds(std::stol(value_));
    }

    [[nodiscard]] std::chrono::seconds ConfigReader::Value::AsSeconds() const {
        return std::chrono::seconds(std::stol(value_));
    }

    [[nodiscard]] std::filesystem::path ConfigReader::Value::AsPath() const {
        return value_;
    }

    [[nodiscard]] std::string ConfigReader::Value::AsString() const {
        return value_;
    }

    [[nodiscard]] int ConfigReader::Value::AsInt() const {
        return std::stoi(value_);
    }

    [[nodiscard]] long long ConfigReader::Value::AsLongLong() const {
        return std::stoll(value_);
    }

    [[nodiscard]] long ConfigReader::Value::AsLong() const {
        return std::stol(value_);
    }

    [[nodiscard]] double ConfigReader::Value::AsDouble() const {
        return std::stod(value_);
    }

}  // namespace tape_sorter