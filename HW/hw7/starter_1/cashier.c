#include "cashier.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>/*include header*/
int log_int(uint64_t x);
struct cashier *cashier_init(struct cache_config config) {
  // YOUR CODE HERE   
  struct cashier* newcashier = (struct cashier*)malloc(sizeof(struct cashier)); 
  //newcashier=(struct cachier*)malloc(sizeof(struct cachier));/*allocate*/
  if(!newcashier){/*fail to allocate*/
    return NULL;
  }
  newcashier->lines=(struct cache_line*)malloc(config.lines*sizeof(struct cache_line));
  if(!newcashier->lines){/*fail to allocate*/
    free(newcashier); 
    return NULL;/*return*/
  }
  for(uint64_t i=0;i<config.lines;i++){
    newcashier->lines[i].data=(uint8_t*)malloc((config.line_size)*sizeof(uint8_t));/*every cache line store cache line size data, and data here is uint8*/
    /*fail to allocate*/
    if(!newcashier->lines[i].data){
      for(uint64_t j=i-1;j>=0;j--){
        free(newcashier->lines[j].data);/*error free others*/
        if(j==0){
          free(newcashier->lines);
          free(newcashier);
          return NULL;/*return*/
        }
      }/*loop*/
    }
  }
  newcashier->config=config;
  uint64_t index_bit=log_int(config.lines/config.ways);/*number of lines = associativity*sets*/
  uint64_t offset_bit=log_int(config.line_size);/*offset*/
  uint64_t tag_bit=config.address_bits-index_bit-offset_bit;/*=tags+index(number of set)+offset(cache line size)*/
  //int associativity_bit=(int)log(config.ways);/*associativity*/
  newcashier->size=config.lines*config.line_size; /*cache size = lines(index*associativity)*offset*/
  newcashier->index_bits=index_bit;
  newcashier->offset_bits=offset_bit;
  newcashier->tag_bits=tag_bit;/*equal*/

  newcashier->index_mask=((1<<index_bit)-1)<<offset_bit; /*second*/
  newcashier->offset_mask=(1<<offset_bit)-1; /*first*/
  newcashier->tag_mask=((1<<tag_bit)-1)<<(index_bit+offset_bit);  /*third*/
  for(uint64_t i=0;i<config.lines;i++){//initialize
    newcashier->lines[i].dirty=0;
    newcashier->lines[i].last_access=0;
    newcashier->lines[i].tag=0;
    newcashier->lines[i].valid=0;
  }
    //printf("task_mask=%ld",newcashier->tag_bits);
  return newcashier;
}

void cashier_release(struct cashier *cache) {
  // YOUR CODE HERE
  /*write back data before releasing*/
  uint64_t index;
  uint64_t inverse_addr;/*set variables*/
  for(uint64_t i=0;i<cache->config.lines;i++){
    printf("whether dirty%d\n",cache->lines->dirty);
  }
  for(uint64_t i=0;i<cache->config.ways;i++){
    for(uint64_t j=0;j<(cache->config.lines/cache->config.ways);j++){/*travel in given order*/
      index=i+j*cache->config.ways;
      if(cache->lines[index].valid==1){/*dirty and valid write back*/ /***********wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!! First valid. Then dirty. valid clean doesnot need write back, but another need*/
        inverse_addr=(cache->lines[index].tag<<(cache->offset_bits+cache->index_bits))
                        +(j<<cache->offset_bits);
        before_eviction(j,&cache->lines[index]);
        if(cache->lines[index].dirty==1){
          for(uint64_t k=0;k<cache->config.line_size;k++){/*execute write back*/
            mem_write(inverse_addr+k,cache->lines[index].data[k]);
          }
        }
      }
    }/*loop*/
  }
  for(uint64_t i=0;i<cache->config.lines;i++){
    free(cache->lines[i].data);/*free*/
  }
  free(cache->lines);/*free*/
  free(cache);
}


