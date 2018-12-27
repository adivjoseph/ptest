#ifndef __STATS_H
#define __STATS_H

typedef struct {
    int txPkts;
    int rxPkts;
    int txBytes;
    int rxBytes;

} ports_stats_t;


typedef struct {
    int cmd;
    int arg1;
    union {
        char  data[250];
        ports_stats_t stats[8];
    };
} cli_msg_t;


#define RESP_ACK    0
#define RESP_NACK   1
#define REQ_CLEAR   2
#define REQ_READ    3
#define REQ_START   4
#define REQ_STOP    5


#endif



