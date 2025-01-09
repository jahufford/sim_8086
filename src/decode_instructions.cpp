#include "decode_instructions.h"
#include "decode_opcode.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <cstdlib>
#include "instructions/instructions.h"

std::vector<uint8_t> loadBinary(std::string filename)
{
    std::ifstream fin;
    fin.open(filename, std::ios::binary);
    if(!fin){
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(1);
    }
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(fin),{});
    // file closes on exit of this function
}

std::vector<std::string> instructionsToStrings(std::vector<Instruction> instructions)
{
    std::vector<std::string> instruction_strs;
    for(auto& inst: instructions){
        instruction_strs.push_back(instructionToString(inst));
    }
    return instruction_strs;
}

std::unordered_map<Opcode, DecodeIntructionFunc> decode_instruction = 
{
    {Opcode::MOV1, decodeMOV1},
    {Opcode::MOV2, decodeMOV2},
    {Opcode::MOV3, decodeMOV3},
    {Opcode::MOV4, decodeMOV4},
    {Opcode::MOV5, decodeMOV5},
};

std::vector<Instruction> decodeInstructions(std::vector<uint8_t> binary_buffer)
{
    std::vector<Instruction> instructions;
    auto byte_emitter = binary_buffer.cbegin();
    while(byte_emitter != binary_buffer.cend())
    {
        uint8_t byte1 = (*byte_emitter++);
        Opcode opcode = decodeOpcode(byte1);

        Instruction inst = decode_instruction[opcode](byte1,byte_emitter);
        instructions.push_back(inst);
    }
    return instructions;
}