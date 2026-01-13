#include "app_config.h"


//Define Test Frames Below
frame_t test_frames[] = {
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,  
        .header.seq = 0,
        .header.length = 8,
        .header.cmd = CMD, 
        .header.subcmd = READ,
        .payload = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 1,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = READ,
        .payload = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 0,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = WRITE,
        .payload = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 0,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = EXEC,
        .payload = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,  
        .header.seq = 0,
        .header.length = 8,
        .header.cmd = CMD, 
        .header.subcmd = READ,
        .payload = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 1,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = READ,
        .payload = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 0,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = WRITE,
        .payload = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 0,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = EXEC,
        .payload = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,  
        .header.seq = 0,
        .header.length = 8,
        .header.cmd = CMD, 
        .header.subcmd = READ,
        .payload = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 1,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = READ,
        .payload = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 0,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = WRITE,
        .payload = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 0,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = EXEC,
        .payload = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,  
        .header.seq = 0,
        .header.length = 8,
        .header.cmd = CMD, 
        .header.subcmd = READ,
        .payload = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 1,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = READ,
        .payload = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 0,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = WRITE,
        .payload = {0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB}
    },
    {   
        .header.sof = SOF,
        .header.version = PROTOCOL_VERSION,
        .header.seq = 0,
        .header.length = 8,  
        .header.cmd = CMD, 
        .header.subcmd = EXEC,
        .payload = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC}
    }
};

int number_of_frames = sizeof(test_frames) / sizeof(test_frames[0]);