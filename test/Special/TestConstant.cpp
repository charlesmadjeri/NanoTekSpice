#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "../../include/paths.hpp"
#include "../../src/Manager/Manager.hpp"

nts::Manager ConstantManager;

void ConstantSetup() {
    ConstantManager.initializeTruthTables(TRUTH_TABLE_PATH);
    ConstantManager.createComponent("true", "true1");
    ConstantManager.createComponent("false", "false1");
}

void ConstantTeardown() {
    ConstantManager.clearComponents();
}

TestSuite(Constant, .init = ConstantSetup, .fini = ConstantTeardown);

Test(Constant, should_create_true_const) {
    nts::IComponent *component = ConstantManager.getComponent("true1");

    cr_assert_eq(component->getLabel(), "true1");
    cr_assert_eq(component->getType(), nts::ComponentType::Constants);
    cr_assert_eq(component->getValue(1), nts::Tristate::True);
}

Test(Constant, should_create_false_const) {
    nts::IComponent *component = ConstantManager.getComponent("false1");

    cr_assert_eq(component->getLabel(), "false1");
    cr_assert_eq(component->getType(), nts::ComponentType::Constants);
    cr_assert_eq(component->getValue(1), nts::Tristate::False);
}

Test(Constant, should_compute_behaviour_true_const) {
    nts::IComponent *component = ConstantManager.getComponent("true1");

    ConstantManager.simulate(1);
    cr_assert_eq(component->getValue(1), nts::Tristate::True);

    ConstantManager.simulate(2);
    cr_assert_eq(component->getValue(1), nts::Tristate::True);

    ConstantManager.simulate(3);
    cr_assert_eq(component->getValue(1), nts::Tristate::True);
}

Test(Constant, should_compute_behaviour_false_const) {
    nts::IComponent *component = ConstantManager.getComponent("false1");

    ConstantManager.simulate(1);
    cr_assert_eq(component->getValue(1), nts::Tristate::False);

    ConstantManager.simulate(2);
    cr_assert_eq(component->getValue(1), nts::Tristate::False);

    ConstantManager.simulate(3);
    cr_assert_eq(component->getValue(1), nts::Tristate::False);
}
