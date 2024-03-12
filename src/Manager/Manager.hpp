#pragma once

#include <cstring>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <functional>
#include <signal.h>
#include <filesystem>
#include <cmath>

#include "../../include/Components.hpp"

namespace nts {
    enum ParserStage {
        UNDEFINED = -1,
        UNCHANGED = 0,
        CHIPSET = 1,
        LINKS = 2
    };

    class Manager {
        private:
            std::map<std::string, IComponent *> _components;
            std::size_t _currentTick;

            std::map<
                std::string, // label
                std::tuple<
                    std::size_t, // pinNb
                    std::vector<std::size_t>, // input pins
                    std::vector<std::size_t>, // output pins
                    std::vector<std::vector<nts::Tristate> // truth table
            >>> _componentTruthTables;
            std::map<std::string, std::function<IComponent*(const std::string&)>> _gates; // Gates factory

            void _handleCommand(const std::string &line);
            void _interpretLine(const std::string &line);

            void _stageChipsetHandler(const std::string &line);
            void _stageLinksHandler(const std::string &line, bool remove = false);
            void _parserLoop(std::ifstream &fs);

            void _checkRun() const;
            bool _addComponent(const std::string &label, IComponent *component);

            void _displayPrint(const std::string &title, ComponentType type);
            void _dumpPrint(const std::string &title, ComponentType type);
            static void _help();
            static void _commandHelp();

            void _loop();

        public:
            Manager() : _currentTick(0) {};
            ~Manager();

            bool createComponent(const std::string &type, const std::string &label);
            IComponent *getComponent(const std::string &label);

            void addLink(
                const std::string &source,
                std::size_t sourcePin,
                const std::string &dest,
                std::size_t otherPin);
            void removeLink(
                const std::string &source,
                std::size_t sourcePin,
                const std::string &dest,
                std::size_t otherPin);

            void dump(bool = true, bool = true, bool = true, bool = false);

            void simulate() { this->simulate(++_currentTick); };
            void simulate(std::size_t tick);
            void initConstantsLinks();

            IComponent * factory(
                const std::string &type,
                const std::string &label);

            void initializeTruthTables(const std::string &folder);
            void _generateTruthTableFromFile(const std::string &filename);
            void initGates(const std::string &folder);
            void _parseGateConfig(const std::string& filename, std::map<std::string, bool>& paths);

            static nts::ParserStage stageChecker(std::ifstream &fs, std::string &line, nts::ParserStage &stage);
            void preParse(int ac, char **av);
            void parser(int ac, char **av);

            void display();
            void run();
            void clearComponents();
    };
}
