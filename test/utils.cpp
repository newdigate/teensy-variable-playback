#include <cstdint>
#include <boost/test/unit_test.hpp>
#include "utils.h"

static int count;
void printTest(uint32_t expectedDataSize,double rate)
{
    auto theCase = boost::unit_test::framework::current_test_case;
    //std::string p_file_name;
    auto p_file_name = theCase().p_file_name;
/*
    auto s = p_file_name.rfind("/");
    auto e = p_file_name.rfind(".cpp");
    p_file_name = p_file_name.substr(s+1,e-s);
//*/

    std::cout << std::endl
              << "================================== "
              << ++count
              << " =================================="
              << std::endl
              << p_file_name 
              << "::" 
              << theCase().p_name 
              << "(" 
              << (0 == expectedDataSize?"":std::to_string(expectedDataSize))  
              << (NO_RATE == rate?"":" @ ")
              << (NO_RATE == rate?"":std::to_string(rate))
              << ")"
              << std::endl;
}


