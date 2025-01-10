#include "addsubcmp.h"
#include <iostream>
#include <unordered_map>

static std::unordered_map<uint8_t,Opcode> decodeASC = {
        {0x00, Opcode::ADD1},
        {0x28, Opcode::SUB1},
        {0x38, Opcode::CMP1},
        {0x04, Opcode::ADD3},
        {0x2C, Opcode::SUB3},
        {0x3C, Opcode::CMP3},
};

Instruction decodeADDSUBCMP1(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter)
{
    Opcode opcode = decodeASC[(byte1&0b11111100)];
    uint8_t W = byte1&01;
    uint8_t D = (byte1&02)>>1;
    uint8_t byte2 = (*byte_emitter++);
    uint8_t MOD = (byte2&0b11000000)>>6;
    uint8_t REG = (byte2&0b00111000)>>3;
    uint8_t RM  = (byte2&0b00000111);
    Instruction instruction;
    if(MOD == 0x03){
        // register to register
        // D=0 reg is src, d=1 reg is dest
        Register src_reg,dest_reg;
        if(D){
            src_reg = register_decoder[W][RM];
            dest_reg = register_decoder[W][REG];
        }else{
            src_reg = register_decoder[W][REG];
            dest_reg = register_decoder[W][RM];
        }
        instruction = {
            .opcode=opcode,
            .operands={
                Operand{.type=OperandT::Register,.reg=dest_reg},
                Operand{.type=OperandT::Register,.reg=src_reg},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0},
            }
        };
    }else{
        // memory mode
        uint16_t data_or_address = 0;

        if(RM==0x06 && MOD==0x00){
            // direct address
            data_or_address = (*byte_emitter++);
            uint16_t hb_address = (*byte_emitter++);
            hb_address <<= 8;
            data_or_address += hb_address; 
        }else if(MOD==0x01){ // 8 bit
            data_or_address = (*byte_emitter++);
        }else if(MOD==0x02){ // 16 bit
            uint16_t tmp = (*byte_emitter++);
            data_or_address = (*byte_emitter++);
            data_or_address <<= 8;
            data_or_address += tmp;
        }
        Register reg = register_decoder[W][REG];
        EffectiveAddress ea = createEffectiveAddress(MOD,RM,data_or_address);
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
    }

    #ifdef PRINT_INSTRUCTIONS_AS_YOU_GO
        std::cout << instructionToString(instruction) << std::endl;
    #endif
    return instruction;
}

static std::unordered_map<uint8_t, Opcode> opcode_selector = {
    {0x00, Opcode::ADD2},
    {0x05, Opcode::SUB2},
    {0x07, Opcode::CMP2},
};

Instruction decodeADDSUBCMP2(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter)
{
    // immediate to register/memory
    uint8_t W = byte1 & 0x01;
    uint8_t S = (byte1 & 0x02)>>1;
    uint8_t byte2 = (*byte_emitter++);
    // further decoding which opcode
    uint8_t selector = (byte2&0b00111000)>>3;
    Opcode opcode = opcode_selector[selector];

    uint8_t MOD = (byte2 & 0b11000000)>>6;
    uint8_t RM  = (byte2 & 0b00000111);

    Register reg = register_decoder[W][RM];
    Instruction instruction;
    if(MOD==0){
        // no displacement
        uint16_t address = 0;
        if(RM==0b110){ // direct address
            address = (*byte_emitter++);
            uint16_t hb_address = (*byte_emitter++);
            hb_address <<= 8;
            address += hb_address;
        }
        uint16_t value = (*byte_emitter++);
        if(S==0 && W==1){
            uint16_t hb_value = (*byte_emitter++);
            hb_value <<= 8;
            value += hb_value;
        }

        EffectiveAddress ea = createEffectiveAddress(MOD,RM,address);

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
        
    }else if(MOD==0x01 || MOD==0x02){
        // MOD = 01 8 bit displacement
        // MOD = 02 16 bit displacement

            uint16_t displacement = (*byte_emitter++);
            if(MOD==0x02){
                uint16_t hb_disp = (*byte_emitter++);
                hb_disp <<= 8;
                displacement += hb_disp;
            }
            uint16_t value = (*byte_emitter++);
            if(S==0 && W==1){
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

    }else if(MOD==0x03){
        // no effective address
        uint16_t value = (*byte_emitter++);
        if(W==1 && S==0){
            uint16_t high_byte = (*byte_emitter++);
            high_byte <<= 8;
            value += high_byte;
        }else if(W==1 && S==1){
            // sign extend 8-bit to 16-bit
            int16_t signed_data = (int16_t)(*(reinterpret_cast<int8_t*>(&value)));
            value = (uint16_t)signed_data;
        }
        instruction = {
            .opcode=opcode,
            .operands={
                Operand{.type=OperandT::Register, .reg=reg},
                Operand{.type=OperandT::Immediate,.immediate_value={.is16Bit=(W?true:false),.value=value}},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0}
            }
        };
    }
    #ifdef PRINT_INSTRUCTIONS_AS_YOU_GO
        std::cout << instructionToString(instruction) << std::endl;
    #endif
    return instruction;
}


Instruction decodeADDSUBCMP3(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter)
{
    Opcode opcode = decodeASC[(byte1&0b11111110)];
    uint8_t W = byte1 & 0x01;
    Register reg;
    uint16_t value;
        value = (*byte_emitter++);
    if(W==1){
        reg = Register::AX;
        uint16_t hb_value = (*byte_emitter++);
        hb_value <<= 8;
        value += hb_value;
    }else{
        reg = Register::AL;
    }
    Instruction instruction = {
        .opcode=opcode,
        .operands={
            Operand{.type=OperandT::Register,.reg=reg},
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