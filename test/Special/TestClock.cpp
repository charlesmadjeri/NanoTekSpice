#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "../../include/paths.hpp"
#include "../../src/Manager/Manager.hpp"

nts::Manager ClockManager;

void ClockSetup() {
    ClockManager.initializeTruthTables(TRUTH_TABLE_PATH);
    ClockManager.createComponent("clock", "clock1");
}

void ClockTeardown() {
    ClockManager.clearComponents();
}

TestSuite(Clock, .init = ClockSetup, .fini = ClockTeardown);

Test(Clock, should_create_clock) {
    nts::IComponent *component = ClockManager.getComponent("clock1");

    cr_assert_eq(component->getLabel(), "clock1");
    cr_assert_eq(component->getType(), nts::ComponentType::Input);
    cr_assert_eq(component->getValue(1), nts::Tristate::Undefined);
}

Test(Clock, should_compute_behaviour) {
    nts::IComponent *component = ClockManager.getComponent("clock1");

    component->setValue(nts::Tristate::True);

    ClockManager.simulate(1);
    cr_assert_eq(component->getValue(1), nts::Tristate::True);

    ClockManager.simulate(2);
    cr_assert_eq(component->getValue(1), nts::Tristate::False);

    ClockManager.simulate(3);
    cr_assert_eq(component->getValue(1), nts::Tristate::True);
}
