#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "../../include/paths.hpp"
#include "../../src/Manager/Manager.hpp"

nts::Manager AndManager;

void AndSetup() {
    AndManager.initializeTruthTables(TRUTH_TABLE_PATH);
    AndManager.createComponent("and", "and1");
    AndManager.createComponent("input", "in1");
    AndManager.createComponent("input", "in2");
    AndManager.createComponent("output", "out1");
    AndManager.addLink("in1", 1, "and1", 1);
    AndManager.addLink("in2", 1, "and1", 2);
    AndManager.addLink("and1", 3, "out1", 1);
}

void AndTeardown() {
    AndManager.clearComponents();
}

TestSuite(And, .init = AndSetup, .fini = AndTeardown);

Test(And, should_create_and) {
    nts::IComponent *component = AndManager.getComponent("and1");

    cr_assert_eq(component->getLabel(), "and1");
    cr_assert_eq(component->getType(), nts::ComponentType::Standard);
}

Test(And, should_link_and) {
    nts::IComponent *output = AndManager.getComponent("out1");
    cr_assert_eq(output->getValue(1), nts::Tristate::Undefined);

    AndManager.simulate(0);
    cr_assert_eq(output->getValue(1), nts::Tristate::Undefined);

    AndManager.getComponent("in1")->setValue(nts::Tristate::True);
    AndManager.getComponent("in2")->setValue(nts::Tristate::True);
    AndManager.simulate(1);

    cr_assert_eq(output->getValue(1), nts::Tristate::True);

    AndManager.getComponent("in2")->setValue(nts::Tristate::False);
    AndManager.simulate(2);

    cr_assert_eq(output->getValue(1), nts::Tristate::False);
}

Test(And, and_truth_table) {
    std::vector<nts::Tristate> states = {nts::Tristate::True, nts::Tristate::False, nts::Tristate::Undefined};

    for (auto state1 : states) {
        for (auto state2 : states) {
            AndManager.getComponent("in1")->setValue(state1);
            AndManager.getComponent("in2")->setValue(state2);
            AndManager.simulate();

            nts::Tristate expectedOutput;
            if (state1 == nts::Tristate::False || state2 == nts::Tristate::False) {
                expectedOutput = nts::Tristate::False;
            } else if (state1 == nts::Tristate::True && state2 == nts::Tristate::True) {
                expectedOutput = nts::Tristate::True;
            } else {
                expectedOutput = nts::Tristate::Undefined;
            }

            nts::IComponent *output = AndManager.getComponent("out1");
            cr_assert_eq(output->getValue(1), expectedOutput);
        }
    }
}
