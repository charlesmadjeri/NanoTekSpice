#pragma once

#include "IComponent.hpp"
#include "../../include/orTruthTable.hpp"

namespace nts {
    class AComponent : public IComponent {
        protected:
            std::map<std::size_t,
                     std::pair<
                         Tristate,
                         std::vector<
                            std::pair<
                                std::reference_wrapper<IComponent>,
                                std::size_t>
            >>> _pins;
            std::size_t _lastTick;
            std::string _label;
            nts::ComponentType _type;
            nts::Tristate _internalCompute(std::size_t pin, bool forceCompute = true);

        public:
            AComponent(int pinNb, const std::string &label, ComponentType type = ComponentType::Standard);
            ~AComponent() {};
            void simulate(std::size_t tick) override;
            nts::Tristate compute (std::size_t pin, bool forceCompute = true) override;
            void update(std::size_t tick) override;
            void constantsInit() override;
            void setLink (
                std::size_t pin, nts::IComponent& other, std::size_t otherPin) override;
            void removeLink(std::size_t pin) override;
            void dump() const override;
            ComponentType getType() const override { return _type; };
            std::string getLabel() const override { return _label; }
            std::map<std::size_t,
                std::pair<
                    Tristate,
                    std::vector<
                        std::pair<
                            std::reference_wrapper<IComponent>,
                            std::size_t>
            >>> getPins() const override { return _pins; };
            std::vector<
                std::pair<
                    std::reference_wrapper<IComponent>,
                    std::size_t>
            > getLinks(std::size_t pin) override { return _pins[pin].second; };
            void setState(std::size_t pin, Tristate state) override { _pins[pin].first = state; };
            virtual void computeBehaviour(std::size_t tick) = 0;
            void setValue(nts::Tristate) override {};
            nts::Tristate getValue(std::size_t pin) override { return _pins[pin].first; };
    };
}
