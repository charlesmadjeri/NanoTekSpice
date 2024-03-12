#include "Manager.hpp"

#include "../Component/gate/genericGate.hpp"

namespace nts {

    bool g_sigintCaught = false;

    /* Destructor */

    Manager::~Manager() {
        for (auto &component : _components) {
            delete component.second;
        }
    }

    /* Component management */

    bool Manager::createComponent(const std::string &type, const std::string &label) {
        return this->_addComponent(label, this->factory(type, label));
    }

    IComponent *Manager::getComponent(const std::string &label) {
        if (_components.find(label) == _components.end()) {
            throw CustomError("Component not found: " + label);
        }
        return _components[label];
    }

    bool Manager::_addComponent(const std::string &label, IComponent *component) {
        if (component == nullptr)
            throw CustomError("Creation went wrong.");
        if (_components.find(label) != _components.end())
            return false;
        _components[label] = std::move(component);
        return true;
    }

    IComponent *Manager::factory(const std::string &type, const std::string &label) {
        std::function<std::string(std::string)> toUpperCase = [](std::string str) {
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
            return str;
        };

        std::map<std::string, std::function<IComponent*(const std::string&)>> special = {
                {"input", [](const std::string &label) { return new InputComp(label); }},
                {"output", [](const std::string &label) { return new OutputComp(label); }},
                {"clock", [](const std::string &label) { return new Clock(label); }},
                {"true", [](const std::string &label) { return new TrueConst(label); }},
                {"false", [](const std::string &label) { return new FalseConst(label); }},
                {"logger", [](const std::string &label) { return new Logger(label); }}
        };

        if (special.find(type) != special.end()) {
            return special[type](label);
        }
        if (_gates.find(type) != _gates.end()) {
            return _gates[type](label);
        }
        if (_componentTruthTables.find(toUpperCase(type)) == _componentTruthTables.end()) {
            throw CustomError("No initializer found for \'" + type + "\' during creation.");
        }
        return new Chipset(
                std::get<0>(_componentTruthTables[toUpperCase(type)]), // pinNb
                label,
                ComponentType::Standard,
                std::get<3>(_componentTruthTables[toUpperCase(type)]), // truth table
                std::get<1>(_componentTruthTables[toUpperCase(type)]), // input pins
                std::get<2>(_componentTruthTables[toUpperCase(type)])); // output pins
    }

    void Manager::addLink(
            const std::string &source,
            std::size_t sourcePin,
            const std::string &dest,
            std::size_t otherPin
    ) { // This function DOES NOT link back if both components are outputs
        _components[dest]->setLink(otherPin, *_components[source], sourcePin);
        if (_components[source]->getType() == ComponentType::Output && _components[dest]->getType() == ComponentType::Output)
            return;
        _components[source]->setLink(sourcePin, *_components[dest], otherPin);
    }

    void Manager::removeLink(
        const std::string &source,
        std::size_t sourcePin,
        const std::string &dest,
        std::size_t otherPin
    ) {
        _components[source]->removeLink(sourcePin);
        _components[dest]->removeLink(otherPin);
    }

    /* Debug */

    void Manager::_dumpPrint(const std::string &title, ComponentType type) {
        std::cout << title << std::endl;
        for (auto &input : _components) {
            if (input.second->getType() != type)
                continue;
            input.second->dump();
        }
    }

    void Manager::dump(bool inputs, bool, bool outputs, bool truthTables) {
        std::cout << "tick: " << _currentTick << std::endl;
        if (inputs) {
            this->_dumpPrint("Constants: ", ComponentType::Constants);
        }
        if (inputs) {
            this->_dumpPrint("Inputs: ", ComponentType::Input);
        }
        if (outputs) {
            this->_dumpPrint("Output: ", ComponentType::Output);
        }
        if (!truthTables)
            return;
        for (auto &[key, vale] : _componentTruthTables) {
            std::cout << "Truth table for " << key << std::endl;
            std::cout << "  PinNb: " << std::get<0>(vale) << std::endl;
            std::cout << "  Input pins: ";
            for (auto &input : std::get<1>(vale)) {
                std::cout << input << " ";
            }
            std::cout << std::endl;
            std::cout << "  Output pins: ";
            for (auto &output : std::get<2>(vale)) {
                std::cout << output << " ";
            }
            std::cout << std::endl;
            std::cout << "  Truth table: " << std::endl;
            for (auto &row : std::get<3>(vale)) {
                for (auto &tristate : row) {
                    std::cout << (tristate == Tristate::True ? "T" : (tristate == Tristate::False ? "F" : "U"));
                }
                std::cout << std::endl;
            }
        }
    }

