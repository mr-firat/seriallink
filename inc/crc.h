#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>
#include <stddef.h>

// CRC32 
uint32_t compute_crc32(const uint8_t *data, size_t length);

#endif // CRC32_H