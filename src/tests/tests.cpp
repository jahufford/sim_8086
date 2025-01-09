#include <iostream>
#include <string>
#include <cassert>
#include <utility>
#include <gtest/gtest.h>
#include "../decode_instructions.h"
#include "../decode_opcode.h"

TEST(basicTasks, testLoadBinary)
{
    std::string filename = "../src/data/0037_single_register_mov";
    auto byte_stream = loadBinary(filename);
    ASSERT_EQ(byte_stream.size(),2);
    ASSERT_EQ(byte_stream.at(0),0x89);
}

TEST(basicTasks, testDecodeInstructions_0037)
{
    std::string filename = "../src/data/0037_single_register_mov";
    auto instructions = decodeInstructions(loadBinary(filename));
    auto instruction_str = instructionsToStrings(instructions);
    ASSERT_EQ(instruction_str.at(0), "mov cx, bx");
}

template <typename T, typename U>
std::vector<std::pair<T,U>> zip(std::vector<T>& vect1, std::vector<U>& vect2){
    std::vector<std::pair<T,U>> vect;
    assert(vect1.size()==vect2.size());
    for(size_t i=0;i<vect1.size();++i){
        vect.push_back(std::make_pair(vect1.at(i),vect2.at(i)));
    }
    return vect;
}
// creates a zipped vector of pointers so no copying is done
template <typename T, typename U>
std::vector<std::pair<const T*,const U*>> zip_p(const std::vector<T>& vect1,const std::vector<U>& vect2){
    std::vector<std::pair<const T*,const U*>> vect;
    assert(vect1.size()==vect2.size());
    for(size_t i=0;i<vect1.size();++i){
        vect.push_back(
            std::make_pair(&vect1.at(i),&vect2.at(i))
        );
    }
    return vect;
}
TEST(basicTasks, testDecodeInstructions_0038)
{
    std::string filename = "../src/data/0038_many_register_mov";
    auto instructions = decodeInstructions(loadBinary(filename));
    auto instruction_strs = instructionsToStrings(instructions);
    std::vector<std::string> correct_out = {
        "mov cx, bx",
        "mov ch, ah",
        "mov dx, bx",
        "mov si, bx",
        "mov bx, di",
        "mov al, cl",
        "mov ch, ch",
        "mov bx, ax",
        "mov bx, si",
        "mov sp, di",
        "mov bp, ax",
    };
    for(auto& line : zip_p(instruction_strs,correct_out)){
        ASSERT_EQ(*line.first, *line.second);
    }
}
TEST(basicTasks, testDecodeInstructions_0039)
{
    std::string filename = "../src/data/0039_more_movs";
    auto instructions = decodeInstructions(loadBinary(filename));
    auto instruction_strs = instructionsToStrings(instructions);
    std::vector<std::string> correct_out = {
        "mov si, bx",
        "mov dh, al",
        "mov cl, byte 12",
        "mov ch, byte -12",
        "mov cx, word 12",
        "mov cx, word -12",
        "mov dx, word 3948",
        "mov dx, word -3948",
        "mov al, [bx + si]",
        "mov bx, [bp + di]",
        "mov dx, [bp]",
        "mov ah, [bx + si + 4]",
        "mov al, [bx + si + 4999]",
        "mov [bx + di], cx",
        "mov [bp + si], cl",
        "mov [bp], ch"
    };
    for(auto& line : zip_p(instruction_strs,correct_out)){
        ASSERT_EQ(*line.first, *line.second);
    }
}
TEST(basicTasks, testDecodeInstructions_0040)
{
    std::string filename = "../src/data/0040_challenge_movs";
    auto instructions = decodeInstructions(loadBinary(filename));
    auto instruction_strs = instructionsToStrings(instructions);
    std::vector<std::string> correct_out = {
        "mov ax, [bx + di - 37]",
        "mov [si - 300], cx",
        "mov dx, [bx - 32]",
        "mov [bp + di], byte 7",
        "mov [di + 901], word 347",
        "mov bp, [5]",
        "mov bx, [3458]",
        "mov ax, [2555]",
        "mov ax, [16]",
        "mov [2554], ax",
        "mov [15], ax"
    };
    for(auto& line : zip_p(instruction_strs,correct_out)){
        ASSERT_EQ(*line.first, *line.second);
    }
}
TEST(basicTasks, testDecodeOpcode)
{
    // {"100010xx", Opcode::MOV1},
    // {"1000101x", Opcode::MOV2},
    // {"1011xxxx" ,Opcode::MOV3},
    Opcode opcode = decodeOpcode(0b10001001);
    ASSERT_EQ(opcode, Opcode::MOV1);
    opcode = decodeOpcode(0b10001011);
    ASSERT_EQ(opcode, Opcode::MOV1);
    opcode = decodeOpcode(0b10111001);
    ASSERT_EQ(opcode, Opcode::MOV3);
}
TEST(basicTasks, testInstructionToString)
{
    EffectiveAddress ea = createEffectiveAddress(0x02,0x03,12345);
    Instruction instruction {
        .opcode=Opcode::MOV1,
        .operands={Operand{.type=OperandT::Register, .reg=Register::BX},
                Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0}
                }
    };
    auto inst_str = instructionToString(instruction);
    ASSERT_EQ(inst_str, "mov bx, [bp + di + 12345]");
    ea = createEffectiveAddress(0x00,0x06,12345);
    instruction = {
        .opcode=Opcode::MOV1,
        .operands={Operand{.type=OperandT::Register, .reg=Register::BP},
                Operand{.type=OperandT::EffectiveAddress, .effective_address=ea},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0},
                Operand{.type=OperandT::NoOperand,.noValue=0}
                }
    };
    inst_str = instructionToString(instruction);
    ASSERT_EQ(inst_str, "mov bp, [12345]");
}
int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret{RUN_ALL_TESTS()};
    if(!ret){
        std::cout << "<<<SUCCESS>>>" << std::endl;
    }else{
        std::cout << "FAILED" << std::endl;
    }
}