#ifndef D751CF13_75F1_42AF_A5A8_26A40FFB05F7
#define D751CF13_75F1_42AF_A5A8_26A40FFB05F7

#include "instructions.h"

Instruction decodeMOV1(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter);
Instruction decodeMOV2(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter);
Instruction decodeMOV3(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter);
Instruction decodeMOV4(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter);
Instruction decodeMOV5(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter);

#endif /* D751CF13_75F1_42AF_A5A8_26A40FFB05F7 */
