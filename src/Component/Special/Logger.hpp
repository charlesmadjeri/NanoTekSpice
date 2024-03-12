#pragma once

#include "../AComponent.hpp"

#define LOG_FILE "./log.bin"

namespace nts {
    class Logger : public AComponent {
        public:
            Logger(const std::string &label) : AComponent(1, label, ComponentType::Output) {};
            void computeBehaviour(std::size_t tick) {
                this->update(tick);
                std::ofstream file(LOG_FILE, std::ios::app | std::ios::binary);
                nts::Tristate state = _pins[1].first;
                file.write(reinterpret_cast<char*>(&state), sizeof(state));
                file.close();
            }
    };
}
