#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "my_malloc.h"

#define NUM_ITERS    100
#define NUM_ITEMS    10000

#ifdef FF
#define MALLOC(sz) ff_malloc(sz)
#define FREE(p)    ff_free(p)
#endif
#ifdef BF
#define MALLOC(sz) bf_malloc(sz)
#define FREE(p)    bf_free(p)
#endif


double calc_time(struct timeval start, struct timeval end) {
  double start_sec = (double)start.tv_sec + (double)start.tv_usec / 1000000.0;
  double end_sec = (double)end.tv_sec + (double)end.tv_usec / 1000000.0;

  if (end_sec < start_sec) {
    return 0;
  } else {
    return end_sec - start_sec;
  }
};


struct malloc_list {
  size_t bytes;
  int *address;
};
typedef struct malloc_list malloc_list_t;

malloc_list_t malloc_items[2][NUM_ITEMS];

unsigned free_list[NUM_ITEMS];


int main(int argc, char *argv[])
{
  int i, j, k;
  unsigned tmp;
  unsigned long data_segment_size;
  unsigned long data_segment_free_space;
  struct timeval start_time, end_time;

  srand(0);

  const unsigned chunk_size = 32;
  const unsigned min_chunks = 4;
  const unsigned max_chunks = 16;

  // printf("Create malloc_items\n");
  for (i=0; i < NUM_ITEMS; i++) {
    malloc_items[0][i].bytes = ((rand() % (max_chunks - min_chunks + 1)) + min_chunks) * chunk_size;
    malloc_items[1][i].bytes = ((rand() % (max_chunks - min_chunks + 1)) + min_chunks) * chunk_size;
    free_list[i] = i;
  } //for i

  i = NUM_ITEMS;
  // printf("Make free_list random\n");
  while (i > 1) {
    i--;
    j = rand() % i;
    tmp = free_list[i];
    free_list[i] = free_list[j];
    free_list[j] = tmp;
  } //while


  // printf("Alloc space for each item\n");
  for (i=0; i < NUM_ITEMS; i++) {
    malloc_items[0][i].address = (int *)MALLOC(malloc_items[0][i].bytes);
  }


  //Start Time
  gettimeofday(&start_time, NULL);
  // printf("Start\n");
  // printf("Data segment: %lu\n", get_data_segment_size());
  // printf("Free segment: %lu\n", get_data_segment_free_space_size()); 

  for (i=0; i < NUM_ITERS; i++) {
    // printf("%d\n", i);
    unsigned malloc_set = i % 2;
    for (j=0; j < NUM_ITEMS; j+=50) {
      for (k=0; k < 50; k++) {
      	unsigned item_to_free = free_list[j+k];
      	FREE(malloc_items[malloc_set][item_to_free].address);
      }
      for (k=0; k < 50; k++) {
	       malloc_items[1-malloc_set][j+k].address = (int *)MALLOC(malloc_items[1-malloc_set][j+k].bytes);
      } 
    }
    // printf("Data segment: %lu\n", get_data_segment_size());
    // printf("Free segment: %lu\n", get_data_segment_free_space_size()); 
  } 

  //Stop Time
  gettimeofday(&end_time, NULL);

  data_segment_size = get_data_segment_size();
  data_segment_free_space = get_data_segment_free_space_size();
  printf("data_segment_size = %lu, data_segment_free_space = %lu\n", data_segment_size, data_segment_free_space);

  double elapsed_sec = calc_time(start_time, end_time);
  printf("Execution Time = %f seconds\n", elapsed_sec);
  printf("Fragmentation  = %f\n", (float)data_segment_free_space/(float)data_segment_size);

  for (i=0; i < NUM_ITEMS; i++) {
    FREE(malloc_items[0][i].address);
  } //for i

  return 0;
}