    /* Simulation */

    void Manager::simulate(std::size_t tick) {
        _currentTick = tick;
        for (auto &input : _components) {
            if (input.second->getType() == ComponentType::Input || input.second->getType() == ComponentType::Constants)
                input.second->computeBehaviour(tick);
        }
        for (auto &output : _components) {
            if (output.second->getType() == ComponentType::Output)
                output.second->computeBehaviour(tick);
        }
    }

    void Manager::initConstantsLinks() {
        for (auto &component : _components) {
            component.second->constantsInit();
        }
    }

    /* Parser */

    nts::ParserStage Manager::stageChecker(std::ifstream &fs, std::string &line, nts::ParserStage &stage) {
        nts::ParserStage newStage = nts::ParserStage::UNCHANGED;
        if (line == ".chipsets:") {
            newStage = nts::ParserStage::CHIPSET;
            std::getline(fs, line);
        } else if (line == ".links:") {
            newStage = nts::ParserStage::LINKS;
            std::getline(fs, line);
        }
        if (newStage != nts::ParserStage::UNCHANGED && newStage < stage) {
            throw CustomError("Invalid stage: " + std::to_string(newStage));
        }
        return newStage = (newStage == nts::ParserStage::UNCHANGED) ? stage : newStage;
    }

    void Manager::_stageChipsetHandler(const std::string &line) {
        std::istringstream iss(line);
        std::string type, label;

        if (!(iss >> type && iss >> label)) {
            throw CustomError("Invalid chipset line: " + line);
        }
        label = label.substr(0, label.find_first_of("#"));
        label.erase(label.find_last_not_of(" \t\r\n") + 1);
        if (!this->createComponent(type, label)) {
            throw CustomError("Component already exist: " + label);
        }
    }

    void Manager::_stageLinksHandler(const std::string &line, bool remove) {
        std::istringstream iss(line);
        std::string src, dest;

        if (!(iss >> src && iss >> dest)) {
            throw CustomError("Invalid link line: " + line);
        }
        std::size_t srcPin, destPin;
        try {
            srcPin = std::stoi(src.substr(src.find(":") + 1));
            destPin = std::stoi(dest.substr(dest.find(":") + 1));
        } catch (const std::invalid_argument& e) {
            throw CustomError("Invalid argument: " + std::string(e.what()));
        } catch (const std::out_of_range& e) {
            throw CustomError("Out of range: " + std::string(e.what()));
        }
        src = src.substr(0, src.find(":"));
        dest = dest.substr(0, dest.find(":"));

        bool srcExist = false;
        if (_components.find(src) != _components.end())
            srcExist = true;
        if (!srcExist)
            throw CustomError("Source Component not found: " + src);

        bool destExist = false;
        if (_components.find(dest) != _components.end())
            destExist = true;
        if (!destExist)
            throw CustomError("Destination Component not found: " + dest);

        if (!remove)
            return this->addLink(
                src,
                srcPin,
                dest,
                destPin);

        this->removeLink(
            src,
            srcPin,
            dest,
            destPin);
    }

    void Manager::_parserLoop(std::ifstream &fs) {
        std::string line;
        nts::ParserStage stage = nts::ParserStage::UNDEFINED;
        while (std::getline(fs, line)) {
            stage = Manager::stageChecker(fs, line, stage);
            if (line.empty() || line[0] == '#')
                continue;
            switch (stage) {
                case nts::ParserStage::CHIPSET:
                    this->_stageChipsetHandler(line);
                    break;
                case nts::ParserStage::LINKS:
                    this->_stageLinksHandler(line);
                    break;
                default:
                    throw CustomError("Invalid stage: " + std::to_string(stage));
            }
        }
        if (stage != nts::ParserStage::LINKS) {
            throw CustomError("All stages not found");
        }
    }

