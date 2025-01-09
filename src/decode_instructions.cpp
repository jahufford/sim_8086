#include "decode_instructions.h"
#include "decode_opcode.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <cassert>
#include <cstdlib>

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
// first demension is from the w bit
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

std::vector<std::string> instructionsToStrings(std::vector<Instruction> instructions)
{
    std::vector<std::string> instruction_strs;
    for(auto& inst: instructions){
        instruction_strs.push_back(instructionToString(inst));
    }
    return instruction_strs;
}
std::vector<Instruction> decodeInstructions(std::vector<uint8_t> binary_buffer)
{
    std::vector<Instruction> instructions;
    auto byte_emitter = binary_buffer.cbegin();
    while(byte_emitter != binary_buffer.cend())
    {
        uint8_t byte1 = (*byte_emitter++);
        // Opcode opcode = static_cast<Opcode>(byte1 >> 2); // grab 6 msb's
        Opcode opcode = decodeOpcode(byte1);
        // std::cout << std::hex << (int)byte1 << std::endl;
        // std::cout << opcode_strings[opcode] << std::endl;

        // check if it's a single byte opcode
        // if so then handle it and continue
        switch(opcode){
            case Opcode::MOV1: {
                // Reigster/memory to/from register
                uint8_t D = (byte1 & 0x02)>>1; // direction 0 REG is source, 1 REG is destination
                uint8_t W = byte1 & 0x01; // 0 = byte, 1 = word
                uint8_t byte2 = (*byte_emitter++);
                uint8_t MOD = (byte2 & 0b11000000)>>6; // whether one is memory or both are registers
                // 00 memory mode no displacement (except when RM = 110), 01 memory mode 8-bit displacement
                // 10 memory mode 16-bit displacement, 11 register mode no displacement
                uint8_t REG = (byte2 & 0b00111000)>>3;
                uint8_t RM = byte2 & 0b00000111;
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
                        .operands={
                            Operand{.type=OperandT::Register, .reg=dst_reg},
                            Operand{.type=OperandT::Register, .reg=src_reg},
                            Operand{.type=OperandT::NoOperand,.noValue=0},
                            Operand{.type=OperandT::NoOperand,.noValue=0},
                            Operand{.type=OperandT::NoOperand,.noValue=0}
                        }
                    };
                    std::cout << instructionToString(instruction) << std::endl;
                    instructions.push_back(instruction);
                }else if(RM==0x06 && MOD==0x00){
                    // direct address
                    uint16_t daddress = (*byte_emitter++);
                    uint16_t hb_daddress = (*byte_emitter++);
                    hb_daddress <<= 8;
                    daddress += hb_daddress;
                    Register reg = register_decoder[W][REG];
                    EffectiveAddress ea = createEffectiveAddress(MOD,RM,daddress);
                    Instruction instruction;
                    if(D==0){ // REG is source
                        instruction = {
                            .opcode=opcode,
                            .operands={
                                Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                                Operand{.type=OperandT::Register, .reg=reg},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0}
                            }
                        };
                    }else{ // REG is destination
                        instruction = {
                            .opcode=opcode,
                            .operands={
                                Operand{.type=OperandT::Register, .reg=reg},
                                Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0}
                            }
                        };
                    }
                    std::cout << instructionToString(instruction) << std::endl;
                    instructions.push_back(instruction);
                }else{
                    //memory mode
                    uint16_t data = 0;
                    if(MOD==0x01){
                        data = (*byte_emitter++);
                    }else if(MOD==0x02){
                        uint16_t tmp = (*byte_emitter++);
                        data = (*byte_emitter++);
                        data <<= 8;
                        data += tmp;
                    }

                    Register reg = register_decoder[W][REG];
                    EffectiveAddress ea = createEffectiveAddress(MOD,RM,data);
                    Instruction instruction;
                    if(D==0){ // REG is source
                        instruction = {
                            .opcode=opcode,
                            .operands={
                                Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                                Operand{.type=OperandT::Register, .reg=reg},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0}
                            }
                        };
                    }else{ // REG is destination
                        instruction = {
                            .opcode=opcode,
                            .operands={
                                Operand{.type=OperandT::Register, .reg=reg},
                                Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0}
                            }
                        };
                    }
                    std::cout << instructionToString(instruction) << std::endl;
                    instructions.push_back(instruction);
                }
            }
            break;
            case Opcode::MOV2: {
                // immediate to register or memory
                uint8_t W = byte1 & 0x01; // 0 = byte, 1 = word
                uint8_t byte2 = (*byte_emitter++);
                uint8_t MOD = (byte2 & 0b11000000)>>6; // whether one is memory or both are registers
                // 00 memory mode no displacement (except when RM = 110), 01 memory mode 8-bit displacement
                // 10 memory mode 16-bit displacement, 11 register mode no displacement
                // uint8_t REG = (byte2 & 0b00111000)>>3;
                uint8_t RM = byte2 & 0b00000111;
                Instruction instruction;
                if(MOD==0x3){
                    // to register
                    uint16_t data = (*byte_emitter++);
                    if(W){
                        uint16_t high_byte = (*byte_emitter++);
                        high_byte <<= 8;
                        data += high_byte;
                    }
                    instruction = {
                        .opcode=opcode,
                        .operands={
                            Operand{.type=OperandT::Register, .reg=register_decoder[W][RM]},
                            Operand{.type=OperandT::Immediate,.immediate_value={.is16Bit=(W?true:false),.value=data}},
                            Operand{.type=OperandT::NoOperand,.noValue=0},
                            Operand{.type=OperandT::NoOperand,.noValue=0},
                            Operand{.type=OperandT::NoOperand,.noValue=0}
                        }
                    };
                    std::cout << instructionToString(instruction) << std::endl;
                    instructions.push_back(instruction);
                }else{
                    // to memory
                    // need to calculate effective address
                    if(MOD==0x01 || MOD==0x02){
                        // 1 - 8-bit displacement
                        // 2 -  16-bit displacement
                        uint16_t displacement = (*byte_emitter++);
                        if(MOD==0x02){
                            uint16_t hb_disp = (*byte_emitter++);
                            hb_disp <<= 8;
                            displacement += hb_disp;
                        }
                        uint16_t value = (*byte_emitter++);
                        if(W){
                            uint16_t hb_value = (*byte_emitter++);
                            hb_value <<= 8;
                            value += hb_value;
                        }
                        EffectiveAddress ea = createEffectiveAddress(MOD,RM,displacement);

                        instruction = {
                            .opcode=opcode,
                            .operands={
                                Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                                Operand{.type=OperandT::Immediate,.immediate_value={.is16Bit=(W?true:false),.value=value}},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0}
                            }
                        };
                        std::cout << instructionToString(instruction) << std::endl;
                        instructions.push_back(instruction);
                    }else if(MOD==000){
                        // no displacement
                        uint16_t value = (*byte_emitter++);
                        if(W){
                            uint16_t high_byte = (*byte_emitter++);
                            high_byte <<= 8;
                            value += high_byte;
                        }
                        EffectiveAddress ea = createEffectiveAddress(MOD,RM,0);

                        instruction = {
                            .opcode=opcode,
                            .operands={
                                Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                                Operand{.type=OperandT::Immediate,.immediate_value={.is16Bit=(W?true:false),.value=value}},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0},
                                Operand{.type=OperandT::NoOperand,.noValue=0}
                            }
                        };
                        std::cout << instructionToString(instruction) << std::endl;
                        instructions.push_back(instruction);
                    }
                }
            }
            break;
            case Opcode::MOV3: {
                // immediate to register
                uint8_t W = (byte1 & (uint8_t)0b00001000)>>3; // 0 = byte, 1 = word, need the cast for some reason
                uint8_t REG = (byte1 & 0b00000111);
                uint8_t data[2] = {0,0};
                data[0] = (*byte_emitter);
                ++byte_emitter;
                if(W==1){
                    // 16-bit
                    data[1] = (*byte_emitter);
                    ++byte_emitter;
                }
                uint16_t value = data[1];
                value <<= 8;
                value += data[0];
                Instruction instruction {
                    .opcode=opcode,
                    .operands={
                        Operand{.type=OperandT::Register, .reg=register_decoder[W][REG]},
                        Operand{.type=OperandT::Immediate,.immediate_value={.is16Bit=(W?true:false),.value=value}},
                        Operand{.type=OperandT::NoOperand,.noValue=0},
                        Operand{.type=OperandT::NoOperand,.noValue=0},
                        Operand{.type=OperandT::NoOperand,.noValue=0}
                    }
                };
                std::cout << instructionToString(instruction) << std::endl;
                instructions.push_back(instruction);
            }
            break;
            case Opcode::MOV4:{
                // memory to accumulator
                uint8_t W = byte1 & 0x01; // 0 = byte, 1 = word
                uint16_t address = (*byte_emitter++);
                uint16_t hb_address = (*byte_emitter++);
                hb_address <<= 8;
                address += hb_address;
                Register reg = (W ? Register::AX : Register::AL);
                EffectiveAddress ea = createEffectiveAddress(0,6,address); // use direct address for ea
                Instruction instruction {
                    .opcode=opcode,
                    .operands={
                        Operand{.type=OperandT::Register, .reg=reg},
                        Operand{.type=OperandT::EffectiveAddress,.effective_address{.type=EffectiveAddress::EAddrT::DirectAddr,.direct_address=address}},
                        Operand{.type=OperandT::NoOperand,.noValue=0},
                        Operand{.type=OperandT::NoOperand,.noValue=0},
                        Operand{.type=OperandT::NoOperand,.noValue=0}
                    }
                };
                std::cout << instructionToString(instruction) << std::endl;
                instructions.push_back(instruction);
            }
            break;
            case Opcode::MOV5:{
                // accumulator to memory
                uint8_t W = byte1 & 0x01; // 0 = byte, 1 = word
                uint16_t address = (*byte_emitter++);
                uint16_t hb_address = (*byte_emitter++);
                hb_address <<= 8;
                address += hb_address;
                Register reg = (W ? Register::AX : Register::AL);
                EffectiveAddress ea = createEffectiveAddress(0,6,address); // use direct address for ea
                Instruction instruction {
                    .opcode=opcode,
                    .operands={
                        Operand{.type=OperandT::EffectiveAddress,.effective_address{.type=EffectiveAddress::EAddrT::DirectAddr,.direct_address=address}},
                        Operand{.type=OperandT::Register, .reg=reg},
                        Operand{.type=OperandT::NoOperand,.noValue=0},
                        Operand{.type=OperandT::NoOperand,.noValue=0},
                        Operand{.type=OperandT::NoOperand,.noValue=0}
                    }
                };
                std::cout << instructionToString(instruction) << std::endl;
                instructions.push_back(instruction);

            }
        }
    }
    return instructions;
}