#include <iostream>
#include <gtest/gtest.h>
#include "../myclass.h"

// int main(int arc, *argv[])
// {
//     ::testing::InitGoogleTest(&argc, argv);
//     int ret{RUN_ALL_TESTS()};
//     if(!ret){
//         std::cout << "<<<SUCCESS>>>" << std::endl;
//     }else{
//         std::cout << "FAILED" << std::endl;
//     }
// }

TEST(someTest,testTwo)
{
  ASSERT_EQ(5,5);
}

TEST(moreTests,testOne)
{
    MyClass mc;
    ASSERT_NE(mc.add(3,5),9);
    ASSERT_EQ(mc.add(-5,4),-1);
}
TEST(moreTests,testMyCLass_add)
{
    MyClass mc;
    ASSERT_NE(mc.add(3,5),9);
    ASSERT_EQ(mc.add(-5,4),-1);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret{RUN_ALL_TESTS()};
    if(!ret){
        std::cout << "<<<SUCCESS>>>" << std::endl;
    }else{
        std::cout << "FAILED" << std::endl;
    }
}