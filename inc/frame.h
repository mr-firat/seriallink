#ifndef FRAME_H
#define FRAME_H

#include "stdint.h"
#include "types.h"
#define SOF                 0x7EUL
#define PROTOCOL_VERSION    0x01
#define FRAME_HEADER_SIZE   20
#define MAX_PAYLOAD_SIZE    8
#define FRAME_LEN  (FRAME_HEADER_SIZE + MAX_PAYLOAD_SIZE)   

typedef enum{
    CMD,
    TELEMETRY,
    CMD_MAX
}cmd_t;

typedef enum{
    READ,
    WRITE,
    EXEC,
    SUBCMD_MAX
}subcmd_t;

typedef struct
{
    uint8_t  sof;
    uint8_t  version;
    uint8_t  seq;
    uint8_t  length;
    cmd_t    cmd;
    subcmd_t subcmd;                             
    uint32_t header_crc;                        
    uint32_t payload_crc;
}header_t;

typedef struct {
    header_t header;
    uint8_t  payload[MAX_PAYLOAD_SIZE];
}frame_t;


Return_t generate_frame(frame_t *FRAME);
Return_t validate_frame(frame_t *FRAME);

#endif