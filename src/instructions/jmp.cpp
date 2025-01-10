#include "jmp.h"
#include <iostream>
#include <unordered_map>

static std::unordered_map<uint8_t, Opcode> jmp_decoder = {
    {0x70, Opcode::JO},
    {0x71, Opcode::JNO},
    {0x72, Opcode::JB},
    {0x73, Opcode::JNB},
    {0x74, Opcode::JE},
    {0x75, Opcode::JNZ},
    {0x76, Opcode::JBE},
    {0x77, Opcode::JA},
    {0x78, Opcode::JS},
    {0x79, Opcode::JNS},
    {0x7A, Opcode::JP},
    {0x7B, Opcode::JNP},
    {0x7C, Opcode::JL},
    {0x7D, Opcode::JNL},
    {0x7E, Opcode::JLE},
    {0x7F, Opcode::JG},
    {0xE0, Opcode::LOOPNZ},
    {0xE1, Opcode::LOOPZ},
    {0xE2, Opcode::LOOP},
    {0xE3, Opcode::JCXZ},
};

Instruction decodeJMP_s(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter)
{
    std::cout << "jmp_s" << std::endl;
    Opcode opcode = jmp_decoder[byte1];
    uint8_t jmp_offset = (*byte_emitter++);
    Instruction instruction = {
        .opcode=opcode,
        .operands={
            Operand{.type=OperandT::Immediate,.immediate_value={.is16Bit=false,.value=jmp_offset}},
            Operand{.type=OperandT::NoOperand,.noValue=0},
            Operand{.type=OperandT::NoOperand,.noValue=0},
            Operand{.type=OperandT::NoOperand,.noValue=0},
            Operand{.type=OperandT::NoOperand,.noValue=0},
        }
    };
    
    #ifdef PRINT_INSTRUCTIONS_AS_YOU_GO
        std::cout << instructionToString(instruction) << std::endl;
    #endif
   return instruction;
}