#include <stdio.h>
#include <string.h>
#include "stats.h"
#include "fifo.h"

extern msgFifo_t g_msgStats[8];
extern msgFifo_t g_msgCliOut[8];
extern msgFifo_t g_msgCliIn[8]; 


//test
extern msgFifo_t g_msgNet[2];






void* thRxS1u(void *ptr){
    msgFifo_t *sp_msgCliOut =  &g_msgCliOut[3];    //cli comands
    msgFifo_t *sp_msgCliIn  =  &g_msgCliIn[3];    //cli responses
    msgFifo_t *sp_msgStats  =  &g_msgStats[2];    //stats out
    ports_stats_t portStats;
    cli_msg_t cliMsgWork;
    char packet[256];

    int state = 0;  //o idle, 1 running
    int statsCounter = 0;
    int loop = 0;
    int rtc;

    while (1) {
        if((rtc = dequeueFifo(&g_msgNet[1], packet)) != 0) {
            portStats.rxPkts++;
            portStats.rxBytes +=256;
            statsCounter++;
        }
        else{
            if (statsCounter) {
                statsCounter = 100;
            }
        }
        

        if (statsCounter == 100 ) {
            //update stats
            while(enqueueFifo(sp_msgStats, &portStats));
/*            printf("rx %d  %d  %d  %d \n",
                portStats.txPkts,
                portStats.rxPkts,
                portStats.txBytes,
                portStats.rxBytes);*/
            memset(&portStats, 0, sizeof(ports_stats_t));
            statsCounter = 0;
        }
    }
    

}