    void Manager::initGates(const std::string &folder) {
        if (!std::filesystem::exists(folder)) {
            throw CustomError("Directory does not exist: " + folder);
        }

        std::map<std::string, bool> paths;

        for (const auto &entry : std::filesystem::directory_iterator(folder)) {
            if (entry.path().string().rfind(".nts.config") == entry.path().string().size() - 11) {
                paths[entry.path()] = false;
            }
        }
        for (auto &entry : paths) {
            if (entry.second) continue;
            this->_parseGateConfig(entry.first, paths);
        }
    }

    void Manager::initializeTruthTables(const std::string &folder) {
        if (!std::filesystem::exists(folder)) {
            throw CustomError("Directory does not exist: " + folder);
        }

        for (const auto &entry : std::filesystem::directory_iterator(folder)) {
            if (entry.path().string().rfind(".nts.init") == entry.path().string().size() - 9) {
                this->_generateTruthTableFromFile(entry.path());
            }
        }
    }

    void Manager::_generateTruthTableFromFile(const std::string &filename) {
        std::ifstream fs(filename);
        if (!fs.is_open()) {
            throw CustomError("Could not open file: " + filename);
        }

        std::string currentName;
        std::size_t pinNb;
        std::vector<std::size_t> inputPins, outputPins;
        std::vector<std::vector<nts::Tristate>> currentTable;
        std::vector<std::string> forbiddenLabel = {
                "INPUT",
                "OUTPUT",
                "CLOCK",
                "TRUE",
                "FALSE",
                "RAM",
                "ROM",
                "JOHNSON"
        };

        std::string line;
        while (std::getline(fs, line)) {
            if (line.empty() || line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
                continue;
            }

            if (line.find(".LABEL:") == 0) {
                // Parse label
                if (std::find(forbiddenLabel.begin(), forbiddenLabel.end(), line.substr(7)) != forbiddenLabel.end()) {
                    throw CustomError("Forbidden label: " + line.substr(7));
                }
                if (!currentName.empty()) {
                    if (_componentTruthTables.find(currentName) != _componentTruthTables.end()) {
                        _componentTruthTables.erase(currentName);
                    }
                    _componentTruthTables[currentName] = std::make_tuple(pinNb, inputPins, outputPins, currentTable);
                    inputPins.clear();
                    outputPins.clear();
                    currentTable.clear();
                }
                currentName = line.substr(7);
            } else if (line.find(".PINNB:") == 0) {
                // Parse pin number
                pinNb = std::stoi(line.substr(7));
            } else if (line.find(".INPUT:") == 0) {
                // Parse input pins
                std::istringstream iss(line.substr(7));
                std::string pinStr;
                while (std::getline(iss, pinStr, ',')) {
                    std::size_t pin = std::stoi(pinStr);
                    inputPins.push_back(pin);
                }
            } else if (line.find(".OUTPUT:") == 0) {
                // Parse output pins
                std::istringstream iss(line.substr(8));
                std::string pinStr;
                while (std::getline(iss, pinStr, ',')) {
                    std::size_t pin = std::stoi(pinStr);
                    outputPins.push_back(pin);
                }
            } else {
                std::vector<nts::Tristate> row;
                for (char c : line) {
                    switch (c) {
                        case 'F':
                            row.push_back(nts::Tristate::False);
                            break;
                        case 'T':
                            row.push_back(nts::Tristate::True);
                            break;
                        case 'U':
                            row.push_back(nts::Tristate::Undefined);
                            break;
                        case 'X':
                            row.push_back(nts::Tristate::X);
                            break;
                        case '\r': // Ignore carriage return for Windows compatibility
                            break;
                        default:
                            throw CustomError("Invalid character in file `" + filename + "`: `" + std::string(1, c) + "`");
                    }
                }
                currentTable.push_back(row);
            }
        }

        if (!currentName.empty()) {
            _componentTruthTables[currentName] = std::make_tuple(pinNb, inputPins, outputPins, currentTable);
        }

        fs.close();
    }

