#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "fifo.h"
#include "stats.h"


extern int menuInit();
extern void menuLoop();
extern int menuAddItem(char *name, int (*cbFn)(int, char *argv[]) , char *help);
extern void* thStats(void *ptr);
extern void* thTxS1U(void *ptr);
extern void* thRxSgi(void *ptr);
extern void* thTxSgi(void *ptr);
extern void* thRxS1u(void *ptr);
extern void* thMmeSim(void *ptr);

int cbClear(int argc, char *argv[] );
int cbGetStats(int argc, char *argv[] );
int cbStart(int argc, char *argv[] );
int cbStop(int argc, char *argv[] );
int cbState(int argc, char *argv[] );
int cbS11(int argc, char *argv[] );

//IPC queues
msgFifo_t g_msgStats[8];
/*
    0 s1u tx
    1 sgi tx
    2 siu rx
    3 sgi rx
    4 mmeSim
 
*/

msgFifo_t g_msgCliOut[8];
/*
    0 stats
    1 siu tx
    2 sgi tx
    3 siu rx
    4 sgi rx
    5 mmeSim
*/
msgFifo_t g_msgCliIn[8];

//test
msgFifo_t g_msgNet[2];

int g_mmeSim = 0;
int g_state = 0;
int g_burst = 0;
 
int main(void){	
    int i;
    pthread_t pt_status;
    pthread_t pt_txS1u;
    pthread_t pt_rxSgi;
    pthread_t pt_rxS1u;
    pthread_t pt_txSgi;
    pthread_t pt_mmeSim;

    //init msg queues
    for (i = 0; i < 8; i++) {
       initFifo(&g_msgStats[i]);
       initFifo(&g_msgCliIn[i]);
       initFifo(&g_msgCliOut[i]);
    }
    //start stats thread
    if(pthread_create(&pt_status, NULL, &thStats, NULL)) {
       printf("pthread_create failed thStats \n");
       exit(1);
    }

    if(pthread_create(&pt_txS1u, NULL, &thTxS1U, NULL)) {
       printf("pthread_create failed thTxSiu \n");
       exit(1);
    }

    if(pthread_create(&pt_rxSgi, NULL, &thRxSgi, NULL)) {
       printf("pthread_create failed thRxSgi\n ");
       exit(1);
    }

    if(pthread_create(&pt_rxS1u, NULL, &thRxS1u, NULL)) {
       printf("pthread_create failed thSrxSiu\n");
       exit(1);
    }

    if(pthread_create(&pt_txSgi, NULL, &thTxSgi, NULL)) {
       printf("pthread_create failed thTxSgi\n");
       exit(1);
    }

    if(pthread_create(&pt_mmeSim, NULL, &thMmeSim, NULL)) {
       printf("pthread_create failed thMmeSim\n");
       exit(1);
    }


    menuInit();
    menuAddItem("clear", cbClear, "Clear stats");
    menuAddItem("get", cbGetStats, "Get stats");

    menuAddItem("start", cbStart, "Start [burt count] - EPC traffic");
    menuAddItem("stop", cbStop, "Stop EPC traffic");

    menuAddItem("state", cbState, "State of tester");

    menuAddItem("s11", cbS11, "send s11 setup");
    menuLoop();

	return 0;
}



int cbClear(int argc, char *argv[] ){

    cli_msg_t cliMsgWork;
    int rtc = 0;
    cliMsgWork.cmd =REQ_CLEAR;
    while(enqueueFifo(&g_msgCliOut[0], &cliMsgWork));

    while (rtc == 0) {
        rtc = dequeueFifo(&g_msgCliIn[0], &cliMsgWork); 
        if (cliMsgWork.cmd != REQ_CLEAR) {
            rtc = 0;
        }
    }
    printf("clear done\n");
    return 1;

}

int cbGetStats(int argc, char *argv[] ){

    cli_msg_t cliMsgWork;
    ports_stats_t *portStatsWork;
    int rtc = 0, i;
    cliMsgWork.cmd =REQ_READ;
    while(enqueueFifo(&g_msgCliOut[0], &cliMsgWork));
    while (rtc == 0) {
        rtc = dequeueFifo(&g_msgCliIn[0], &cliMsgWork); 
    }
    if (cliMsgWork.cmd == REQ_READ) {
        portStatsWork = (ports_stats_t *) &cliMsgWork.stats[0];
        printf("Port txpkts rxpkts txBytes rxBytes\n");
        for (i = 0; i < 5; i++) {
            printf("%d %d  %d  %d  %d \n",
                   i,
                portStatsWork->txPkts,
                portStatsWork->rxPkts,
                portStatsWork->txBytes,
                portStatsWork->rxBytes);
            portStatsWork++;
        }
        
    }
    return 1;

}


int cbStart(int argc, char *argv[] ){

    cli_msg_t cliMsgWork;
    int rtc = 0;
    int burst = 0;

    if (g_mmeSim == 0) {
        printf("s11 packets not sent\n");
        return 1;
    }
    cliMsgWork.cmd = REQ_START;
    if (argc) {
        burst = atoi(argv[1]);
        printf("  burts %d\n", burst);
    }

    g_burst = burst;
    cliMsgWork.arg1 = burst;
    while(enqueueFifo(&g_msgCliOut[1], &cliMsgWork));
    while(enqueueFifo(&g_msgCliOut[2], &cliMsgWork));
    g_state = 1;

    return 1;

}

int cbStop(int argc, char *argv[] ){

    cli_msg_t cliMsgWork;
    int rtc = 0;
    if (g_state == 0) {
        printf("start not set\n");
        return 1;
    }
    cliMsgWork.cmd = REQ_STOP;
    while(enqueueFifo(&g_msgCliOut[1], &cliMsgWork));
    while(enqueueFifo(&g_msgCliOut[2], &cliMsgWork));
    g_state = 0;

    return 1;

}

int cbState(int argc, char *argv[] ){

    printf("s11 - ");
    if (g_mmeSim == 0) {
        printf("not setup\n");
    }
    else {
        printf("setup\n");
    }
    printf("traffic - ");
    if (g_state == 0) {
        printf("not started\n");
    }
    else {
        printf("started ");
        if(g_burst){
            printf("burst %d\n", g_burst);
        }
        else {
            printf("continuous\n");
        }
    }

    return 1;

}

int cbS11(int argc, char *argv[] ){

    g_mmeSim = 1;


    return 1;

}
