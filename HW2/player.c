#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>
#include <time.h>

#include "potato.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: player index_of_player\n");
    exit(0);
  }
  int player_index = atoi(argv[1]);
  srand((unsigned int)time(NULL)+player_index);
  int num_player;
  char fifoName[BUFF_LEN] = "";
  int fd_rd[3];
  int fd_wr[3];
  fd_set rd_set, cur_set;
  FD_ZERO(&rd_set);
  
  clear_string(fifoName);
  sprintf(fifoName, "%smaster_p%d", LOC, player_index);
  fd_rd[0] = open(fifoName, O_RDONLY);
  FD_SET(fd_rd[0], &rd_set);
  read(fd_rd[0], &num_player, sizeof(int));
  printf("Connected as player %d out of %d total players\n", player_index, num_player);
  
  clear_string(fifoName);
  sprintf(fifoName, "%sp%d_master", LOC, player_index);
  fd_wr[0] = open(fifoName, O_WRONLY);
  if (fd_wr[0]==-1) {
    perror("open error");
  }
  if ( write(fd_wr[0], READY, strlen(READY))==-1 ) {
    perror("write error");
  }
  
  clear_string(fifoName);
  sprintf(fifoName, "%sp%d_p%d",LOC, player_index, (player_index-1+num_player) % num_player);
  fd_wr[1] = open(fifoName, O_RDWR);
  if (fd_wr[1]==-1) perror("open error");
  
  
  clear_string(fifoName);
  sprintf(fifoName, "%sp%d_p%d",LOC, player_index, (player_index+1) % num_player);
  fd_wr[2] = open(fifoName, O_RDWR);
  if (fd_wr[2]==-1) perror("open error");
  
  clear_string(fifoName);
  sprintf(fifoName, "%sp%d_p%d",LOC, (player_index-1+num_player) % num_player, player_index);
  fd_rd[1] = open(fifoName, O_RDONLY | O_NONBLOCK);
  if (fd_rd[1]==-1) perror("open error");
  FD_SET(fd_rd[1], &rd_set);
  
  clear_string(fifoName);
  sprintf(fifoName, "%sp%d_p%d", LOC, (player_index+1) % num_player, player_index);
  fd_rd[2] = open(fifoName, O_RDONLY | O_NONBLOCK);
  if (fd_rd[2]==-1) perror("open error");
  FD_SET(fd_rd[2], &rd_set);
    
    
  cur_set = rd_set;
  int game_over = 0;
  while (select(fd_rd[2]+1, &cur_set, NULL, NULL, NULL)>0) {
    int i = 0;
    for (; i<num_player; i++) {
      if (FD_ISSET(fd_rd[i], &cur_set)) {
	POTATO_T potato;
	if (read(fd_rd[i], &potato, sizeof(potato))>0) {
	  if (potato.msg_type == START) {
	    potato.hop_trace[potato.hop_count] = player_index;
	    if (potato.hop_count >= potato.total_hops-1) {
	      if ( write(fd_wr[0], &potato, sizeof(potato)) < 0 ) perror("write error");
	      else printf("I'm it\n");
	    } else {
	      int random = rand() % 2 + 1;
	      if (random == 1) {
		printf("Sending potato to %d\n", (player_index-1+num_player) % num_player);
	      } else {
		printf("Sending potato to %d\n", (player_index+1) % num_player);
	      }
	      potato.hop_count++;
	      if ( write(fd_wr[random], &potato, sizeof(potato)) < 0 ) perror("Write to player err");
	    }
	  } else {
	    game_over = 1;
	    break;
	  }
	}
      }
      cur_set = rd_set;
    }
    if (game_over == 1) break;
  }
  exit(0);  
}
