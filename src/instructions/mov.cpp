#include "mov.h"
#include "instructions.h"
#include <iostream>

Instruction decodeMOV1(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter)
{
    // Reigster/memory to/from register
    uint8_t D = (byte1 & 0x02)>>1; // direction 0 REG is source, 1 REG is destination
    uint8_t W = byte1 & 0x01; // 0 = byte, 1 = word
    uint8_t byte2 = (*byte_emitter++);
    uint8_t MOD = (byte2 & 0b11000000)>>6; // whether one is memory or both are registers
    // 00 memory mode no displacement (except when RM = 110), 01 memory mode 8-bit displacement
    // 10 memory mode 16-bit displacement, 11 register mode no displacement
    uint8_t REG = (byte2 & 0b00111000)>>3;
    uint8_t RM = byte2 & 0b00000111;
    Instruction instruction;
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
        instruction = {
            .opcode=Opcode::MOV1,
            .operands={
                Operand{.type=OperandT::Register, .reg=dst_reg},
                Operand{.type=OperandT::Register, .reg=src_reg},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0}
            }
        };
        // std::cout << instructionToString(instruction) << std::endl;
        // instructions.push_back(instruction);
    }else if(RM==0x06 && MOD==0x00){
        // direct address
        uint16_t daddress = (*byte_emitter++);
        uint16_t hb_daddress = (*byte_emitter++);
        hb_daddress <<= 8;
        daddress += hb_daddress;
        Register reg = register_decoder[W][REG];
        EffectiveAddress ea = createEffectiveAddress(MOD,RM,daddress);
        if(D==0){ // REG is source
            instruction = {
                .opcode=Opcode::MOV1,
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
                .opcode=Opcode::MOV1,
                .operands={
                    Operand{.type=OperandT::Register, .reg=reg},
                    Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                    Operand{.type=OperandT::NoOperand,.noValue=0},
                    Operand{.type=OperandT::NoOperand,.noValue=0},
                    Operand{.type=OperandT::NoOperand,.noValue=0}
                }
            };
        }
        // std::cout << instructionToString(instruction) << std::endl;
        // instructions.push_back(instruction);
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
        if(D==0){ // REG is source
            instruction = {
                .opcode=Opcode::MOV1,
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
                .opcode=Opcode::MOV1,
                .operands={
                    Operand{.type=OperandT::Register, .reg=reg},
                    Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                    Operand{.type=OperandT::NoOperand,.noValue=0},
                    Operand{.type=OperandT::NoOperand,.noValue=0},
                    Operand{.type=OperandT::NoOperand,.noValue=0}
                }
            };
        }
    }
    #ifdef PRINT_INSTRUCTIONS_AS_YOU_GO
        std::cout << instructionToString(instruction) << std::endl;
    #endif
    return instruction;
}

Instruction decodeMOV2(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter)
{
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
            .opcode=Opcode::MOV2,
            .operands={
                Operand{.type=OperandT::Register, .reg=register_decoder[W][RM]},
                Operand{.type=OperandT::Immediate,.immediate_value={.is16Bit=(W?true:false),.value=data}},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0}
            }
        };
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
                .opcode=Opcode::MOV2,
                .operands={
                    Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                    Operand{.type=OperandT::Immediate,.immediate_value={.is16Bit=(W?true:false),.value=value}},
                    Operand{.type=OperandT::NoOperand,.noValue=0},
                    Operand{.type=OperandT::NoOperand,.noValue=0},
                    Operand{.type=OperandT::NoOperand,.noValue=0}
                }
            };
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
                .opcode=Opcode::MOV2,
                .operands={
                    Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                    Operand{.type=OperandT::Immediate,.immediate_value={.is16Bit=(W?true:false),.value=value}},
                    Operand{.type=OperandT::NoOperand,.noValue=0},
                    Operand{.type=OperandT::NoOperand,.noValue=0},
                    Operand{.type=OperandT::NoOperand,.noValue=0}
                }
            };
        }
    }
    #ifdef PRINT_INSTRUCTIONS_AS_YOU_GO
        std::cout << instructionToString(instruction) << std::endl;
    #endif
    return instruction;
}

Instruction decodeMOV3(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter)
{
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
        .opcode=Opcode::MOV3,
        .operands={
            Operand{.type=OperandT::Register, .reg=register_decoder[W][REG]},
            Operand{.type=OperandT::Immediate,.immediate_value={.is16Bit=(W?true:false),.value=value}},
            Operand{.type=OperandT::NoOperand,.noValue=0},
            Operand{.type=OperandT::NoOperand,.noValue=0},
            Operand{.type=OperandT::NoOperand,.noValue=0}
        }
    };
    #ifdef PRINT_INSTRUCTIONS_AS_YOU_GO
        std::cout << instructionToString(instruction) << std::endl;
    #endif
    return instruction;
}

Instruction decodeMOV4(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter)
{
    // memory to accumulator
    uint8_t W = byte1 & 0x01; // 0 = byte, 1 = word
    uint16_t address = (*byte_emitter++);
    uint16_t hb_address = (*byte_emitter++);
    hb_address <<= 8;
    address += hb_address;
    Register reg = (W ? Register::AX : Register::AL);
    EffectiveAddress ea = createEffectiveAddress(0,6,address); // use direct address for ea
    Instruction instruction {
        .opcode=Opcode::MOV4,
        .operands={
            Operand{.type=OperandT::Register, .reg=reg},
            // Operand{.type=OperandT::EffectiveAddress,.effective_address{.type=EffectiveAddress::EAddrT::DirectAddr,.direct_address=address}},
            Operand{.type=OperandT::EffectiveAddress,.effective_address=ea},
            Operand{.type=OperandT::NoOperand,.noValue=0},
            Operand{.type=OperandT::NoOperand,.noValue=0},
            Operand{.type=OperandT::NoOperand,.noValue=0}
        }
    };
    #ifdef PRINT_INSTRUCTIONS_AS_YOU_GO
        std::cout << instructionToString(instruction) << std::endl;
    #endif
    return instruction;
}


Instruction decodeMOV5(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter)
{
    // accumulator to memory
    uint8_t W = byte1 & 0x01; // 0 = byte, 1 = word
    uint16_t address = (*byte_emitter++);
    uint16_t hb_address = (*byte_emitter++);
    hb_address <<= 8;
    address += hb_address;
    Register reg = (W ? Register::AX : Register::AL);
    EffectiveAddress ea = createEffectiveAddress(0,6,address); // use direct address for ea
    Instruction instruction {
        .opcode=Opcode::MOV5,
        .operands={
            Operand{.type=OperandT::EffectiveAddress,.effective_address=ea},
            // Operand{.type=OperandT::EffectiveAddress,.effective_address{.type=EffectiveAddress::EAddrT::DirectAddr,.direct_address=address}},
            Operand{.type=OperandT::Register, .reg=reg},
            Operand{.type=OperandT::NoOperand,.noValue=0},
            Operand{.type=OperandT::NoOperand,.noValue=0},
            Operand{.type=OperandT::NoOperand,.noValue=0}
        }
    };
    #ifdef PRINT_INSTRUCTIONS_AS_YOU_GO
        std::cout << instructionToString(instruction) << std::endl;
    #endif
    return instruction;
}