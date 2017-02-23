#define BUFF_LEN   512
#define MAX_POTATO 512
#define LOC "./tmp/"

#define START 1
#define END 0

#define READY "ready"



typedef struct potato {
  char msg_type;
  
  int total_hops;
  int hop_count;
  
  unsigned long hop_trace[MAX_POTATO];
} POTATO_T;


void clear_string( char *s ) {
  int i;
  for ( i=0; i < BUFF_LEN; i++ ) {
    s[i] = '\0';
  }
}

void create_fifo (int num_player) {
  char fifoName[BUFF_LEN] = "";
  int i;
  for (i=0; i < num_player; i++) {
    clear_string(fifoName);
    sprintf(fifoName, "%smaster_p%d", LOC, i);
    unlink(fifoName);
    if (mkfifo(fifoName, 0666) != 0) printf("make fifo %s failed\n", fifoName);
    
    clear_string(fifoName);
    sprintf(fifoName, "%sp%d_master", LOC, i);
    unlink(fifoName);
    if (mkfifo(fifoName, 0666) != 0) printf("make fifo %s failed\n", fifoName);
    
    clear_string(fifoName);
    sprintf(fifoName, "%sp%d_p%d", LOC, i, (i-1+num_player) % num_player);
    unlink(fifoName);
    if (0 != mkfifo(fifoName, 0666))	printf("make fifo %s failed\n", fifoName);
    
    clear_string(fifoName);
    sprintf(fifoName, "%sp%d_p%d", LOC, i, (i+1) % num_player);
    unlink(fifoName);
    if (0 != mkfifo(fifoName, 0666)) printf("make fifo %s failed\n", fifoName);
  }
}

void boardcast (int num_player, int* fdWR) {
  char fifoName[BUFF_LEN] = "";
  int i = 0;
  for (; i<num_player; i++) {
    clear_string(fifoName);
    sprintf(fifoName, "%smaster_p%d", LOC, i);
    fdWR[i] = open (fifoName, O_WRONLY);
    write(fdWR[i], &num_player, sizeof(int));
  }
}

void wait_player (int num_player, int * fdRD, fd_set * RD_set) {
  char fifoName[BUFF_LEN] = "";
  char buf[BUFF_LEN];
  FD_ZERO(RD_set);
  int i = 0;
  for (; i<num_player; i++) {
    clear_string(fifoName);
    sprintf(fifoName, "%sp%d_master", LOC, i);
    fdRD[i] = open(fifoName, O_RDONLY);
    FD_SET(fdRD[i], RD_set);
    clear_string(buf);
    read(fdRD[i], buf, BUFF_LEN);
    printf("Player %d is ready to play\n", i);
  }
}

void printTrace(POTATO_T potato) {
  
  printf("Trace of potato:\n");
  int i = 0;
  for (; i < potato.total_hops; i++) {
    printf("%lu,", potato.hop_trace[i]);
  }
  printf("\n");
}
