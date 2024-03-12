#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "../../include/paths.hpp"
#include "../../src/Manager/Manager.hpp"

nts::Manager XorManager;

void XorSetup() {
    XorManager.initializeTruthTables(TRUTH_TABLE_PATH);
    XorManager.createComponent("xor", "xor1");
    XorManager.createComponent("input", "in1");
    XorManager.createComponent("input", "in2");
    XorManager.createComponent("output", "out1");
    XorManager.addLink("in1", 1, "xor1", 1);
    XorManager.addLink("in2", 1, "xor1", 2);
    XorManager.addLink("xor1", 3, "out1", 1);
}

void XorTeardown() {
    XorManager.clearComponents();
}

TestSuite(Xor, .init = XorSetup, .fini = XorTeardown);

Test(Xor, should_create_xor) {
    nts::IComponent *component = XorManager.getComponent("xor1");

    cr_assert_eq(component->getLabel(), "xor1");
    cr_assert_eq(component->getType(), nts::ComponentType::Standard);
}

Test(Xor, should_link_xor) {
    nts::IComponent *output = XorManager.getComponent("out1");
    cr_assert_eq(output->getValue(1), nts::Tristate::Undefined);

    XorManager.simulate(0);
    cr_assert_eq(output->getValue(1), nts::Tristate::Undefined);

    XorManager.getComponent("in1")->setValue(nts::Tristate::True);
    XorManager.getComponent("in2")->setValue(nts::Tristate::False);
    XorManager.simulate(1);

    cr_assert_eq(output->getValue(1), nts::Tristate::True);

    XorManager.getComponent("in1")->setValue(nts::Tristate::False);
    XorManager.simulate(2);

    cr_assert_eq(output->getValue(1), nts::Tristate::False);
}

Test(Xor, xor_truth_table) {
    std::vector<nts::Tristate> states = {nts::Tristate::True, nts::Tristate::False, nts::Tristate::Undefined};

    for (auto state1 : states) {
        for (auto state2 : states) {
            XorManager.getComponent("in1")->setValue(state1);
            XorManager.getComponent("in2")->setValue(state2);
            XorManager.simulate();

            nts::Tristate expectedOutput;
            if ((state1 == nts::Tristate::True && state2 == nts::Tristate::False) ||
                (state1 == nts::Tristate::False && state2 == nts::Tristate::True)) {
                expectedOutput = nts::Tristate::True;
            } else if (state1 == nts::Tristate::Undefined || state2 == nts::Tristate::Undefined) {
                expectedOutput = nts::Tristate::Undefined;
            } else {
                expectedOutput = nts::Tristate::False;
            }

            nts::IComponent *output = XorManager.getComponent("out1");
            cr_assert_eq(output->getValue(1), expectedOutput);
        }
    }
}
