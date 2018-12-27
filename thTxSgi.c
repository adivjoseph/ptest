#include <stdio.h>
#include <string.h>
#include "stats.h"
#include "fifo.h"

extern msgFifo_t g_msgStats[8];
extern msgFifo_t g_msgCliOut[8];
extern msgFifo_t g_msgCliIn[8]; 


//test
msgFifo_t g_msgNet[2];






void* thTxSgi(void *ptr){
    msgFifo_t *sp_msgCliOut =  &g_msgCliOut[2];    //cli comands
    msgFifo_t *sp_msgCliIn  =  &g_msgCliIn[2];    //cli responses
    msgFifo_t *sp_msgStats  =  &g_msgStats[1];    //stats out
    ports_stats_t portStats;
    cli_msg_t cliMsgWork;
    char packet[256];

    int state = 0;  //o idle, 1 running
    int statsCounter = 0;
    int loop = 0;
    int rtc = 0;
    int burst = -1;
    int delay = 0;

    memset(&portStats, 0, sizeof(ports_stats_t));

    while (1) {
        if (state && ((burst) || (burst < 0))){
            if (delay == 1000) {
           
            //send some packets
            while(enqueueFifo(&g_msgNet[1], packet));
            portStats.txPkts++;
            portStats.txBytes +=256;
            statsCounter++;
            burst--;
            delay = 0;
            }
            else
                delay++;
        }
        if (loop++ == 10) {
            if((rtc = dequeueFifo(sp_msgCliOut, &cliMsgWork)) != 0) {
                switch (cliMsgWork.cmd) {
                case REQ_START:
                    burst = cliMsgWork.arg1;
                    if (burst == 0) {
                        burst = -1;
                    }
                    printf("siutx start burst %d\n", burst);
                    state = 1;
                    break;
                case REQ_STOP:
                    printf("siutx stop\n");
                    state = 0;
                    statsCounter = 100;
                    break;
                default:
                    break;
                }
            }
            loop = 0;
        }
        

        if (statsCounter == 100) {
            //update stats
            while(enqueueFifo(sp_msgStats, &portStats));
/*             printf("tx %d  %d  %d  %d \n",
                portStats.txPkts,
                portStats.rxPkts,
                portStats.txBytes,
                portStats.rxBytes);*/
            memset(&portStats, 0, sizeof(ports_stats_t));
            statsCounter = 0;
        }
        




    }
    

}
