#ifndef FEFCB3D4_A052_4411_B217_C7A8F8151994
#define FEFCB3D4_A052_4411_B217_C7A8F8151994
#include <vector>
#include <array>
#include <string>
#include <cstdint>

// see if we can use a scoped enum
// Use capital letters since not is a keyword
// enum class Opcode
// {
//     MOV  = 0b100010,
//     PUSH = 0b11111111,
//     POP  = 0b10001111,
//     ADD  = 0b000000,
//     ADC  = 0b000100,
//     INC  = 0b1111111,
//     SUB  = 0b001010,
//     DEC  = 0b1111111,
//     DEC2 = 0b01001,
//     NEG  = 0b1111011,
//     CMP  = 0b001110,
//     NOT  = 0b1111011,
//     SHL  = 0b110100,
//     SHR  = 0b110100,
//     ROL  = 0b110100,
// };
enum class Opcode
{
    MOV1,
    MOV2,
    MOV3,
    MOV4,
    MOV5,
    MOV6,
    MOV7,
    MOV8,
    PUSH1,
    PUSH2,
    PUSH3,
    POP,
    ADD,
    ADC,
    INC,
    SUB,
    DEC,
    DEC2,
    NEG,
    CMP,
    NOT,
    SHL,
    SHR,
    ROL,
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
std::vector<uint8_t> loadBinary(std::string filename);
std::vector<std::string> instructionToStrings(std::vector<Instruction> instructions);
std::vector<Instruction> decodeInstructions(std::vector<uint8_t> binary_buffer);

#endif /* FEFCB3D4_A052_4411_B217_C7A8F8151994 */
