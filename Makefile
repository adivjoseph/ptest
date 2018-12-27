


ptest : main.c cli.c stats.c fifo.c thTxS1U.c thRxSgi.c thRxSiu.c thTxSgi.c mmeSim.c
	gcc -g -o ptest main.c cli.c stats.c fifo.c thTxS1U.c thRxSgi.c  thRxSiu.c thTxSgi.c mmeSim.c -pthread
