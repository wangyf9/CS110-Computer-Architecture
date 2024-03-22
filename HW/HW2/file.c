#include "file.h"
#include "explorer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*include*/
struct file *file_new(int type, char *name) {
  /* Initialization */
  struct file *file = NULL;
  /* Check for integrity */
  if (!name || type < 0 || type >= MAX_FT_NO) {
    return NULL;
  }
  /* Allocate memory and initialze the file. */
  file = calloc(1, sizeof(struct file));
  file->type = type;
  file->size = DEFAULT_FILE_SIZE;
  file->data = calloc(file->size, 1);
  /* Crate associtate node and set it to base. */
  file->base = node_new(false, name, file);
  return file;
}

void file_release(struct file *file) {
  /* Check for null pointer. */
  if (!file) {
    return;
  }
  /* relase the data. */
  /* Check if base has already been released. Prevent circular call. */
  if (file->base) {
    file->base->inner.file = NULL;
    node_release(file->base);
  }
  /* Release the resources. */
  free(file->data);
  free(file);
}

bool file_write(struct file *file, int offset, int bytes, const char *buf) {
  /* YOUR CODE HERE */
  /*determine whether offset is negative*/
  if(offset<0){
    return false;
  }
  /*determine whether pointers are NULL*/
  if(buf==NULL||file==NULL){
    return false;
  }
  /*determine whether the size of file is less than offset+bytes. 
  Yes, we need to enlarge the size and the size of the requested memory space*/
  if(file->size<(offset+bytes)){
    file->size=offset+bytes;
    /*realloc for more memory space*/
    file->data=realloc(file->data,offset+bytes);
    for(int i=0;i<bytes;i++){
      /*assignment*/
      file->data[i+offset]=buf[i];
    }
    /*finished and return*/
    return true;
  }
  /*No condtion*/
  if(file->size>=(offset+bytes)){
    for(int i=0;i<bytes;i++){
       /*assignment*/
      file->data[i+offset]=buf[i];
    }
     /*finished and return*/
    return true;
  }
  /*finished*/
  return true;
}

bool file_read(const struct file *file, int offset, int bytes, char *buf) {
  /* YOUR CODE HERE */
  /*determine whether offset is negative*/
  if(offset<0){
    return false;
  }
  /*determine whether pointers are NULL*/
  if(file==NULL){
    return false;
  }
  /*determine whether the size of file is less than offset+bytes. Yes, return false*/
  if((offset+bytes)>file->size){
    return false;
  }
  /*No, read the data*/
  if((offset+bytes)<=file->size){
    for(int i=0;i<bytes;i++){
      /*assignment*/
      buf[i]=file->data[offset+i];
    }
    /*return*/
    return true;
  }
  /*finished*/
  return true;
}
