#include "decode_instructions.h"
#include "decode_opcode.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <sstream>

std::unordered_map<Opcode, std::string> instruction_strings = {
    {Opcode::MOV1, "mov"},
    {Opcode::MOV2, "mov"},
    {Opcode::MOV3, "mov"},
    {Opcode::MOV4, "mov"},
    {Opcode::MOV5, "mov"},
    {Opcode::MOV6, "mov"},
    {Opcode::MOV7, "mov"},
    {Opcode::MOV8, "mov"},
    {Opcode::POP, "pop"},
    {Opcode::ADD, "add"},
    {Opcode::ADC, "adc"},
    {Opcode::INC, "inc"},
    {Opcode::SUB, "sub"},
    {Opcode::DEC, "dec"},
    {Opcode::DEC2, "dec"},
    {Opcode::NEG, "neg"},
    {Opcode::CMP, "cmp"},
    {Opcode::NOT, "not"},
    {Opcode::SHL, "shl"},
    {Opcode::SHR, "shr"},
    {Opcode::ROL, "rol"},
};
std::unordered_map<Register, std::string> register_strings = {
    {Register::AL, "al"},{Register::AH, "ah"},{Register::AX, "ax"},
    {Register::BL, "bl"},{Register::BH, "bh"},{Register::BX, "bx"},
    {Register::CL, "cl"},{Register::CH, "ch"},{Register::CX, "cx"},
    {Register::DL, "dl"},{Register::DH, "dh"},{Register::DX, "dx"},
    {Register::SP, "sp"},{Register::BP, "bp"},
    {Register::SI, "si"},{Register::DI, "di"},
    {Register::ES, "es"},{Register::CS, "cs"},{Register::SS, "ss"},{Register::DS, "ds"},
};
// first demension is from the w bit
std::array<std::array<Register,8>,2> register_decoder{ 
  {
    {Register::AL,Register::CL,Register::DL,Register::BL,Register::AH,Register::CH,Register::DH,Register::BH},
    {Register::AX,Register::CX,Register::DX,Register::BX,Register::SP,Register::BP,Register::SI,Register::DI}
  }
};

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
std::vector<std::string> instructionToStrings(std::vector<Instruction> instructions)
{
    std::vector<std::string> instruction_strs;
    for(auto& inst: instructions){
        std::stringstream instruction_sstream;
        instruction_sstream << instruction_strings[inst.opcode] << " ";
        for(auto& operand : inst.operands){
            if(operand.type == OperandT::Register){
                instruction_sstream << register_strings[operand.reg] << ", ";
            }else if(operand.type == OperandT::MemoryAddress){
                instruction_sstream << std::hex << operand.memAddr << ", ";
            }
        }
        std::string str = instruction_sstream.str();
        // remove ", "
        str.pop_back();
        str.pop_back();
        instruction_strs.push_back(str);
    }
    return instruction_strs;
}
std::vector<Instruction> decodeInstructions(std::vector<uint8_t> binary_buffer)
{
    // std::vector<std::string> instructions;
    std::vector<Instruction> instructions;
    auto byte_emitter = binary_buffer.cbegin();
    while(byte_emitter != binary_buffer.cend())
    {
        uint8_t byte1 = (*byte_emitter++);
        // Opcode opcode = static_cast<Opcode>(byte1 >> 2); // grab 6 msb's
        Opcode opcode = decode_opcode(byte1);
        // check if it's a single byte opcode
        // if so then handle it and continue
        uint8_t D = (byte1 & 0x02)>>1; // direction 0 REG is source, 1 REG is destination
        uint8_t W = byte1 & 0x01; // 0 = byte, 1 = word
        uint8_t byte2 = (*byte_emitter++);
        uint8_t MOD = (byte2 & 0b11000000)>>6; // whether one is memory or both are registers
        // 00 memory mode no displacement (except when RM = 110), 01 memory mode 8-bit displacement
        // 10 memory mode 16-bit displacement, 11 register mode no displacement
        uint8_t REG = (byte2 & 0b00111000)>>3;
        uint8_t RM = byte2 & 0b00000111;
        switch(opcode){
            case Opcode::MOV1:
                if(MOD==0x03){
                    // register to register
                    Register src_reg, dst_reg;
                    if(D){
                        src_reg = register_decoder[W][RM];
                        dst_reg = register_decoder[W][REG];
                    }else{
                        src_reg = register_decoder[W][REG];
                        dst_reg = register_decoder[W][RM];
                    }
                    // register to register
                    Instruction instruction {
                        .opcode=opcode,
                        .operands={Operand{.type=OperandT::Register, .reg=dst_reg},
                                   Operand{.type=OperandT::Register, .reg=src_reg},
                                   Operand{.type=OperandT::NoOperand},
                                   Operand{.type=OperandT::NoOperand},
                                   Operand{.type=OperandT::NoOperand}
                                   }
                    };
                    instructions.push_back(instruction);
                }
            break;
        }
    }
    return instructions;
}