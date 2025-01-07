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
    {"1000101x", Opcode::MOV2},
    {"1011xxxx" ,Opcode::MOV3},
    {"1010000x" ,Opcode::MOV4},
    // {"1010001x" ,Opcode::MOV5},
    // {"10001110" ,Opcode::MOV6},
    // {"10001110" ,Opcode::MOV7},
    // {"10001100" ,Opcode::MOV8},
    // {"10001100" ,Opcode::PUSH1},
    // {"01010xxx" ,Opcode::PUSH2},
    // {"000xx110" ,Opcode::PUSH3},
};
struct InstTreeNode;

// using InstTree = std::vector<InstTreeNode>(3); // specify size
using InstTree = std::vector<InstTreeNode>;

struct InstTreeNode{
    union{
        char bit;
        Opcode opcode;
    };
    InstTree children;
};

static bool has(InstTree& vect, char bit){
    for(auto& item: vect){
        if(item.bit == bit){
            return true;
        }
    }
    return false;
}
void placeInTree(InstTree& level, std::string::const_iterator bit_iter, std::string::const_iterator bit_end, Opcode& opcode)
{
    if(bit_iter==bit_end){
        level.push_back(InstTreeNode{.opcode=opcode});
        return;
    }
    char bit = *bit_iter;
    if(!has(level,bit)){
        level.push_back(InstTreeNode{.bit=bit});
    }
    // auto& new_level = level.back();
    ++bit_iter;
    placeInTree(level.back().children,bit_iter,bit_end,opcode);
}
InstTree createDecoderTree()
{
    InstTree tree;
    InstTree* level = &tree;
    for(auto& item: *level){
        std::cout << item.bit << std::endl;
    }
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

Opcode findInTree_impl(InstTree& level, std::string::const_iterator cur_bit, std::string::const_iterator end)
{
    if(cur_bit==end){
        return level.at(0).opcode;
    }
    char bit = *cur_bit;
    auto elem = std::find_if(level.begin(),level.end(),
        [bit](auto item){ 
            return (item.bit==bit) || (item.bit=='x');
    });
    assert(elem!=level.end());
    return findInTree_impl(elem->children, ++cur_bit,end);
}

Opcode findInTree(std::string opcode_str)
{
    return findInTree_impl(decoder_tree, opcode_str.cbegin(), opcode_str.cend());
}

Opcode decode_opcode(uint8_t byte)
{
    std::string opcode_str = makeString(byte);
    return findInTree(opcode_str);
}