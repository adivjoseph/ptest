#include <stdio.h>
#include <string.h>
#include "stats.h"
#include "fifo.h"

extern msgFifo_t g_msgStats[8];
extern msgFifo_t g_msgCliOut[8];
extern msgFifo_t g_msgCliIn[8]; 




void* thMmeSim(void *ptr){
    msgFifo_t *pFifo;
    int i, rtc;
    void * pItem = NULL;

    ports_stats_t portStatsWork;
    cli_msg_t cliMsgWork;
    printf("thMmeSim started\n");


    //bind to a core
    while (1){
  

        if((rtc = dequeueFifo(&g_msgCliOut[5], &cliMsgWork)) != 0) {
            printf("msg from cli %d\n",cliMsgWork.cmd);
            switch (cliMsgWork.cmd) {
            case REQ_START:
                printf("mmeSim start cmd\n");

                break;

            default:
                break;
            }
            
        }
    }

}
