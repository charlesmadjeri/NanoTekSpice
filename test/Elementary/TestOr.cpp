#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "../../include/paths.hpp"
#include "../../src/Manager/Manager.hpp"

nts::Manager OrManager;

void OrSetup() {
    OrManager.initializeTruthTables(TRUTH_TABLE_PATH);
    OrManager.createComponent("or", "or1");
    OrManager.createComponent("input", "in1");
    OrManager.createComponent("input", "in2");
    OrManager.createComponent("output", "out1");
    OrManager.addLink("in1", 1, "or1", 1);
    OrManager.addLink("in2", 1, "or1", 2);
    OrManager.addLink("or1", 3, "out1", 1);
}

void OrTeardown() {
    OrManager.clearComponents();
}

TestSuite(Or, .init = OrSetup, .fini = OrTeardown);

Test(Or, should_create_or) {
    nts::IComponent *component = OrManager.getComponent("or1");

    cr_assert_eq(component->getLabel(), "or1");
    cr_assert_eq(component->getType(), nts::ComponentType::Standard);
}

Test(Or, should_link_or) {
    nts::IComponent *output = OrManager.getComponent("out1");
    cr_assert_eq(output->getValue(1), nts::Tristate::Undefined);

    OrManager.simulate(0);
    cr_assert_eq(output->getValue(1), nts::Tristate::Undefined);

    OrManager.getComponent("in1")->setValue(nts::Tristate::True);
    OrManager.getComponent("in2")->setValue(nts::Tristate::False);
    OrManager.simulate(1);

    cr_assert_eq(output->getValue(1), nts::Tristate::True);

    OrManager.getComponent("in1")->setValue(nts::Tristate::False);
    OrManager.simulate(2);

    cr_assert_eq(output->getValue(1), nts::Tristate::False);
}

Test(Or, or_truth_table) {
    std::vector<nts::Tristate> states = {nts::Tristate::True, nts::Tristate::False, nts::Tristate::Undefined};

    for (auto state1 : states) {
        for (auto state2 : states) {
            OrManager.getComponent("in1")->setValue(state1);
            OrManager.getComponent("in2")->setValue(state2);
            OrManager.simulate();

            nts::Tristate expectedOutput;
            if (state1 == nts::Tristate::True || state2 == nts::Tristate::True) {
                expectedOutput = nts::Tristate::True;
            } else if (state1 == nts::Tristate::False && state2 == nts::Tristate::False) {
                expectedOutput = nts::Tristate::False;
            } else {
                expectedOutput = nts::Tristate::Undefined;
            }

            nts::IComponent *output = OrManager.getComponent("out1");
            cr_assert_eq(output->getValue(1), expectedOutput);
        }
    }
}
