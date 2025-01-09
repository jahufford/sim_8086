#ifndef FEFCB3D4_A052_4411_B217_C7A8F8151994
#define FEFCB3D4_A052_4411_B217_C7A8F8151994
#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include "instructions/instructions.h"

std::vector<uint8_t> loadBinary(std::string filename);
std::vector<std::string> instructionsToStrings(std::vector<Instruction> instructions);
std::vector<Instruction> decodeInstructions(std::vector<uint8_t> binary_buffer);

#endif /* FEFCB3D4_A052_4411_B217_C7A8F8151994 */
