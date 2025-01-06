#ifndef FEFCB3D4_A052_4411_B217_C7A8F8151994
#define FEFCB3D4_A052_4411_B217_C7A8F8151994
#include <vector>
#include <array>
#include <string>
#include <cstdint>

// see if we can use a scoped enum
// Use capital letters since not is a keyword
enum class Opcode
{
    MOV  = 0b100010,
    PUSH = 0b11111111,
    POP  = 0b10001111,
    ADD  = 0b000000,
    ADC  = 0b000100,
    INC  = 0b1111111,
    SUB  = 0b001010,
    DEC  = 0b1111111,
    DEC2 = 0b01001,
    NEG  = 0b1111011,
    CMP  = 0b001110,
    NOT  = 0b1111011,
    SHL  = 0b110100,
    SHR  = 0b110100,
    ROL  = 0b110100,
};
enum class Register
{
    AL,AH,AX,
    BL,BH,BX,
    CL,CH,CX,
    DL,DH,DX,
    SP,BP,
    SI,DI,
    ES,CS,SS,DS
};
using MemoryAddr = uint32_t;
// const auto NoOperand = 0xFFFFFFFF;
enum class OperandT
{
    Register,
    MemoryAddress,
    Immediate,
    NoOperand
};
struct Operand
{
    OperandT type;
    union{
        Register reg;
        MemoryAddr memAddr;
    };
};
struct Instruction
{
    Opcode opcode;
    std::array<Operand,5> operands;
};
std::vector<uint8_t> load_binary(std::string filename);
std::vector<std::string> instruction_to_strings(std::vector<Instruction> instructions);
std::vector<Instruction> decode_instructions(std::vector<uint8_t> binary_buffer);

#endif /* FEFCB3D4_A052_4411_B217_C7A8F8151994 */
