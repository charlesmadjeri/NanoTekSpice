#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "../../include/paths.hpp"
#include "../../src/Manager/Manager.hpp"

nts::Manager IOManager;

void IOSetup() {
    IOManager.initializeTruthTables(TRUTH_TABLE_PATH);
    IOManager.createComponent("input", "input1");
    IOManager.createComponent("output", "output1");
}

void IOTeardown() {
    IOManager.clearComponents();
}

TestSuite(InputOutput, .init = IOSetup, .fini = IOTeardown);

Test(InputOutput, should_create_input) {
    nts::IComponent *component = IOManager.getComponent("input1");

    cr_assert_eq(component->getLabel(), "input1");
    cr_assert_eq(component->getType(), nts::ComponentType::Input);
    cr_assert_eq(component->getValue(1), nts::Tristate::Undefined);
}

Test(InputOutput, should_create_output) {
    nts::IComponent *component = IOManager.getComponent("output1");

    cr_assert_eq(component->getLabel(), "output1");
    cr_assert_eq(component->getType(), nts::ComponentType::Output);
    cr_assert_eq(component->getValue(1), nts::Tristate::Undefined);
}

Test(InputOutput, should_set_value_input) {
    nts::IComponent *component = IOManager.getComponent("input1");

    component->setValue(nts::Tristate::True);
    IOManager.simulate(1);
    cr_assert_eq(component->getValue(1), nts::Tristate::True);

    component->setValue(nts::Tristate::False);
    IOManager.simulate(2);
    cr_assert_eq(component->getValue(1), nts::Tristate::False);
}

Test(InputOutput, should_compute_behaviour_output) {
    nts::IComponent *input = IOManager.getComponent("input1");
    nts::IComponent *output = IOManager.getComponent("output1");

    IOManager.addLink("input1", 1, "output1", 1);

    input->setValue(nts::Tristate::True);
    IOManager.simulate(1);
    cr_assert_eq(output->getValue(1), nts::Tristate::True);

    input->setValue(nts::Tristate::False);
    IOManager.simulate(2);
    cr_assert_eq(output->getValue(1), nts::Tristate::False);
}
