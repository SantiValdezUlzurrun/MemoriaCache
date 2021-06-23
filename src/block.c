#include "block.h"
#include <string.h>

void block_init(block_t *block, int block_size){
  block -> bs = block_size;
  block -> valid = false;
  block-> dirty = false;
  block -> direccion = 0x0;
  block -> antiguedad = 0;
  block -> data = calloc(block_size, sizeof(char));
}

void write_block(block_t *block, unsigned char *mem_block){
  set_valid(block);
  memcpy(block -> data, mem_block, block -> bs);
}

char read_byte_b(block_t *block, unsigned int offset){
  return (block -> data)[offset]; 
}

void write_byte_b(block_t *block, unsigned int offset, unsigned char value){
  (block -> data)[offset] = value;
}

bool tag_compare(block_t *block, unsigned int tag){
  return block -> tag == tag;
}

bool is_valid(block_t *block){
  return block -> valid;
}

void set_valid(block_t *block){
  block -> valid = true;
}

void set_tag(block_t *block, unsigned int tag_){
  block -> tag = tag_;
}

void block_destroy(block_t *block) {
  free(block -> data);
}