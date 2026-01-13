#include "app.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"
#include "fcntl.h"
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#include "crc.h"
#include "frame.h"
#include "queue.h"
#include "app_config.h"

#define IDLE_TIMEOUT_MS 5000 //Exit after this much time if nothing is received.
#define SLEEP_US 1000   // 1 ms 

//For signaling consumer thread.
volatile int signal = 1;

static uint64_t now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void print_buffer(uint8_t *buf, int len)
{
    printf("\nPrinting buffer:\n");
    for (int i = 0; i < len; i++)
        printf("0x%02x ", buf[i]);
    printf("\n\n");
}


void print_help()
{
    printf("\nCommand/Telemetry Protocol\n");
    printf("\n");
    printf("-d              DEVICE (Device full path. i.e. /dev/pts/1)\n");
    printf("-m              MODE App mode. 1->Write  2->Read.\n");
    printf("-r              Generate n frames with random payload. [0..1000].\n");
    printf("-p              Packet drop policy when rx queue is full. 0->Drop new frame  1->Overwrite the oldest frame.\n");
    printf("-e              Enable Inject Error for testing\n");
    printf("-v              Verbose output for debugging.\n");
    printf("-h              Print this help message.\n");
    printf("\n");
}

Return_t parse_arguments(config_t *conf, int argc, char** argv)
{
    int value, opt;

    if(argc < 2) {
        print_help();
        return EXIT_FAILURE;
    }
    while ((opt = getopt(argc, argv, "d:m:r:p:evh")) != -1) {
        switch (opt) {
            case 'd': {
                conf->dev = optarg;
                conf->fd = open(conf->dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
                if (conf->fd == -1)
                {
                    LOG("Invalid device.\n");
                    return EXIT_FAILURE;
                }
            }break;
            case 'm': {
                value = (app_mode_t)atoi(optarg);
                if((value < MODE_WRITE) || (value > MODE_MAX))
                {
                    LOG("Invalid argument for policy.\n");
                    return EXIT_FAILURE;
                }
                conf->mode = value;    
            }break;
            case 'r': {
                value = atoi(optarg);
                if((value < 0) || (value > RANDOM_PAYLOAD_MAX))
                {
                    LOG("Invalid number of random payload.\n");
                    return EXIT_FAILURE;
                }
                conf->random_payload = value;    
            }break;
            case 'p': {
                rb_policy_t policy = (rb_policy_t)atoi(optarg);
                if((policy < RB_DROP_NEW) || (policy > RB_OVERWRITE_OLD))
                {
                    LOG("Invalid argument for mode.\n");
                    return EXIT_FAILURE;
                } 
                rb_init(conf->rx_queue, policy);
            }break;
            case 'e':{
                conf->inject_error = true;
            }   break;
            case 'v':{
                conf->verbose = true;
            }   break;
            case 'h':{
                print_help();
            }break;
            default:{
                print_help();
                return EXIT_FAILURE;
            }break;
        }
    }
       
    return EXIT_SUCCESS;
}

/*TEST PAYLOAD*/

void generate_random_payload(uint8_t *buf, uint8_t len)
{
    if (!buf || len == 0)
        return;

    for (size_t i = 0; i < len; i++) {
        buf[i] = (uint8_t)(rand() & 0xFF);
    }
}

/*Write Frame*/
Return_t write_frame(config_t *conf, uint8_t *write_buffer)
{
    Return_t ret = EXIT_SUCCESS;
    
    int total_sent = 0;
    int sent_bytes = 0;
    

    while(total_sent < FRAME_LEN){
        sent_bytes = write(conf->fd, write_buffer+total_sent, (FRAME_LEN-total_sent));
        if(sent_bytes > 0){
            total_sent += sent_bytes;
        }else if(sent_bytes < 0){
            ret = EXIT_FAILURE;
            LOG("Write failed.\n");
            break;
        }
        if(conf->verbose){
            LOG("Total bytes written: %d\n", total_sent);
        }
    }

    return ret;
}

/*Generate Frames (randomly or from app_config) and call write_frame */
Return_t write_loop(config_t *conf, uint8_t *write_buffer)
{
    if(conf->random_payload)
    {
        frame_t frame = {0};
        frame.header.sof = SOF;
        frame.header.version = PROTOCOL_VERSION;
        frame.header.seq = 0;
        frame.header.length = MAX_PAYLOAD_SIZE;
        frame.header.cmd = CMD;
        frame.header.subcmd = EXEC;
        

        for(int i = 0; i < conf->random_payload; i++){
            generate_random_payload(frame.payload, MAX_PAYLOAD_SIZE);
        
            if(generate_frame(&frame) == EXIT_FAILURE) 
            {   
                LOG("Frame %d generation failed. \n", i);
                break;
            }
        
            memcpy(write_buffer, &frame, FRAME_LEN);
            
            if(conf->inject_error) {
                if(i%5 == 0) write_buffer[FRAME_HEADER_SIZE +1] = 0xFF; // HARD CODED for now. Corrupt first byte in payload for every 5th frame
            }
            
            if(EXIT_FAILURE == write_frame(conf, write_buffer)) 
            {
                LOG("write_frame failed for Frame %d. \n", i);
                break;
            }
            LOG("Frame %d sent. \n", i);
        }
        
    }
    else
    {
        for(int i = 0; i < number_of_frames; i++){

            if(generate_frame(&test_frames[i]) == EXIT_FAILURE) 
            {   
                LOG("Frame generation failed. \n");
                break;
            }
            
            memcpy(write_buffer, &test_frames[i], FRAME_LEN);
            
            if(conf->inject_error) {
                if(i%5 == 0) write_buffer[FRAME_HEADER_SIZE +1] = 0xFF; //HARD CODED for now. Corrupt first byte in payload for every 5th frame
            }

            if(EXIT_FAILURE == write_frame(conf, write_buffer)) 
            {
                LOG("write_frame failed for Frame %d. \n", i);
                break;
            }
            LOG("Frame %d sent. \n", i);
        }
    }
    return EXIT_SUCCESS;     
}

/*Read Thread*/

void *read_loop(void *args)
{
    
    rthread_args_t *targs = (rthread_args_t *)args;
    config_t *conf = targs->conf;
    uint8_t *read_buffer = targs->read_buffer;

    int buf_size = FRAME_LEN;
    
    int total_read = 0;
    int read_bytes = 0;
    int frame_read_count = 0;
    int frame_drop_count = 0;
    unsigned int dropped_from_queue = 0;

    uint64_t last_rx = now_ms();
    
    conf->state = WAIT_SOF;
    while(1)
    {
        read_bytes = read(conf->fd, (read_buffer + total_read), (buf_size - total_read));
        
        if(conf->verbose) LOG("Reading %d.\n",read_bytes);
        
        
        if(read_bytes > 0){
            
            last_rx = now_ms();

            if(read_bytes < FRAME_LEN){
                frame_drop_count++;
                LOG("Data cropped. Dropping.\n");
                continue;
            }
            
            int i = 0;
    
            while(i <= read_bytes - FRAME_LEN)
            {
                switch (conf->state)
                {
                    case WAIT_SOF: {
                        if(read_buffer[i] == SOF)
                        {
                            conf->state = READ_FRAME;
                        }
                        else
                        {
                            i++;
                            continue;
                        }
                    }    __attribute__((fallthrough)); //Continue with READ_FRAME
                    case READ_FRAME: {
                        
                        frame_t *frame = (frame_t *)(read_buffer + i);
                        
                        if (EXIT_FAILURE == validate_frame(frame)) {
                            LOG("Frame Validation Failed.\n");
                            i += FRAME_LEN;
                            conf->state = WAIT_SOF;
                            frame_drop_count++;
                            
                            break;
                        }
                        else{
                            if (rb_push(conf->rx_queue, frame) != 0) {
                                LOG("Frame being dropped.\n");
                            }
                            
                            pthread_cond_signal(&conf->rx_queue->not_empty);
                            conf->state = WAIT_SOF;
                            i += FRAME_LEN;
                            frame_read_count++;
                            LOG("FRAME %d received.\n",frame_read_count);
                            
                            break;
                        }
                    }
                    /*TODO: Define IDLE state*/
                    case IDLE:
                        conf->state = WAIT_SOF;
                        break;
                    default:
                        break;
                }
            }
            
        }else if(read_bytes == 0){
            LOG("Nothing to receive.\n");
            break;
        }else{
            if(errno == EAGAIN)
            {
                if (now_ms() - last_rx > IDLE_TIMEOUT_MS) {
                    printf("TIMEOUT No more frames will be received, exiting\n");
                    signal = 0;
                    pthread_cond_signal(&conf->rx_queue->not_empty);
                    break;
                }
                usleep(SLEEP_US*5000);
            }
        }
    }
    
    LOG("Total %d frame received.\n",frame_read_count);
    LOG("Total %d frame dropped.\n",frame_drop_count);
    rb_get_stats(conf->rx_queue, &dropped_from_queue);
    if(conf->rx_queue->policy == RB_DROP_NEW){
        LOG("Total %d frame dropped. Queue is full.\n",dropped_from_queue);
    }else{
         LOG("Total %d frame overwritten in the queue.\n",dropped_from_queue);
    }
    LOG("Total %d frame lost.\n",frame_drop_count + dropped_from_queue);
    
    return NULL;
}

/*Consumer Thread*/
void *frame_consumer_thread(void *arg)
{
    ring_buffer_t *rb = (ring_buffer_t *)arg;
    frame_t frame;
    
    while (signal) {
        
        if (rb_pop_blocking(rb, &frame, &signal) != 0){
            
            break;
        }
        
        //Tweak for frame drop simulation
        usleep(SLEEP_US*20);
    
        LOG("Frame read from queue.\n");

        // TODO: handle frame
    }

    return NULL;
}


int main(int argc, char **argv)
{   
    pthread_t consumer_tid;
    pthread_t read_loop_tid;

    

    config_t app_config = {
        .dev = "",
        .fd = -1,
        .mode = MODE_MAX,
        .state = IDLE,
        .rx_queue = NULL,
        .random_payload = 0,
        .inject_error = false,
        .verbose = false
    };
    
    uint8_t *read_buffer = (uint8_t *)calloc(FRAME_LEN, sizeof(uint8_t));
    uint8_t *write_buffer = (uint8_t *)calloc(FRAME_LEN, sizeof(uint8_t));

    app_config.rx_queue = (ring_buffer_t *)calloc(1,sizeof(ring_buffer_t));
     
    if(EXIT_FAILURE == parse_arguments(&app_config, argc, argv))
    {
        LOG("Parsing arguments failed.\n");
        return EXIT_FAILURE;
    }
    else if(app_config.mode == MODE_READ)
    {
        rthread_args_t targs;
        targs.conf = &app_config;
        targs.read_buffer = read_buffer;
        
        if(pthread_create(&consumer_tid, NULL, frame_consumer_thread, app_config.rx_queue)!= EXIT_SUCCESS){
            LOG("Consumer thread creation fail.\n");
            return EXIT_FAILURE;
        }
        if(pthread_create(&read_loop_tid, NULL, read_loop, (void *)&targs)!= EXIT_SUCCESS){
            LOG("Consumer thread creation fail.\n");
            return EXIT_FAILURE;
        }

       
        pthread_join(consumer_tid, NULL);
        pthread_join(read_loop_tid, NULL);
    }
    else if(app_config.mode == MODE_WRITE)
    {
        write_loop(&app_config, write_buffer);
    }

    

    close(app_config.fd); 
    
    free(read_buffer);
    free(write_buffer);

    return EXIT_SUCCESS;
}