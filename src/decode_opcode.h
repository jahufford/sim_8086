#ifndef AAE6A2E5_D164_4F4C_BFA8_2533DC6961CC
#define AAE6A2E5_D164_4F4C_BFA8_2533DC6961CC
#include <cstdint>
#include "decode_instructions.h"

Opcode decodeOpcode(uint8_t byte1, std::vector<uint8_t>::const_iterator byte_emitter);

#endif /* AAE6A2E5_D164_4F4C_BFA8_2533DC6961CC */
