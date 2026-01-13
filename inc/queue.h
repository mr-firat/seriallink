#ifndef QUEUE_H
#define QUEUE_H

#include "frame.h"
#include "stdint.h"
#include <pthread.h>

#define RING_CAPACITY  8   // number of frames in buffer

typedef enum {
    RB_DROP_NEW = 0,
    RB_OVERWRITE_OLD = 1
} rb_policy_t;

typedef struct {
    frame_t     buffer[RING_CAPACITY];
    int         head;   // next write index
    int         tail;   // next read index
    int         count;  // number of frames stored
    
    rb_policy_t policy;

    unsigned int dropped;
    
    pthread_mutex_t mutex;
    pthread_cond_t  not_empty;
} ring_buffer_t;


void rb_get_stats(ring_buffer_t *rb, unsigned *dropped);
void rb_init(ring_buffer_t *rb, rb_policy_t policy);
int rb_push(ring_buffer_t *rb, const frame_t *frame);
int rb_pop(ring_buffer_t *rb, frame_t *out);
int rb_pop_blocking(ring_buffer_t *rb, frame_t *out, volatile int *signal);

#endif