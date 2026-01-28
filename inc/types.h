#ifndef TYPES_H
#define TYPES_H


typedef enum{
    FALSE = 0,
    TRUE  = 1
}bool_t;


typedef enum{
    EXIT_SUCCESS = 0,
    EXIT_FAILURE = 1
}Return_t;

typedef enum {
    MODE_WRITE = 1,
    MODE_READ = 2,
    MODE_MAX
}app_mode_t;

typedef enum{
    IDLE = 0,
    WAIT_SOF = 1,
    READ_FRAME = 2,
    MAX
}RX_State_t;


#endif