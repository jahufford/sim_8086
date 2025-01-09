#include "instructions.h"
#include <cassert>
#include <sstream>


std::unordered_map<Opcode, std::string> opcode_strings = {
    {Opcode::MOV1, "mov"},
    {Opcode::MOV2, "mov"},
    {Opcode::MOV3, "mov"},
    {Opcode::MOV4, "mov"},
    {Opcode::MOV5, "mov"},
    // {Opcode::MOV6, "mov"},
    // {Opcode::MOV7, "mov"},
    // {Opcode::POP1, "pop"},
    // {Opcode::POP2, "pop"},
    // {Opcode::POP3, "pop"},
    // {Opcode::ADD, "add"},
    // {Opcode::ADC, "adc"},
    // {Opcode::INC, "inc"},
    // {Opcode::SUB, "sub"},
    // {Opcode::DEC, "dec"},
    // {Opcode::DEC2, "dec"},
    // {Opcode::NEG, "neg"},
    // {Opcode::CMP, "cmp"},
    // {Opcode::NOT, "not"},
    // {Opcode::SHL, "shl"},
    // {Opcode::SHR, "shr"},
    // {Opcode::ROL, "rol"},
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

std::array<std::array<Register,8>,2> register_decoder{ 
  {
    {Register::AL,Register::CL,Register::DL,Register::BL,Register::AH,Register::CH,Register::DH,Register::BH},
    {Register::AX,Register::CX,Register::DX,Register::BX,Register::SP,Register::BP,Register::SI,Register::DI}
  }
};

EffectiveAddress createEffectiveAddress(uint8_t mod, uint8_t rm, uint16_t data)
{
    assert(mod!=0x03);
    // first index is MOD, second is R/M
    std::array<std::array<EffectiveAddress,8>,3> effective_address_decoder {{
        {//MOD = 00
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BX,.secondReg=Register::SI,.hasDisplacement=false,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BX,.secondReg=Register::DI,.hasDisplacement=false,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BP,.secondReg=Register::SI,.hasDisplacement=false,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BP,.secondReg=Register::DI,.hasDisplacement=false,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::SI,.secondReg=Register::NOREG,.hasDisplacement=false,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::DI,.secondReg=Register::NOREG,.hasDisplacement=false,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::DirectAddr,.direct_address=0},//fill in with direct address
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BX,.secondReg=Register::NOREG,.hasDisplacement=false,.is16Bit=false,.displacement=0}}
        },
        {//MOD = 01 
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BX,.secondReg=Register::SI,.hasDisplacement=true,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BX,.secondReg=Register::DI,.hasDisplacement=true,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BP,.secondReg=Register::SI,.hasDisplacement=true,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BP,.secondReg=Register::DI,.hasDisplacement=true,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::SI,.secondReg=Register::NOREG,.hasDisplacement=true,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::DI,.secondReg=Register::NOREG,.hasDisplacement=true,.is16Bit=false,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BP,.secondReg=Register::NOREG,.hasDisplacement=true,.is16Bit=false,.displacement=0}},//fill in with direct address
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BX,.secondReg=Register::NOREG,.hasDisplacement=true,.is16Bit=false,.displacement=0}}
        },
        {//MOD = 10
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BX,.secondReg=Register::SI,.hasDisplacement=true,.is16Bit=true,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BX,.secondReg=Register::DI,.hasDisplacement=true,.is16Bit=true,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BP,.secondReg=Register::SI,.hasDisplacement=true,.is16Bit=true,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BP,.secondReg=Register::DI,.hasDisplacement=true,.is16Bit=true,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::SI,.secondReg=Register::NOREG,.hasDisplacement=true,.is16Bit=true,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::DI,.secondReg=Register::NOREG,.hasDisplacement=true,.is16Bit=true,.displacement=0}},
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BP,.secondReg=Register::NOREG,.hasDisplacement=true,.is16Bit=true,.displacement=0}},//fill in with direct address
            EffectiveAddress{.type=EffectiveAddress::EAddrT::CaclulatedAddrT,.address={.firstReg=Register::BX,.secondReg=Register::NOREG,.hasDisplacement=true,.is16Bit=true,.displacement=0}}
        }
    }};
    EffectiveAddress ea;
    if(mod==0x00){
        if(rm==0b110){
            ea.type = EffectiveAddress::EAddrT::DirectAddr;
            ea.direct_address = data;
        }else{
            // has no data
            ea.type = EffectiveAddress::EAddrT::CaclulatedAddrT;
            ea = effective_address_decoder[mod][rm];
        }
    }else{
        // has data
        ea.type = EffectiveAddress::EAddrT::CaclulatedAddrT;
        ea = effective_address_decoder[mod][rm];
        if(mod==0x01){
            // 8 bit displacement, automatically sign extended to 16 bits
            // below is janky af
            int16_t signed_data = (int16_t)(*(reinterpret_cast<int8_t*>(&data)));
            // int16_t s = static_cast<int16_t>(signed_data);
            //  ea.address.data = reinterpret_cast<uint16_t>(s);
             ea.address.displacement = (uint16_t)signed_data;
            //  ea.address.data = reinterpret_cast<uint16_t>(static_cast<int16_t>(signed_data));
        }else{
            ea.address.displacement = data;    
        }
    }
    return ea;
}

std::string effectiveAddressToString(EffectiveAddress& ea)
{
    std::stringstream eas;
    eas << '[';
    if(ea.type==EffectiveAddress::EAddrT::DirectAddr){
        eas << ea.direct_address << "]";
    }else{
        eas << register_strings[ea.address.firstReg];
        if(ea.address.secondReg != Register::NOREG){
            eas << " + " << register_strings[ea.address.secondReg];
        }
        if(ea.address.hasDisplacement && ea.address.displacement!=0){
            if(ea.address.is16Bit){
                int16_t sdata = static_cast<int16_t>(ea.address.displacement);
                if(sdata>=0){
                    eas << " + " << ea.address.displacement;
                }else{
                    eas << " - " << std::abs(sdata);
                }
            }else{
                // it's 8-bits, so it's actually 16-bit signed extneded so use - instead of a + if needed
                int16_t sdata = static_cast<int16_t>(ea.address.displacement);
                if(sdata>=0){
                    eas << " + " << sdata; 
                }else{
                    // eas << " - " << static_cast<int>(val); 
                    eas << " - " << std::abs(sdata);
                }
            }
        }
        eas << "]";
    }
    return eas.str();
}
std::string instructionToString(Instruction& inst)
{
    std::stringstream instruction_sstream;
    instruction_sstream << opcode_strings[inst.opcode] << " ";
    for(auto& operand : inst.operands){
        switch(operand.type){
            case OperandT::Register:
                instruction_sstream << register_strings[operand.reg] << ", ";
            break;
            case OperandT::Immediate:
                if(operand.immediate_value.is16Bit){
                    int16_t  value = static_cast<int16_t>(operand.immediate_value.value);
                    instruction_sstream << "word " << value << ", ";
                }else{
                    int8_t  value = static_cast<int8_t>(operand.immediate_value.value);
                    if(value>=0){
                        instruction_sstream << "byte " << (int)value << ", ";
                    }else{
                        instruction_sstream << "byte -" << (int)std::abs(value) << ", ";
                    }
                }
            break;
            case OperandT::EffectiveAddress:
                instruction_sstream << effectiveAddressToString(operand.effective_address) << ", ";
            break;
            case OperandT::NoOperand:
            break;
        }
    }
    std::string str = instruction_sstream.str();
    // remove ", "
    str.pop_back();
    str.pop_back();
    return str;
}
