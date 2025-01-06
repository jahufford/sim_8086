#include <iostream>
#include <string>
#include <cassert>
#include <utility>
#include <gtest/gtest.h>
#include "../decode_instructions.h"
#include "../opcode_decode.h"

TEST(basicTasks, testLoadBinary)
{
    std::string filename = "../src/data/0037_single_register_mov";
    auto byte_stream = load_binary(filename);
    ASSERT_EQ(byte_stream.size(),2);
    ASSERT_EQ(byte_stream.at(0),0x89);
}

TEST(basicTasks, testDecodeInstructions_0037)
{
    std::string filename = "../src/data/0037_single_register_mov";
    auto instructions = decode_instructions(load_binary(filename));
    auto instruction_str = instruction_to_strings(instructions);
    ASSERT_EQ(instruction_str.at(0), "mov cx, bx");
}

template <typename T, typename U>
std::vector<std::pair<T,U>> zip(std::vector<T>& vect1, std::vector<U>& vect2){
    std::vector<std::pair<T,U>> vect;
    assert(vect1.size()==vect2.size());
    for(size_t i=0;i<vect1.size();++i){
        vect.push_back(std::make_pair(vect1.at(i),vect2.at(i)));
    }
    return vect;
}
// creates a zipped vector of pointers so no copying is done
template <typename T, typename U>
std::vector<std::pair<const T*,const U*>> zip_p(const std::vector<T>& vect1,const std::vector<U>& vect2){
    std::vector<std::pair<const T*,const U*>> vect;
    assert(vect1.size()==vect2.size());
    for(size_t i=0;i<vect1.size();++i){
        vect.push_back(
            std::make_pair(&vect1.at(i),&vect2.at(i))
        );
    }
    return vect;
}
TEST(basicTasks, testDecodeInstructions_0038)
{
    std::string filename = "../src/data/0038_many_register_mov";
    auto instructions = decode_instructions(load_binary(filename));
    auto instruction_strs = instruction_to_strings(instructions);
    std::vector<std::string> correct_out = {
        "mov cx, bx",
        "mov ch, ah",
        "mov dx, bx",
        "mov si, bx",
        "mov bx, di",
        "mov al, cl",
        "mov ch, ch",
        "mov bx, ax",
        "mov bx, si",
        "mov sp, di",
        "mov bp, ax",
    };
    for(auto& line : zip_p(instruction_strs,correct_out)){
        ASSERT_EQ(*line.first, *line.second);
    }
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