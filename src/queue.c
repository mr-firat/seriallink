#include "queue.h"
#include "app.h"



void rb_get_stats(ring_buffer_t *rb, unsigned *dropped)
{
    pthread_mutex_lock(&rb->mutex);
    *dropped     = rb->dropped;
    pthread_mutex_unlock(&rb->mutex);
}


void rb_init(ring_buffer_t *rb, rb_policy_t policy)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->dropped = 0;
    rb->policy = policy;

    pthread_mutex_init(&rb->mutex, NULL);
    pthread_cond_init(&rb->not_empty, NULL);
}


int rb_push(ring_buffer_t *rb, const frame_t *frame)
{
    pthread_mutex_lock(&rb->mutex);

    if (rb->count == RING_CAPACITY) {

        if (rb->policy == RB_DROP_NEW) {
            rb->dropped++;
            pthread_mutex_unlock(&rb->mutex);
            return -1;
        }

        if (rb->policy == RB_OVERWRITE_OLD) {
            
            rb->tail = (rb->tail + 1) % RING_CAPACITY;
            rb->count--;
            rb->dropped++;   
        }
    }

    rb->buffer[rb->head] = *frame;
    rb->head = (rb->head + 1) % RING_CAPACITY;
    rb->count++;

    pthread_cond_signal(&rb->not_empty);
    pthread_mutex_unlock(&rb->mutex);
    return 0;
}

int rb_pop_blocking(ring_buffer_t *rb, frame_t *out, volatile int *signal)
{
    pthread_mutex_lock(&rb->mutex);
    
    while (rb->count == 0) {
        if (!*signal) {                         //exit on signal event
            pthread_mutex_unlock(&rb->mutex);
            return -1;  
        }
        pthread_cond_wait(&rb->not_empty, &rb->mutex);
    }

    *out = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % RING_CAPACITY;
    rb->count--;

    pthread_mutex_unlock(&rb->mutex);
    return 0;
}