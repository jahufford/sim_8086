#include "decode_opcode.h"
#include "decode_instructions.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cassert>

template <typename T>
struct TD;

static std::unordered_map<std::string,Opcode> opcodes = { 
    {"100010xx", Opcode::MOV1},
    {"1100011x", Opcode::MOV2},
    {"1011xxxx", Opcode::MOV3},
    {"1010000x", Opcode::MOV4},
    {"1010001x", Opcode::MOV5},
    // {"10001110" ,Opcode::MOV6},
    // {"10001100" ,Opcode::MOV7},
    {"100000xx", Opcode::ADDSUBCMP2},
    {"000000xx", Opcode::ADD1},
    {"001010xx", Opcode::SUB1},
    {"001110xx", Opcode::CMP1},
    {"0000010x", Opcode::ADD3},
    {"0010110x", Opcode::SUB3},
    {"0011110x", Opcode::CMP3},
    {"01110101", Opcode::JNZ},
    {"01110100", Opcode::JE},
    {"01111100", Opcode::JL},
    {"01111110", Opcode::JLE},
    {"01110010", Opcode::JB},
    {"01110110", Opcode::JBE},
    {"01111010", Opcode::JP},
    {"01110000", Opcode::JO},
    {"01111000", Opcode::JS},
    {"01111101", Opcode::JNL},
    {"01111111", Opcode::JG},
    {"01110011", Opcode::JNB},
    {"01110111", Opcode::JA},
    {"01111011", Opcode::JNP},
    {"01110001", Opcode::JNO},
    {"01111001", Opcode::JNS},
    {"11100000", Opcode::LOOPNZ},
    {"11100001", Opcode::LOOPZ},
    {"11100010", Opcode::LOOP},
    {"11100011", Opcode::JCXZ},
    // {"10001100" ,Opcode::PUSH1},
    // {"01010xxx" ,Opcode::PUSH2},
    // {"000xx110" ,Opcode::PUSH3},
    // {"10001111" ,Opcode::POP1},
    // {"01011xxx" ,Opcode::POP2},
    // {"000xx111" ,Opcode::POP3},
    // {"1000011x" ,Opcode::XCHG1},
    // {"10010xxx" ,Opcode::XCHG2},
};
struct InstTreeNode;

using InstTree = std::vector<InstTreeNode>;

struct InstTreeNode{
    union{
        char bit;
        Opcode opcode;
    };
    InstTree children;
};

void placeInTree(InstTree& level, std::string::const_iterator bit_iter, std::string::const_iterator bit_end, Opcode& opcode)
{
    if(bit_iter==bit_end){
        level.push_back(InstTreeNode{.opcode=opcode,.children=std::vector<InstTreeNode>{}});
        return;
    }
    char bit = *bit_iter;
    const auto elem = std::find_if(level.begin(), level.end(), 
        [bit](auto item){return item.bit==bit;}
    );
    if(elem==level.end()){
        level.push_back(InstTreeNode{.bit=bit,.children=std::vector<InstTreeNode>{}});
        placeInTree(level.back().children,++bit_iter,bit_end,opcode);
    }else{
        placeInTree(elem->children,++bit_iter,bit_end,opcode);
    }
}
InstTree createDecoderTree()
{
    InstTree tree;
    for(auto& kv: opcodes){
        auto bit = kv.first.cbegin();
        auto bit_end = kv.first.cend();
        placeInTree(tree, bit, bit_end, kv.second);
    }
    return tree;
}

static InstTree decoder_tree = createDecoderTree();

std::string makeString(uint8_t byte)
{
    std::stringstream sstr;
    uint8_t mask = 0b10000000;
    for(auto i=0;i<8;i++){
        auto bit = byte&mask;
        bit >>= 8-i-1;
        sstr << static_cast<char>(bit+48);
        mask >>= 1;
    }
    return sstr.str();
}

Opcode findInTree_impl(InstTree& level, std::string::const_iterator cur_bit, std::string::const_iterator end, std::string opcode_str)
{
    if(cur_bit==end){
        return level.at(0).opcode;
    }
    char bit = *cur_bit;
    auto elem = std::find_if(level.begin(),level.end(),
        [bit](auto item){ 
            return item.bit==bit;
    });
    if(elem==level.end()){
        elem = std::find_if(level.begin(),level.end(),
            [bit](auto item){ 
                return item.bit=='x';
        });
    }
    assert(elem!=level.end());
    return findInTree_impl(elem->children, ++cur_bit,end,opcode_str);
}

Opcode findInTree(std::string opcode_str)
{
    return findInTree_impl(decoder_tree, opcode_str.cbegin(), opcode_str.cend(), opcode_str);
}

Opcode decodeOpcode(uint8_t byte1, std::vector<uint8_t>::const_iterator byte_emitter)
{
    // uint8_t byte = (*byte_emitter++);
    std::string opcode_str = makeString(byte1);
    Opcode opcode = findInTree(opcode_str);

    return opcode;
}