#pragma once

#include <memory>
#include <iostream>
#include <map>
#include <vector>
#include <stack>

#include "../../include/Error/Error.hpp"

namespace nts {
    enum Tristate {
        Undefined = (-true),
        True = true,
        False = false,
        X = 2 // Defined value not used during computation
    };

    enum ComponentType {
        Input = 0,
        Output = 1,
        Standard = 2,
        Constants = 3,
    };

    class IComponent {
        public :
            virtual ~IComponent() = default;
            virtual void simulate(std::size_t tick) = 0;
            virtual nts::Tristate compute (std::size_t pin, bool forceCompute) = 0;
            virtual void update(std::size_t tick) = 0;
            virtual void constantsInit() = 0;
            virtual void setLink (
                std::size_t pin, nts::IComponent& other, std::size_t otherPin) = 0;
            virtual void removeLink(std::size_t pin) = 0;
            virtual void dump() const = 0;
            virtual ComponentType getType() const = 0;
            virtual std::string getLabel() const = 0;
            virtual std::map<std::size_t,
                    std::pair<
                        Tristate,
                        std::vector<
                            std::pair<
                                std::reference_wrapper<IComponent>,
                                std::size_t>
            >>> getPins() const = 0;
            virtual std::vector<
                std::pair<
                        std::reference_wrapper<IComponent>,
                        std::size_t>
            > getLinks(std::size_t pin) = 0;
            virtual void setState(std::size_t pin, Tristate state) = 0;
            virtual void computeBehaviour(std::size_t tick) = 0;
            virtual void setValue(nts::Tristate value) = 0;
            virtual Tristate getValue(std::size_t pin) = 0;
    };
}