    /* Commands */

    void Manager::_help() {
        if (std::system("pandoc --version > /dev/null 2>&1") != 0) {
            throw CustomError("Usage: ./nanotekspice [file.nts] (flags)");
        }
        if (std::system("lynx --version > /dev/null 2>&1") != 0) {
            throw CustomError("Usage: ./nanotekspice [file.nts] (flags)");
        }
        system("pandoc ReadMe.md | lynx -stdin");
    }

    void Manager::_commandHelp() {
        std::ifstream file("ReadMe.md");
        if (!file.is_open()) {
            throw CustomError("Could not open file: ReadMe.md");
        }

        std::string line;
        bool isShellCommandsSection = false;

        while (std::getline(file, line)) {
            if (line == "## Shell commands") {
                isShellCommandsSection = true;
                getline(file, line);
            } else if (isShellCommandsSection && line.substr(0, 2) == "##") {
                break;
            } else if (isShellCommandsSection) {
                std::cout << line << std::endl;
            }
        }
        file.close();
    }

    void Manager::preParse(int ac, char **av) {
        for (int i = 1; i < ac; i++) {
            if (std::string(av[i]) == "--table-dir" && i + 1 < ac) {
                this->initializeTruthTables(av[i + 1]);
                i++;
            }
        }
    }

    void Manager::parser(int ac, char **av) {
        if (ac <= 1) throw CustomError("Usage: ./nanotekspice [file.nts] (flags)");
        if (std::string(av[1]) == "-h" || std::string(av[1]) == "--help") {
            _help();
            exit(0);
        }
        if (std::string(av[1]) == "--run-empty") {
            this->createComponent("input", "input1");
            this->createComponent("output", "output1");
            this->addLink("input1", 1, "output1", 1);
            return;
        }
        const char* extension = strrchr(av[1], '.');
        if (extension == nullptr || strcmp(extension, ".nts") != 0) {
            throw CustomError("Invalid file extension: " + std::string(av[1]));
        }
        std::ifstream fs(av[1]);
        if (!fs.is_open()) {
            throw CustomError("Could not open file: " + std::string(av[1]));
        }
        this->_parserLoop(fs);
        fs.close();
    }

    void Manager::_displayPrint(const std::string &title, ComponentType types) {
        std::cout << title << std::endl;
        for (auto &input: _components) {
            if (input.second->getType() != types)
                continue;
            std::cout << "  " << input.first << ": "
                << ((input.second->getPins()[1].first == nts::Tristate::Undefined)
                ? "U" : (input.second->getPins()[1].first == nts::Tristate::True) ? "1" : "0")
                << std::endl;
        }
    }

    void Manager::display() {
        std::cout << "tick: " << _currentTick << std::endl;
        this->_displayPrint("input(s):", ComponentType::Input);
        this->_displayPrint("output(s):", ComponentType::Output);
    }

    void Manager::_loop() {
        signal(SIGINT, [](int) {
            std::cout << std::endl; // Prevents the ^C from being printed
            g_sigintCaught = true;
        });

        while (!g_sigintCaught) {
            this->simulate(_currentTick + 1);
            this->display();
        }
    }

    /* Execution Loop */

