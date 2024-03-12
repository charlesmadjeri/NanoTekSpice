#pragma once

#include "AComponent.hpp"

namespace nts {
    class Chipset : public AComponent {
        private:
            std::vector<std::size_t> _inputPins;
            std::vector<std::size_t> _outputPins;
            std::vector<std::vector<nts::Tristate>> _truthTable;

        public:
            Chipset(
                std::size_t pinNb,
                const std::string &label,
                ComponentType type,
                std::vector<std::vector<nts::Tristate>> truthTable,
                std::vector<std::size_t> inputPins,
                std::vector<std::size_t> outputPins
            ) : AComponent(pinNb, label, type),
                _inputPins(inputPins), _outputPins(outputPins), _truthTable(truthTable) {};

            void computeBehaviour(std::size_t tick) override {
                bool inputLinkFound = false;
                if (tick <= _lastTick)
                    return;
                _lastTick = tick;
                this->update(tick);
                std::vector<nts::Tristate> inputStates;

                for (std::size_t i : _inputPins) {
                    for (auto link : _pins[i].second) {
                        inputStates.push_back(link.first.get().getPins()[link.second].first);
                        inputLinkFound = true;
                    }
                }

                if (!inputLinkFound) {
                    return;
                }

                if (!_truthTable.size())
                    throw nts::CustomError("No truth table found for " + _label);

                for (const auto& row : _truthTable) {
                    std::vector<nts::Tristate> inputPart(row.begin(), row.begin() + _inputPins.size());
                    std::vector<nts::Tristate> outputPart(row.begin() + _inputPins.size(), row.end());

                    bool match = true;
                    for (std::size_t i = 0; i < inputStates.size(); ++i) {
                        if (inputPart[i] != nts::Tristate::X && inputStates[i] != inputPart[i]) {
                            match = false;
                            break;
                        }
                    }
                    if (match) {
                        for (std::size_t i = 0; i < _outputPins.size(); ++i) {
                            _pins[_outputPins[i]].first = outputPart[i];
                        }
                        return;
                    }
                }

                throw nts::CustomError("No match found in truth table");
            };
    };
}
