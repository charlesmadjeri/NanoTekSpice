#include <criterion/criterion.h>

#include "../include/paths.hpp"
#include "../src/Manager/Manager.hpp"

nts::Manager *FourBitAdderManager;

void FourBitAdderSetup(void) {
    FourBitAdderManager = new nts::Manager();
    FourBitAdderManager->initializeTruthTables(TRUTH_TABLE_PATH);
    FourBitAdderManager->initGates(PRIMARY_GATES_PATH);
    FourBitAdderManager->initGates(SECONDARY_GATES_PATH);
    char av1[] = "./a.out";
    char av2[] = "./test/nts_single/4008_adder.nts";
    char *av[] = {av1, av2};
    FourBitAdderManager->parser(2, av);
}

void FourBitAdderTeardown(void) {
    delete FourBitAdderManager;
}

TestSuite(four_bit_adder_tests, .init = FourBitAdderSetup, .fini = FourBitAdderTeardown);

std::vector<std::vector<nts::Tristate>> generateTestCases() {
    std::vector<std::vector<nts::Tristate>> testCases;

    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            for (int k = 0; k < 2; ++k) {
                std::vector<nts::Tristate> testCase;
                // Generate inputs for in_a1 to in_a4
                for (int bit = 0; bit < 4; ++bit) {
                    testCase.push_back((i & (1 << bit)) ? nts::Tristate::True : nts::Tristate::False);
                }
                // Generate inputs for in_b1 to in_b4
                for (int bit = 0; bit < 4; ++bit) {
                    testCase.push_back((j & (1 << bit)) ? nts::Tristate::True : nts::Tristate::False);
                }
                // Generate input for in_c
                testCase.push_back(k ? nts::Tristate::True : nts::Tristate::False);

                // Calculate expected outputs for out_0 to out_3 and out_c
                int sum = i + j + k;
                for (int bit = 0; bit < 4; ++bit) {
                    testCase.push_back((sum & (1 << bit)) ? nts::Tristate::True : nts::Tristate::False);
                }
                testCase.push_back((sum & (1 << 4)) ? nts::Tristate::True : nts::Tristate::False);

                testCases.push_back(testCase);
            }
        }
    }

    return testCases;
}

Test(four_bit_adder_tests, test_four_bit_adder)
{
    std::vector<std::vector<nts::Tristate>> testCases = generateTestCases();

    for (auto& testCase : testCases) {
        std::vector<std::string> inputNames = {"in_a1", "in_a2", "in_a3", "in_a4", "in_b1", "in_b2", "in_b3", "in_b4", "in_c"};
        for (int i = 0; i < 9; ++i) {
            std::string componentName = inputNames[i];
            nts::Tristate value = testCase[i];
            FourBitAdderManager->getComponent(componentName)->setValue(value);
        }

        FourBitAdderManager->simulate();

        std::vector<std::string> outputNames = {"out_0", "out_1", "out_2", "out_3", "out_c"};
        for (unsigned int i = 0; i < outputNames.size(); ++i) {
            std::string componentName = outputNames[i];
            nts::Tristate expectedValue = testCase[9+i];
            nts::Tristate actualValue = FourBitAdderManager->getComponent(componentName)->getValue(1);
            cr_assert_eq(actualValue, expectedValue,
                "Failed on %s with inputs in_a1=%d, in_a2=%d, in_a3=%d, in_a4=%d, in_b1=%d, in_b2=%d, in_b3=%d, in_b4=%d, in_c=%d. Expected: %d, but got: %d",
                componentName.c_str(), testCase[0], testCase[1], testCase[2], testCase[3],
                testCase[4], testCase[5], testCase[6], testCase[7], testCase[8],expectedValue, actualValue);
        }
    }
}