/*****************************************************************************************************************************************no consider about set */
bool cashier_read(struct cashier *cache, uint64_t addr, uint8_t *byte) {
  // YOUR CODE HERE
  uint64_t inverse_addr;
  uint64_t min;/*set variables*/
  uint64_t index_min;
  uint64_t offset=(addr&cache->offset_mask);
  uint64_t tag=((addr&cache->tag_mask)>>(cache->index_bits+cache->offset_bits));/*tag*/
  uint64_t set=(addr&(cache->index_mask))>>cache->offset_bits;/*set*/
  /*get the set index of addr*/
  uint64_t num_of_set_in_total=cache->config.lines/cache->config.ways;/*get the num of set*/
  uint64_t read_from_memory_initial_address=(addr&cache->tag_mask)+(addr&cache->index_mask);
  uint64_t set_in_lines=set*cache->config.ways;
  for(uint64_t i=set_in_lines;i<(set_in_lines+cache->config.ways);i++){/*this is equivalent to travel every this set index*/
    /*every time we just need to check different ways but same set index*/

      if((cache->lines[i].tag==tag)&&(cache->lines[i].valid==1)){/*cache hit*//*don't have considered about whether valid                      1110*/
          byte[0]=cache->lines[i].data[offset];/*not change last_access parameter                                              555*/
          cache->lines[i].last_access=get_timestamp();
       ///     printf("read_index=[%ld]",i);
          return true;
      }
  }
 // puts("1111");
  /*at this position, that means cache miss*/
 for(uint64_t i=set_in_lines;i<(set_in_lines+cache->config.ways);i++){/*this is equivalent to travel every this set index*/
    if(cache->lines[i].valid==0){/*invalid*/
      for(uint64_t j=0;j<cache->config.line_size;j++){
              cache->lines[i].data[j]=mem_read(read_from_memory_initial_address+j);/*read from*/
      }
      byte[0]=cache->lines[i].data[offset];
      cache->lines[i].tag=tag;
      cache->lines[i].dirty=0;
      cache->lines[i].valid=1;/*set parameters*/
      cache->lines[i].last_access= get_timestamp();
      return false;
    }
  }
   // puts("2222222");
  /*at this position, that means there are no invalid line to replace directly and means there are no empty entries at the meantime, we only can find a evictor*/
  min=cache->lines[set_in_lines].last_access;
    index_min=set_in_lines;
  // printf("%ld",num_of_set_in_total);
 for(uint64_t i=set_in_lines;i<(set_in_lines+cache->config.ways);i++){/*this is equivalent to travel every this set index*//*find the least recently use entry*/
    if(cache->lines[i].last_access<min){
      // printf("%ld",i);
      min=cache->lines[i].last_access;
      index_min=i;/*get it*/
    }
  }
  //  puts("3333333");
  /*valid*/
  /*check whether dirty*/
  before_eviction(set,&(cache->lines[index_min]));
  if(cache->lines[index_min].dirty==1){/*not dirty, clean, don't need to write back*/
      inverse_addr=(cache->lines[index_min].tag<<(cache->offset_bits+cache->index_bits))
              +(set<<cache->offset_bits);/*what about offset, don't care about it*/
      for(uint64_t i=0;i<cache->config.line_size;i++){
          mem_write(inverse_addr+i,cache->lines[index_min].data[i]);/******inversely calculate addr write it back*/
      }
  }
   // puts("4444444");
  /*at this point it is dirty*/
  /*****************************************no consider about set */
  for(uint64_t j=0;j<cache->config.line_size;j++){
        cache->lines[index_min].data[j]=mem_read(read_from_memory_initial_address+j);/*read from*/
  }
  byte[0]=cache->lines[index_min].data[offset];
  cache->lines[index_min].tag=tag;/*get tag*/
  cache->lines[index_min].dirty=0;
  cache->lines[index_min].valid=1;/*set parameters*/
  cache->lines[index_min].last_access=get_timestamp();
  return false;
}

