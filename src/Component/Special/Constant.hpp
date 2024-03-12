#pragma once

#include "../AComponent.hpp"

namespace nts {
    class TrueConst : public AComponent {
        public:
            TrueConst(const std::string &label) : AComponent(1, label, ComponentType::Constants) {
                this->computeBehaviour(0);
            };
            void computeBehaviour(std::size_t) {
                _pins[1].first = nts::Tristate::True;
            };
    };

    class FalseConst : public AComponent {
        public:
            FalseConst(const std::string &label) : AComponent(1, label, ComponentType::Constants) {
                this->computeBehaviour(0);
            };
            void computeBehaviour(std::size_t) {
                _pins[1].first = nts::Tristate::False;
            };
    };
}
