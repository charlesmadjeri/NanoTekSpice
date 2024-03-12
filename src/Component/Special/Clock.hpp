#pragma once

#include "../AComponent.hpp"

namespace nts {
    class Clock : public AComponent {
        public:
            nts::Tristate _newValue;
            bool needUpdate;

            Clock(const std::string &label) : AComponent(1, label, ComponentType::Input) {
                needUpdate = false;
            };
            void computeBehaviour(std::size_t tick) {
                if (tick <= _lastTick) return;
                _lastTick = tick;
                if (needUpdate) {
                    this->setState(1, _newValue);
                    return (void)(needUpdate = false);
                }
                if (_pins[1].first == nts::Tristate::Undefined) return;
                return (void)(_pins[1].first = (_pins[1].first == nts::Tristate::True) ? nts::Tristate::False : nts::Tristate::True);
            }
            void setValue(nts::Tristate value) {
                _newValue = value;
                needUpdate = true;
            }
    };
}
