#pragma once

#include <vector>

namespace nts {
    const std::vector<std::vector<nts::Tristate>> OR_TRUTH_TABLE = {
            {Tristate::True, Tristate::True, Tristate::True},
            {Tristate::True, Tristate::False, Tristate::True},
            {Tristate::True, Tristate::Undefined, Tristate::True},
            {Tristate::False, Tristate::True, Tristate::True},
            {Tristate::False, Tristate::False, Tristate::False},
            {Tristate::False, Tristate::Undefined, Tristate::Undefined},
            {Tristate::Undefined, Tristate::True, Tristate::True},
            {Tristate::Undefined, Tristate::False, Tristate::Undefined},
            {Tristate::Undefined, Tristate::Undefined, Tristate::Undefined}
    };
}