    void Manager::_handleCommand(const std::string &line) {
        std::map<std::string, std::function<void()>> commands = {
                {"clear", [this]() {
                    #ifdef __unix__
                    system("clear");
                    #endif
                    #ifdef _WIN32
                    throw CustomError("Use a real operating system");
                    #endif
                }},
                {"help", [this]() {this->_commandHelp(); }},
                {"loop", [this]() { this->_loop(); }},
                {"dump", [this]() { this->dump(); }},
                {"simulate", [this]() { this->simulate(_currentTick + 1); }},
                {"sm", [this]() { this->simulate(_currentTick + 1); }},
                {"display", [this]() { this->display(); }},
                {"ls", [this]() { this->display(); }},
                {"add ", [this, &line]() {
                    std::string command = line.substr(4);
                    this->_stageChipsetHandler(command);
                }},
                {"link ", [this, &line]() {
                    std::string command = line.substr(5);
                    this->_stageLinksHandler(command);
                }},
                {"removeLink ", [this, &line]() {
                    std::string command = line.substr(11);
                    this->_stageLinksHandler(command, true);
                }},
                {"removeChipset ", [this, &line]() {
                    std::string command = line.substr(14);
                    auto it = _components.find(command);
                    if (it == _components.end()) {
                        throw CustomError("Component not found: " + command);
                    }
                    auto &component = it->second;
                    auto pins = component->getPins();
                    for (const auto &pin : pins) {
                        removeLink(command, pin.first, command, pin.first);
                    }
                    for (auto &otherComponentPair : _components) {
                        if (otherComponentPair.first == command) continue; // Skip the component being removed
                        auto &otherComponent = otherComponentPair.second;
                        auto otherPins = otherComponent->getPins();
                        for (const auto &otherPin : otherPins) {
                            if (otherPin.second.second.size() > 0 && otherPin.second.second[0].first.get().getLabel() == command) {
                                removeLink(otherComponentPair.first, otherPin.first, command, otherPin.second.second[0].second);
                            }
                        }
                    }
                    _components.erase(it);
                }}
        };

        for (auto &command : commands) {
            if (line.find(command.first) == 0) {
                command.second();
                return;
            }
        }
        this->_interpretLine(line);
    }

    void Manager::_interpretLine(const std::string &line) {
        std::string target;
        std::string tension;

        std::istringstream iss(line);
        if (!(std::getline(iss, target, '=') && std::getline(iss, tension) && iss.eof())) {
            throw CustomError("Invalid command: " + line);
        }

        target.erase(target.begin(), std::find_if(target.begin(), target.end(), [](int ch) {
            return !std::isspace(ch);
        }));
        target.erase(std::find_if(target.rbegin(), target.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), target.end());

        tension.erase(tension.begin(), std::find_if(tension.begin(), tension.end(), [](int ch) {
            return !std::isspace(ch);
        }));
        tension.erase(std::find_if(tension.rbegin(), tension.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), tension.end());

        if (tension != "0" && tension != "1" && tension != "U") {
            throw CustomError("Invalid tension: " + tension);
        }
        if (_components.find(target) == _components.end()) {
            throw CustomError("Unknown component: " + target);
        }
        if (_components[target]->getType() != ComponentType::Input) {
            throw CustomError("Invalid component type: " + target);
        }
        _components[target]->setValue(tension == "1" ? nts::Tristate::True : (tension == "0" ? nts::Tristate::False : nts::Tristate::Undefined));
    }

    void Manager::_checkRun() const {
        if (_components.empty()) {
            throw CustomError("No components found");
        }
        bool hasInput = false;
        for (auto &component: _components) {
            if (component.second->getType() == ComponentType::Input || component.second->getType() == ComponentType::Constants) {
                hasInput = true;
                break;
            }
        }
        if (!hasInput) {
            throw CustomError("No input found");
        }
        bool hasOutput = false;
        for (auto &component: _components) {
            if (component.second->getType() == ComponentType::Output) {
                hasOutput = true;
                break;
            }
        }
        if (!hasOutput) {
            throw CustomError("No output found");
        }
    }

    void Manager::run() {
        this->_checkRun();
        std::string line;
        while (std::cout << "> " && std::getline(std::cin, line)) {
            std::vector<std::string> commands;
            std::istringstream iss(line);
            std::string token;

            while (std::getline(iss, token, '&')) {
                token.erase(0, token.find_first_not_of(" \t\r\n"));
                token.erase(token.find_last_not_of(" \t\r\n") + 1);
                if (!token.empty()) {
                    commands.push_back(token);
                }
            }

            for (const auto& command : commands) {
                if (command == "exit") {
                    return;
                }
                try {
                    this->_handleCommand(command);
                } catch (const CustomError &e) {
                    std::cout << e.what() << std::endl;
                }
            }
        }
    }

    void Manager::clearComponents() {
        for (auto &component : _components) {
            delete component.second;
        }
        _components.clear();
        _currentTick = 0;
    }
}
