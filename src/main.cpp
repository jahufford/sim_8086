#include <iostream>
#include <gtest/gtest.h>

#include "myclass.h"

int main(int argc, char *argv[])
{
    // int x = TESTING;
// #ifdef TESTING
//     ::testing::InitGoogleTest(&argc, argv);
//     int ret{RUN_ALL_TESTS()};
//     if(!ret){
//         std::cout << "<<<SUCCESS>>>" << std::endl;
//     }else{
//         std::cout << "FAILED" << std::endl;
//     }
// #else
    std::cout << "Hello world!" << std::endl;
    MyClass mc;
    mc.foo();
// #endif
}
// TEST(someTest,testOne)
// {
//   ASSERT_EQ(5,5);
// }