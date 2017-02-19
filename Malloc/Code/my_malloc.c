#include "my_malloc.h"



void * ff_malloc(size_t size) {
  t_block b, last;
  size_t s;
  s = align8(size);
  if(first_block) {
    last = first_block;
    b = ff_find(&last, s);
    if(b) {
      if ((b->size - s) >= ( BLOCK_SIZE + 8)) 
        split_block(b, s);
      b->free = 0;
      b->ptr = b->data;
    } 
    else {
      b = extend_heap(last, s);
      b->ptr = b->data;
      if(!b)  
        return NULL;
    }
  } 
  else {
    b = extend_heap(NULL, s);
    b->ptr = b->data;
    if(!b)
      return NULL;
    first_block = b;
  }
  return b->data;
}

void * bf_malloc(size_t size) {
  t_block b, last;
  size_t s;
  s = align8(size);
  if(first_block) {
    last = first_block;
    b = bf_find(&last, s);
    if(b) {
      if ((b->size - s) >= ( BLOCK_SIZE + 8)) 
        split_block(b, s);
      b->free = 0;
      b->ptr = b->data;
    } else {
      b = extend_heap(last, s);
      b->ptr = b->data;
      if(!b)  
        return NULL;
    }
  } else {
    b = extend_heap(NULL, s);
    b->ptr = b->data;
    if(!b)
      return NULL;
    first_block = b;
  }
  return b->data;
}



void ff_free(void *p) {
  t_block b;
  if(valid_addr(p)) {
    b = get_block(p);
    b->free = 1;
    if(b->prev && b->prev->free)
      b = fusion(b->prev);
    if(b->next)
      fusion(b);
    else {
      if(b->prev)
	     b->prev->next = NULL;
      else
	     first_block = NULL;
    }
  }
}

void bf_free(void * p) {
  ff_free(p);
}



unsigned long get_data_segment_size(){
  unsigned long ans = 0;
  t_block temp = first_block;
    while(temp){
    ans = ans + temp ->size;
    temp = temp -> next;
   }
  return ans;
}

unsigned long get_data_segment_free_space_size(){
  unsigned long ans = 0;
  t_block temp = first_block;
  while(temp){
    if(temp -> free){
      ans = ans + temp -> size;
    }
    temp = temp -> next;
  }
  return ans;
}

pthread_mutex_t lock;

void * ts_malloc(size_t size) {
  t_block b, last;
  size_t s;
  s = align8(size);
  pthread_mutex_lock(&lock);
  if(first_block) {
    last = first_block;
    b = bf_find(&last, s);
    if(b) {
      if ((b->size - s) >= ( BLOCK_SIZE + 8)) 
        split_block(b, s);
      b->free = 0;
      b->ptr = b->data;
    } else {
      b = extend_heap(last, s);
      b->ptr = b->data;
      if(!b) {
        pthread_mutex_unlock(&lock);
        return NULL;  
      }
    }
  } else {
    b = extend_heap(NULL, s);
    b->ptr = b->data;
    if(!b) {
      pthread_mutex_unlock(&lock);
      return NULL;
    }
    first_block = b;
  }
  pthread_mutex_unlock(&lock);
  return b->data;
}


void ts_free(void * ptr) {
  pthread_mutex_lock(&lock);
  ff_free(ptr);
  pthread_mutex_unlock(&lock);
}
