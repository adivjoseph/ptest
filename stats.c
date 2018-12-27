#include <stdio.h>
#include <string.h>
#include "stats.h"
#include "fifo.h"

extern msgFifo_t g_msgStats[8];
extern msgFifo_t g_msgCliOut[8];
extern msgFifo_t g_msgCliIn[8]; 

ports_stats_t portStats[8];



void* thStats(void *ptr){
    msgFifo_t *pFifo;
    int i, rtc;
    void * pItem = NULL;

    ports_stats_t portStatsWork;
    cli_msg_t cliMsgWork;
    printf("thStats started\n");


    //bind to a core
    while (1){
        for (i=0; i< 5; i++) {
            rtc = dequeueFifo(&g_msgStats[i], &portStatsWork);
            if (rtc == 1){
/*                printf("p%d %d  %d  %d  %d \n",
                  i,
                portStatsWork.txPkts,
                portStatsWork.rxPkts,
                portStatsWork.txBytes,
                portStatsWork.rxBytes); */
                portStats[i].txPkts  += portStatsWork.txPkts;
                portStats[i].rxPkts  += portStatsWork.rxPkts;
                portStats[i].txBytes += portStatsWork.txBytes;
                portStats[i].rxBytes += portStatsWork.rxBytes;
/*                printf("PP%d %d %d %d %d\n",
                               i,
                                portStats[i].txPkts,
                                portStats[i].rxPkts,
                                portStats[i].txBytes,
                                portStats[i].rxBytes);*/
            }
 
        }

        if((rtc = dequeueFifo(&g_msgCliOut[0], &cliMsgWork)) != 0) {
            printf("msg from cli %d\n",cliMsgWork.cmd);
            switch (cliMsgWork.cmd) {
            case REQ_CLEAR:
                printf("stats clear cmd\n");
                for (i = 0; i < 8; i++) {
                    portStats[i].txPkts = 0;
                    portStats[i].rxPkts = 0;
                    portStats[i].txBytes = 0;
                    portStats[i].rxBytes = 0;
                }
                cliMsgWork.cmd = REQ_CLEAR;
                while(enqueueFifo(&g_msgCliIn[0], &cliMsgWork));
                break;
            case REQ_READ:
                    cliMsgWork.cmd = REQ_READ;
                    printf("get\n");
/*                    for (i = 0; i <4; i++) {
                        printf("P%d %d %d %d %d\n",
                               i,
                                portStats[i].txPkts,
                                portStats[i].rxPkts,
                                portStats[i].txBytes,
                                portStats[i].rxBytes);

                    }*/
                    memcpy(cliMsgWork.data, &portStats[0], sizeof(ports_stats_t) * 5);
                    while(enqueueFifo(&g_msgCliIn[0], &cliMsgWork));


                break;
            default:
                //echo back;
                break;
            }
            
        }
    }

}
