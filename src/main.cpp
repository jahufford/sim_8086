#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include <cassert>
#include "decode_instructions.h"
#include <array>
template <typename T>
struct TD;


int main(int argc, char *argv[])
{
    std::string filename = "./src/data/0038_many_register_mov";
    // std::string filename = "./src/data/0037_single_register_mov";
    auto instructions = decode_instructions(load_binary(filename));
    auto instruction_strs = instruction_to_strings(instructions);
    for(auto line : instruction_strs){
        std::cout << line << std::endl;
    }
    // write instructions to file
    // write out bits 16
}