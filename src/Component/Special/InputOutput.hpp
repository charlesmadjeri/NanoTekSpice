#pragma once

#include "../AComponent.hpp"

namespace nts {
    class InputComp : public AComponent {
        public:
            nts::Tristate _newValue;
            bool needUpdate;

            InputComp(const std::string &label) : AComponent(1, label, ComponentType::Input) {};
            void computeBehaviour(std::size_t) {
                if (!needUpdate) return;
                this->setState(1, _newValue);
                needUpdate = false;
            };
            void setValue(nts::Tristate value) {
                _newValue = value;
                needUpdate = true;
            };
    };

    class OutputComp : public AComponent {
        public:
            OutputComp(const std::string &label) : AComponent(1, label, ComponentType::Output) {};
            void computeBehaviour(std::size_t tick) { this->update(tick); };
    };
}
