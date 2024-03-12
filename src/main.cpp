#include "../include/paths.hpp"
#include "Manager/Manager.hpp"

int main(int ac, char **av) {
    try {
        nts::Manager manager;
        manager.initializeTruthTables(TRUTH_TABLE_PATH);
        manager.preParse(ac, av);
        manager.initGates(PRIMARY_GATES_PATH);
        manager.initGates(SECONDARY_GATES_PATH);
        manager.parser(ac, av);
        manager.initConstantsLinks();
        manager.run();
    } catch (const nts::CustomError &e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }
    return 0;
}
