#include "dir.h"
#include "explorer.h"
#include "file.h"
#include "node.h"
#include <stdlib.h>
#include <string.h>
/*include*/
///static bool dir_add_sub(struct directory *dirnode, struct node *sub);

struct directory *dir_new(char *name) {
  /* Initialization */
  struct directory *dir = NULL;
  /* Check for null pointer */
  if (!name) {
    return NULL;
  }
  /* Allocate memory */
  dir = calloc(1, sizeof(struct directory));
  dir->capacity = DEFAULT_DIR_SIZE;
  dir->subordinates = calloc(dir->capacity, sizeof(struct node));
  dir->parent = NULL;
  /* Create base node */
  dir->base = node_new(true, name, dir);
  return dir;
}
#include <stdio.h>
void dir_release(struct directory *dir) {
  /* Initialization */
  int i = 0;
  if (!dir) {
    return;
  }
  /* Release all the subordiniates */
  for (i = 0; i < dir->size; i++) {
    node_release(dir->subordinates[i]);
  }
  /* Release the resources */
  /* Check if base has already been released. Prevent circular call. */
  if (dir->base) {
    dir->base->inner.dir = NULL;
    node_release(dir->base);
  }
  /* Release data and self. */
  free(dir->subordinates);
  free(dir);
}

struct node *dir_find_node(const struct directory *dir, const char *name) {
  /* YOUR CODE HERE */
  /*set a variable in order to determine whether such node exists*/
  int flag=0;
  /* Check for null pointer */
  if(dir==NULL||name==NULL){
    /*return null*/
    return NULL;
  }
  /*find the node with the same name, and return it*/
  for(int i=0;i<dir->size;i++){
      /*call strcmp function to determine whether the two are same. Yes return the result*/
    if(strcmp(dir->subordinates[i]->name,name)==0){
      flag=1;
      /*return*/
      return dir->subordinates[i];
    }
  }
  /*determine whether =0*/
  /*don't exist, return NULL*/
  if(flag==0){
    return NULL;
  }
    /*finished*/
  return NULL;
}

bool dir_add_file(struct directory *dir, int type, char *name) {
  /* YOUR CODE HERE */
  /* Check for null pointer */
  if(dir==NULL||name==NULL){
    return false;
  }
  /*call strcmp function to determine whether the two are same. YES, return false*/
  for(int i=0;i<dir->size;i++){
    if(strcmp(dir->subordinates[i]->name,name)==0){
      /*return false*/
      return false;
    }
  }
  /*determine whether the size is greater than capacity*/
  if(dir->size+1>dir->capacity){
    /*enlarge the capacity by multipling two and realloc more memory space*/
    dir->capacity=2*dir->capacity;
    dir->subordinates=realloc(dir->subordinates,dir->capacity*sizeof(struct node));
  }
/*create new file and fill it in*/
  dir->subordinates[dir->size]=file_new(type,name)->base;
  dir->size++;
/*return and finished*/
  return true;
}

bool dir_add_subdir(struct directory *dir, char *name) {
  /* YOUR CODE HERE */
  /* Check for null pointer */
  if(dir==NULL||name==NULL){
    return false;
  }
  /*call strcmp function to determine whether the two are same. YES, return false*/
  for(int i=0;i<dir->size;i++){
    if(strcmp(dir->subordinates[i]->name,name)==0){
      /*return false*/
      return false;
    }
  }
  /*determine whether the size is greater than capacity*/
  if(dir->size+1>dir->capacity){
        /*enlarge the capacity by multipling two and realloc more memory space*/
    dir->capacity=2*dir->capacity;
    dir->subordinates=realloc(dir->subordinates,dir->capacity*sizeof(struct node));
  }
/*create new file and fill it in and let its parent be correct*/
  struct directory *new_dir=NULL;
  new_dir=dir_new(name);
  /*fill it in*/
  dir->subordinates[dir->size]=new_dir->base;
  /*change parent*/
  new_dir->parent=dir;
  dir->size++;
/*return and finished*/
  return true;
}

bool dir_delete(struct directory *dir, const char *name) {
  /* YOUR CODE HERE */
   /*set a variable in order to determine whether such node exists*/
  int flag=0;
  /* Check for null pointer */
  if(dir==NULL||name==NULL){
    return false;
  }
  /*call strcmp function to determine whether the two are same. YES, release it*/
  for(int i=0;i<dir->size;i++){
    if(strcmp(dir->subordinates[i]->name,name)==0){
      flag=1;
      /*release*/
      node_release(dir->subordinates[i]);
/*fill the void by removing the last element it is more convenient to move one by one*/
      dir->subordinates[i]=dir->subordinates[dir->size-1];
      dir->size--;
      /*return ture*/
      return true;

    }
  }
  /*without vaild release return false*/
  if(flag==0){
    return false;
  }
  /*return false and finished*/
  return false;
}
