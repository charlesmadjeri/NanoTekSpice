#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "../include/paths.hpp"
#include "../src/Manager/Manager.hpp"

Test(ParsingErrorHandling, test_no_args)
{
    nts::Manager manager;
    char av1[] = "./a.out";
    char *av[] = {av1};

    cr_assert_throw(manager.parser(1, av), nts::CustomError);
}

Test(ParsingErrorHandling, test_missing_file)
{
    nts::Manager manager;
    char av1[] = "./a.out";
    char av2[] = "missing_file";
    char *av[] = {av1, av2};

    cr_assert_throw(manager.parser(2, av), nts::CustomError);
}

Test(ParsingErrorHandling, test_parsing_missing_chipset)
{
    nts::Manager manager;
    char av1[] = "./a.out";
    char av2[] = "nts_errors/no_chipset.nts";
    char *av[] = {av1, av2};

    cr_assert_throw(manager.parser(2, av), nts::CustomError);
}

Test(ParsingErrorHandling, test_parsing_missing_link)
{
    nts::Manager manager;
    char av1[] = "./a.out";
    char av2[] = "nts_errors/no_link.nts";
    char *av[] = {av1, av2};

    cr_assert_throw(manager.parser(2, av), nts::CustomError);
}

Test(ParsingErrorHandling, test_parsing_inversed_sections)
{
    nts::Manager manager;
    char av1[] = "./a.out";
    char av2[] = "nts_errors/inversed_sections.nts";
    char *av[] = {av1, av2};

    cr_assert_throw(manager.parser(2, av), nts::CustomError);
}
