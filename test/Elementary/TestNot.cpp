#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "../../include/paths.hpp"
#include "../../src/Manager/Manager.hpp"

nts::Manager NotManager;

void NotSetup() {
    NotManager.initializeTruthTables(TRUTH_TABLE_PATH);
    NotManager.createComponent("not", "not1");
    NotManager.createComponent("input", "in1");
    NotManager.createComponent("output", "out1");
    NotManager.addLink("in1", 1, "not1", 1);
    NotManager.addLink("not1", 2, "out1", 1);
}

void NotTeardown() {
    NotManager.clearComponents();
}

TestSuite(Not, .init = NotSetup, .fini = NotTeardown);

Test(Not, should_create_not) {
    nts::IComponent *component = NotManager.getComponent("not1");

    cr_assert_eq(component->getLabel(), "not1");
    cr_assert_eq(component->getType(), nts::ComponentType::Standard);
}

Test(Not, should_link_not) {
    nts::IComponent *output = NotManager.getComponent("out1");
    cr_assert_eq(output->getValue(1), nts::Tristate::Undefined);

    NotManager.simulate(0);
    cr_assert_eq(output->getValue(1), nts::Tristate::Undefined);

    NotManager.getComponent("in1")->setValue(nts::Tristate::True);
    NotManager.simulate(1);

    cr_assert_eq(output->getValue(1), nts::Tristate::False);

    NotManager.getComponent("in1")->setValue(nts::Tristate::False);
    NotManager.simulate(2);

    cr_assert_eq(output->getValue(1), nts::Tristate::True);
}

Test(Not, not_truth_table) {
    std::vector<nts::Tristate> states = {nts::Tristate::True, nts::Tristate::False, nts::Tristate::Undefined};

    for (auto state1 : states) {
        NotManager.getComponent("in1")->setValue(state1);
        NotManager.simulate();

        nts::Tristate expectedOutput;
        if (state1 == nts::Tristate::True) {
            expectedOutput = nts::Tristate::False;
        } else if (state1 == nts::Tristate::False) {
            expectedOutput = nts::Tristate::True;
        } else {
            expectedOutput = nts::Tristate::Undefined;
        }

        nts::IComponent *output = NotManager.getComponent("out1");
        cr_assert_eq(output->getValue(1), expectedOutput);
    }
}
