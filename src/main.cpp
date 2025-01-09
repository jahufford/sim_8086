#include <iostream>
#include <vector>
#include <cstdint>
#include <fstream>
#include "decode_instructions.h"
template <typename T>
struct TD;


int main(int argc, char *argv[])
{
    argc=argc; //suppress warning
    argv=argv; //suppress warning
    // std::string filename = "./src/data/0039_more_movs";
    // std::string filename = "./src/data/0037_single_register_mov";
    std::string filename = "./src/data/0040_challenge_movs";
    auto instructions = decodeInstructions(loadBinary(filename));
    auto instruction_strs = instructionsToStrings(instructions);
    for(auto line : instruction_strs){
        std::cout << line << std::endl;
    }
    std::vector<int> vect;

    for(auto& item: vect){
        std::cout << item << std::endl;
    }
    // write instructions to file
    // write out bits 16
}