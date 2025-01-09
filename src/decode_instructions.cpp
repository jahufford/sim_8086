#include "decode_instructions.h"
#include "decode_opcode.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <cstdlib>
#include <iterator>
#include <cstddef>
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

class InstructionStreamer
{
private:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = Instruction;
    using pointer           = value_type const*;
    using reference         = value_type const&;


    Instruction current_instruction;
    std::vector<uint8_t>::const_iterator byte_emitter;
    std::vector<uint8_t>::const_iterator byte_end;
    void getAndDecodeInstruction(){
        uint8_t byte1 = (*byte_emitter++);
        Opcode opcode = decodeOpcode(byte1);
        current_instruction = decode_instruction[opcode](byte1,byte_emitter);
    }
public:
    InstructionStreamer(std::vector<uint8_t>& binary_buffer){
        this->byte_emitter = binary_buffer.cbegin();
        this->byte_end = binary_buffer.cend();
        getAndDecodeInstruction();
    }
    explicit operator bool() const { return this->byte_emitter!=this->byte_end;}
    reference operator*()const{return current_instruction;}
    pointer operator->() const{ return &current_instruction;}
    InstructionStreamer& operator++(){
        assert(byte_emitter!=byte_end);
        getAndDecodeInstruction();
        return *this;
    }
    InstructionStreamer operator++(int){
        InstructionStreamer const tmp(*this);
        ++*this;
        return tmp;
    }
};

std::vector<Instruction> decodeInstructions(std::vector<uint8_t> binary_buffer)
{
    std::vector<Instruction> instructions;
    
    // // streaming interface so you can interlace decoding of instructions with running of instructions
    // auto streamer = InstructionStreamer(binary_buffer);
    // while(streamer)
    // {
    //     Instruction instruction = (*streamer++);
    //     ExecuteInstruction(instruction);
    // }

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