bool cashier_write(struct cashier *cache, uint64_t addr, uint8_t byte) {/*the main point whhich is different from read operation is changing the dirty bit, and change the data in the cache block because the strategy is write back not write through*/
  // YOUR CODE HERE
     // puts("10101010101");
 // printf("task_mask=%ld",cache->tag_mask);
  uint64_t inverse_addr;
  uint64_t min;/*set variables*/
  uint64_t index_min;
  uint64_t offset=(addr&cache->offset_mask);
  uint64_t tag=(addr&cache->tag_mask)>>(cache->index_bits+cache->offset_bits);/*tag*/
  uint64_t set=(addr&cache->index_mask)>>cache->offset_bits;/*set*/
  /*get the set index of addr*/
  uint64_t num_of_set_in_total=cache->config.lines/cache->config.ways;/*get the num of set*/
  uint64_t read_from_memory_initial_address=(addr&cache->tag_mask)+(addr&cache->index_mask);
  uint64_t set_in_lines=set*cache->config.ways;
  for(uint64_t i=set_in_lines;i<(set_in_lines+cache->config.ways);i++){/*this is equivalent to travel every this set index*/
    /*every time we just need to check different ways but same set index*/
   //   printf("tag=[%ld]\n",tag);
      if((cache->lines[i].tag==tag)&&(cache->lines[i].valid==1)){/*cache hit*//*don't have considered about whether valid                      1110*/
      //    printf("write_index=[%ld]\n",i);
          cache->lines[i].data[offset]=byte;/*not change last_access parameter                                              555*/
          cache->lines[i].dirty=1;
          cache->lines[i].last_access = get_timestamp();/*set access order*/
          return true;
      }
      //puts("44444444444444");
  }
  /*at this position, that means cache miss*/
 // puts("000000000000000");
 for(uint64_t i=set_in_lines;i<(set_in_lines+cache->config.ways);i++){/*this is equivalent to travel every this set index*/
    if(cache->lines[i].valid==0){/*invalid*/ /*for writing, when cache miss, we also need read from memory first*/
      for(uint64_t j=0;j<cache->config.line_size;j++){
              cache->lines[i].data[j]=mem_read(read_from_memory_initial_address+j);/*read from*/
      }
     //   puts("====1111====");
      cache->lines[i].tag=tag;
      //cache->lines[i].dirty=0;
      cache->lines[i].valid=1;/*set parameters*/
      cache->lines[i].data[offset]=byte; 
      cache->lines[i].dirty=1; /*change, setting dirty*/
      cache->lines[i].last_access = get_timestamp();
      return false;
    }
  }
//  puts("5555555555555");
  /*at this position, that means there are no invalid line to replace directly and means there are no empty entries at the meantime, we only can find a evictor*/
  min=cache->lines[set_in_lines].last_access;
  index_min=set_in_lines;
   //printf("%ld",num_of_set_in_total);
 for(uint64_t i=set_in_lines;i<(set_in_lines+cache->config.ways);i++){/*this is equivalent to travel every this set index*//*find the least recently use entry*/
    if(cache->lines[i].last_access<min){
       //printf("%ld",i);
      min=cache->lines[i].last_access;
      index_min=i;/*get it*/
    }
  }
  /*valid*/
  /*check whether dirty*/
  before_eviction(set,&(cache->lines[index_min]));
  if(cache->lines[index_min].dirty==1){/*not dirty, clean, don't need to write back*/
   // puts("====222222====");
        inverse_addr=(cache->lines[index_min].tag<<(cache->offset_bits+cache->index_bits))
              +(set<<cache->offset_bits);/*what about offset, don't care about it*/
          for(uint64_t i=0;i<cache->config.line_size;i++){
              mem_write(inverse_addr+i,cache->lines[index_min].data[i]);/******inversely calculate addr write it back*/
          }
  }
  /*at this point it is dirty*/
  

 /*****************************************no consider about set */
  for(uint64_t j=0;j<cache->config.line_size;j++){
        cache->lines[index_min].data[j]=mem_read(read_from_memory_initial_address+j);/*read from*/
  }
  cache->lines[index_min].tag=tag;
  //cache->lines[index_min].dirty=0;
  cache->lines[index_min].valid=1;/*set parameters*/
  cache->lines[index_min].data[offset]=byte; 
  cache->lines[index_min].dirty=1; /*change, setting dirty*/
  cache->lines[index_min].last_access = get_timestamp();
  return false;
}
int log_int(uint64_t x){
  int pow=0;
  while(x>1){
    x=x>>1;
    pow++;/*calculate bits*/
  }
  return pow;
}
