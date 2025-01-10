#ifndef FCE89745_9E97_46F7_90CC_B18685CDDF60
#define FCE89745_9E97_46F7_90CC_B18685CDDF60
#include "instructions.h"

Instruction decodeADDSUBCMP1(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter);
Instruction decodeADDSUBCMP2(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter);
Instruction decodeADDSUBCMP3(uint8_t byte1, std::vector<uint8_t>::const_iterator& byte_emitter);

#endif /* FCE89745_9E97_46F7_90CC_B18685CDDF60 */
