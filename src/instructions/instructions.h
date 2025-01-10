#ifndef E1A250DB_764D_4CBF_9D35_C539D945EB20
#define E1A250DB_764D_4CBF_9D35_C539D945EB20

#include <array>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <string>
#define PRINT_INSTRUCTIONS_AS_YOU_GO
enum  class Opcode
{
    MOV1,
    MOV2,
    MOV3,
    MOV4,
    MOV5,
    ADDSUBCMP2,
    ADD1,
    ADD2,
    ADD3,
    SUB1,
    SUB2,
    SUB3,
    CMP1,
    CMP2,
    CMP3,
    JNZ,
    JE,
    JL,
    JLE,
    JB,
    JBE,
    JP,
    JO,
    JS,
    JNL,
    JG,
    JNB,
    JA,
    JNP,
    JNO,
    JNS,
    LOOP,
    LOOPZ,
    LOOPNZ,
    JCXZ,

    // MOV6,
    // MOV7,
    // PUSH1,
    // PUSH2,
    // PUSH3,
    // POP1,
    // POP2,
    // POP3,
    // XCHG1,
    // XCHG2,
    // ADD,
    // ADC,
    // INC,
    // SUB,
    // DEC,
    // DEC2,
    // NEG,
    // CMP,
    // NOT,
    // SHL,
    // SHR,
    // ROL,
};
enum class Register
{
    AL,AH,AX,
    BL,BH,BX,
    CL,CH,CX,
    DL,DH,DX,
    SP,BP,
    SI,DI,
    ES,CS,SS,DS,NOREG
};
using MemoryAddr = uint32_t;
struct ImmediateValue
{
    bool is16Bit;
    uint16_t value;
};
// const auto NoOperand = 0xFFFFFFFF;
enum class OperandT
{
    Register,
    // MemoryAddress,
    EffectiveAddress,
    Immediate,
    NoOperand
};
struct EffectiveAddress
{
    enum class EAddrT{
        CaclulatedAddrT,
        DirectAddr
    };
    EAddrT type;
    union{
        struct {
            Register firstReg;
            Register secondReg;
            bool hasDisplacement;
            bool is16Bit;
            uint16_t displacement;
        } address;
        uint16_t direct_address;
    };
};
struct Operand
{
    OperandT type;
    union{
        Register reg;
        MemoryAddr memAddr;
        EffectiveAddress effective_address;
        ImmediateValue immediate_value;
        uint8_t noValue; // to get rid of compiler warning
    };
};
struct Instruction
{
    Opcode opcode;
    std::array<Operand,5> operands;
};

// Functions that decode insructions have the following form
//byte 1 is the byte that has the opcode
typedef Instruction (*DecodeIntructionFunc)(uint8_t byte1 ,std::vector<uint8_t>::const_iterator& byte_emitter);


extern std::unordered_map<Opcode, std::string> opcode_strings;
extern std::unordered_map<Register, std::string> register_strings;
// first demension is from the w bit
extern std::array<std::array<Register,8>,2> register_decoder;

EffectiveAddress createEffectiveAddress(uint8_t mod, uint8_t rm, uint16_t data);
std::string effectiveAddressToString(EffectiveAddress& ea);
std::string instructionToString(Instruction& inst);

#include "mov.h"
#include "addsubcmp.h"
#include "jmp.h"

#endif /* E1A250DB_764D_4CBF_9D35_C539D945EB20 */
