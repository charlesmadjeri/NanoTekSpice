#include <criterion/criterion.h>

#include "../include/paths.hpp"
#include "../src/Manager/Manager.hpp"

nts::Manager *FullAdderManager;

void setup(void) {
    FullAdderManager = new nts::Manager();
    FullAdderManager->initializeTruthTables(TRUTH_TABLE_PATH);
    FullAdderManager->initGates(PRIMARY_GATES_PATH);
    FullAdderManager->initGates(SECONDARY_GATES_PATH);
    char av1[] = "./a.out";
    char av2[] = "./test/nts_single/fulladder.nts";
    char *av[] = {av1, av2};
    FullAdderManager->parser(2, av);
}

void teardown(void) {
    delete FullAdderManager;
}

typedef enum {
    A = 1,
    B = 2,
    Cin = 3,
    S = 4,
    Cout = 5
} PinName;

TestSuite(fulladder_tests, .init = setup, .fini = teardown);

Test(fulladder_tests, test_all_cases) {
    std::vector<std::tuple<nts::Tristate, nts::Tristate, nts::Tristate, nts::Tristate, nts::Tristate>> testCases = {
            {nts::Tristate::False, nts::Tristate::False, nts::Tristate::False, nts::Tristate::False, nts::Tristate::False},
            {nts::Tristate::False, nts::Tristate::False, nts::Tristate::True, nts::Tristate::True, nts::Tristate::False},
            {nts::Tristate::False, nts::Tristate::True, nts::Tristate::False, nts::Tristate::True, nts::Tristate::False},
            {nts::Tristate::False, nts::Tristate::True, nts::Tristate::True, nts::Tristate::False, nts::Tristate::True},
            {nts::Tristate::True, nts::Tristate::False, nts::Tristate::False, nts::Tristate::True, nts::Tristate::False},
            {nts::Tristate::True, nts::Tristate::False, nts::Tristate::True, nts::Tristate::False, nts::Tristate::True},
            {nts::Tristate::True, nts::Tristate::True, nts::Tristate::False, nts::Tristate::False, nts::Tristate::True},
            {nts::Tristate::True, nts::Tristate::True, nts::Tristate::True, nts::Tristate::True, nts::Tristate::True}
    };

    for (const auto& testCase : testCases) {
    nts::Tristate A = std::get<0>(testCase);
    nts::Tristate B = std::get<1>(testCase);
    nts::Tristate Cin = std::get<2>(testCase);
    nts::Tristate expectedS = std::get<3>(testCase);
    nts::Tristate expectedCout = std::get<4>(testCase);

    FullAdderManager->getComponent("A")->setValue(A);
    FullAdderManager->getComponent("B")->setValue(B);
    FullAdderManager->getComponent("Cin")->setValue(Cin);

    FullAdderManager->simulate();

    nts::Tristate S = FullAdderManager->getComponent("S")->getValue(1);
    nts::Tristate Cout = FullAdderManager->getComponent("Cout")->getValue(1);

    cr_assert_eq(S, expectedS, "Failed on S with inputs A=%d, B=%d, Cin=%d", A, B, Cin);
    cr_assert_eq(Cout, expectedCout, "Failed on Cout with inputs A=%d, B=%d, Cin=%d", A, B, Cin);
    }
}
