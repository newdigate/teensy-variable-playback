#include <cstdint>
#include <boost/test/unit_test.hpp>
#include "utils.h"

void printTest(uint32_t expectedDataSize)
{
    auto theCase = boost::unit_test::framework::current_test_case;
    std::cout << std::endl
              << "==========================================="
              << std::endl
              << theCase().p_file_name 
              << "::" 
              << theCase().p_name 
              << "(" << expectedDataSize << ")" 
              << std::endl;
}
