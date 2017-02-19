#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>

#include "potato.h"


int main(int argc, char *argv[]) {
    if (argc != 3) {
		printf("usage: ringmaster number_of_players number_of_hops\n");
		exit(0);
	}
	int num_player = atoi(argv[1]);
	int num_hop = atoi(argv[2]);
	int *fdRD = malloc(num_player * sizeof(int));
	int *fdWR = malloc(num_player * sizeof(int));
	fd_set RD_set, WR_set, Cur_set;
	printf("Potato Ringmaster \nPlayers = %d \nHops = %d\n", num_player, num_hop);
    
    create_fifo(num_player);
    
    sleep(1);
    
    boardcast (num_player, fdWR);
    
    sleep(1);
    
	wait_player (num_player, fdRD, &RD_set);
	
	sleep(1);
	
	POTATO_T potato;
	potato.msg_type = START;
	potato.total_hops = num_hop;
	potato.hop_count = 0;
	
	// time_t t;
	// srand((unsigned) time(&t));
	int random = rand() % num_player;
	write(fdWR[random], &potato, sizeof(potato));
	printf("All players present, sending potato to player %d\n", random);
	
	char buf[BUFF_LEN];
	Cur_set = RD_set;
	int readOnce = 0;
	while (select(fdRD[num_player-1]+1, &Cur_set, NULL, NULL, NULL)>0) {
		int i = 0;
		for (; i<num_player; i++) {
			if (FD_ISSET(fdRD[i], &Cur_set)) {
				POTATO_T potato;
				if (read(fdRD[i], &potato, sizeof(potato))>0) {
					printTrace(potato);
					readOnce = 1;
					break;
				}
			}
		}
		if (readOnce == 1) break;
    }
	
	int i = 0;
	for (; i < num_player; i++) {
		potato.msg_type = END;
		write(fdWR[i], &potato, sizeof(POTATO_T));
	}

	sleep(1);

	for (i = 0; i < num_player; i++) {
		char fifoName[BUFF_LEN];
		clear_string(fifoName);
		sprintf(fifoName, "%sp%d_master", LOC, i);
		unlink(fifoName);
		
		clear_string(fifoName);
		sprintf(fifoName, "%smaster_p%d", LOC, i);
		unlink(fifoName);
		
		clear_string(fifoName);
		sprintf(fifoName, "%sp%d_p%d", LOC, i, (i+1)%num_player);
		unlink(fifoName);
		
		clear_string(fifoName);
		sprintf(fifoName, "%sp%d_p%d", LOC, i, (i-1+num_player)%num_player);
		unlink(fifoName);
	}
	// printf("master exits\n");
 	exit(0);
}
