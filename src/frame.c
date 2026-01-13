#include "frame.h"
#include "crc.h"
#include "app.h"

#include "string.h"
#include <stdlib.h>


//Check fields and generate CRCs
Return_t generate_frame(frame_t *FRAME){


    //Generate the crc for the payload
    uint32_t crc = compute_crc32(FRAME->payload, MAX_PAYLOAD_SIZE);

    if(!crc)
    {
        LOG("CRC payload computation failed. \n");
        return EXIT_FAILURE;
       
    }
    
    FRAME->header.payload_crc = crc;

    //Generate the crc for the header
    FRAME->header.header_crc = 0;
    crc = compute_crc32((uint8_t*)FRAME, FRAME_HEADER_SIZE);

    if(!crc)
    {
        LOG("CRC header computation failed. \n");
        return EXIT_FAILURE;
    }
    
    FRAME->header.header_crc = crc;
    
    if(validate_frame(FRAME)) 
    {   
        LOG("Invalid Frame generated.\n");
        return EXIT_FAILURE;
        
    }
 /*    
    print_buffer((uint8_t*)FRAME,FRAME_LEN);

    LOG("cmd %u\n",FRAME->header.cmd);
    LOG("subcmd %u\n",FRAME->header.subcmd);
    LOG("header crc 0x%08x\n",FRAME->header.header_crc);
    LOG("payload crc 0x%08x\n",FRAME->header.payload_crc);
    for(int i=0;i<(sizeof(frame_t));i++)
    {
        printf("0x%01x ",((uint8_t *)FRAME)[i]);
    }
    LOG("\n\n");
*/ 
    return EXIT_SUCCESS;
}

Return_t validate_frame(frame_t *FRAME){
  
    if(FRAME->header.version != PROTOCOL_VERSION){
        LOG("Protocol Version Error.\n");
        return EXIT_FAILURE;
    }
    //TODO: check the frame length of each frame
    if(FRAME->header.length > FRAME_LEN){
        LOG("Frame length mismatch.\n");
        return EXIT_FAILURE;
    }

    uint32_t crc = FRAME->header.header_crc;
    FRAME->header.header_crc = 0;
    FRAME->header.header_crc = compute_crc32((uint8_t*)FRAME, FRAME_HEADER_SIZE);

    if(crc != FRAME->header.header_crc){
        LOG("Frame Header CRC Fail.\n");
        LOG("Received header crc 0x%08x Calculated crc 0x%08x\n",crc, FRAME->header.header_crc);
        return EXIT_FAILURE;
    }

    crc = FRAME->header.payload_crc;
    FRAME->header.payload_crc = 0;
    FRAME->header.payload_crc = compute_crc32(FRAME->payload, MAX_PAYLOAD_SIZE);
    
    if(crc != FRAME->header.payload_crc){
        LOG("Frame Payload CRC Fail.\n");
        LOG("Received payload crc 0x%08x Calculated crc 0x%08x\n",crc, FRAME->header.payload_crc);
        return EXIT_FAILURE;
    }

    if((FRAME->header.cmd < 0) || (FRAME->header.cmd >= CMD_MAX)){
        LOG("Frame Invalid CMD.\n");
        return EXIT_FAILURE;
    }

    if((FRAME->header.subcmd < 0) || (FRAME->header.subcmd >= SUBCMD_MAX)){
        LOG("Frame Invalid SUBCMD.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}