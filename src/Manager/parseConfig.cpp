#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <functional>

#include "../../include/Components.hpp"
#include "../Component/gate/genericGate.hpp"
#include "../../include/paths.hpp"

#include "Manager.hpp"

namespace nts {
    void Manager::_parseGateConfig(const std::string& filename, std::map<std::string, bool>& paths) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        std::string line;
        std::getline(file, line);
        std::string gateName = line;

        std::getline(file, line);
        int pinNb = std::stoi(line.substr(6)); // Skip "PinNb "

        std::vector<std::tuple<
                std::string, // Internal component type
                std::string  // Internal component name
            >> componentsData;
        std::multimap<
                std::size_t,
                std::pair<std::string, std::size_t>
            > pinRefTable;
        std::string componentName, type;

        std::getline(file, line); // Skip "componentsData"
        while (std::getline(file, line) && line != "end") {
            std::istringstream iss(line);
            iss >> componentName >> type;

            componentsData.push_back({type, componentName});
        }

        std::vector<std::string> unparsedPaths;
        for (auto &entry : paths) {
            if (entry.second) continue;
            unparsedPaths.push_back(entry.first);
        }

        for (auto &data : componentsData) {
            std::string primaryPath = PRIMARY_GATES_PATH + std::get<0>(data) + ".nts.config";
            std::string secondaryPath = SECONDARY_GATES_PATH + std::get<0>(data) + ".nts.config";

            for (auto path : unparsedPaths) {
                if (path == primaryPath || path == secondaryPath) {
                    if (!paths[path]) {
                        _parseGateConfig(path, paths);
                    }
                }
            }
        }

        std::getline(file, line); // Skip "pinRefTable"
        while (std::getline(file, line) && line != "end") {
            std::istringstream iss(line);
            std::size_t pin;
            std::string componentName;
            std::size_t componentPin;
            iss >> pin >> componentName >> componentPin;
            pinRefTable.insert({pin, {componentName, componentPin}});
        }

        std::multimap<
                std::string, // Name
                std::tuple<
                    std::size_t, // Pin
                    std::string, // Other name
                    std::size_t> // Other pin
            > internalLink;
        internalLink.clear();

        while (std::getline(file, line) && line != "end") {
            if (line == "internalLink") {
                while (std::getline(file, line) && line != "end") {
                    std::istringstream iss(line);
                    std::string name;
                    std::size_t pin;
                    std::string otherName;
                    std::size_t otherPin;
                    iss >> name >> pin >> otherName >> otherPin;
                    internalLink.insert({name, {pin, otherName, otherPin}});
                }
            }
        }

        paths[filename] = true;
        _gates[gateName] = [this, pinNb, componentsData, pinRefTable, internalLink](const std::string &label) {
            return new GenericGate(this, label, pinNb, componentsData, pinRefTable, internalLink);
        };
    }
}
