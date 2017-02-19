#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>

#define BLOCK_SIZE 40

typedef struct s_block *t_block;
struct s_block {
  size_t size;
  t_block prev;
  t_block next;
  int free; 
  int padding;
  void *ptr;  
  char data[1];
};

void * first_block = NULL;

t_block ff_find(t_block * last, size_t size){
  t_block b = first_block;
  while(b && !(b->free && b->size >= size)) {
    *last = b;
    b = b->next;
  }
  return b;
}

t_block bf_find(t_block *last, size_t size){
  t_block b = first_block;
  t_block target = first_block;
  size_t smallest = INT_MAX;
  while(b) {
    if (b->free) {
      if (b->size == size) {
        return b;  
      } else if (b->size > size && b->size < smallest) {
        target = b;
        smallest = b->size;  
      }
    }
    *last = b;
    b = b->next;
  }
  if (target == first_block) return NULL;  
  else return target;
} 

t_block extend_heap(t_block last, size_t s) {
  t_block b;
  b = sbrk(0);
  if(sbrk(BLOCK_SIZE + s) == (void *)-1)
    return NULL;
  b->size = s;
  b->next = NULL;
  b->prev = last;
  b->free = 0;
  b->ptr = b->data;
  if(last){
    last->next = b;
  }
  return b;
}

void split_block(t_block b, size_t s) {
  t_block new;
  new = (t_block)(b->data + s);
  new->size = b->size - s - BLOCK_SIZE ;
  new->next = b->next;
  new->prev = b;
  new->free = 1;
  new->ptr = new->data;
  b->size = s;
  b->next = new;
  if(new->next)
    new -> next -> prev = new;
}

size_t align8(size_t s) {
  if((s & 0x7) == 0)
    return s;
  return ((s >> 3) + 1) << 3;
}

t_block get_block(void *p) {
  p = p - BLOCK_SIZE;
  t_block ans = p;
  return p;
}

int valid_addr(void *p) {
  if(first_block) {
    if(p > first_block && p < sbrk(0)) {
      return (p == (get_block(p))->ptr);
    }
  }
  return 0;
}

t_block fusion(t_block b) {
  if (b->next && b->next->free) {
    b->size += BLOCK_SIZE + b->next->size;
    b->next = b->next->next;
    if(b->next)
      b->next->prev = b;
  }
  return b;
}


void * ff_malloc(size_t size);
void ff_free(void * ptr);
void * bf_malloc(size_t size);
void bf_free(void * ptr);
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
void * ts_malloc(size_t size);
void ts_free(void * ptr);



