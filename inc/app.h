#ifndef CMD_TELEM_H
#define CMD_TELEM_H

#include "types.h"
#include "queue.h"
#include "stdint.h"
#include "stdio.h"

#define RANDOM_PAYLOAD_MAX 1000

typedef struct{
    char *dev;
    int fd;
    app_mode_t mode;
    RX_State_t state;
    ring_buffer_t *rx_queue;
    int random_payload;
    bool inject_error;
    bool verbose;
}config_t;

typedef struct {
    config_t *conf;
    uint8_t *read_buffer;
} rthread_args_t;

void print_buffer(uint8_t *buf, int len);

//TODO switch to memscpy
#define memscpy(dst, dst_size, src, src_size){                                  \
            size_t copy_size = (dst_size < src_size) ? dst_size : src_size      \
            memcpy(dst, src, copy_size);                                        \
        }                                       


//TODO: Create custom printing func. i.e. with timestamps
#define LOG(fmt, ...)                           \
                do {                            \
                    printf(fmt, ##__VA_ARGS__); \
                }while (0)                      \
           
            

#endif