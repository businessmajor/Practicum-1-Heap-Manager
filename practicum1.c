/******************************
 ******SUBJECT TO CHANGE*******
 ******************************/
// define start of memory and adjustment for header overhead
#define BLOCK_DATA(ptr) ((void*)((unsigned long)ptr + sizeof(page)))
#define BLOCK_HEADER(ptr) ((void*)((unsigned long)ptr - sizeof(page)))
// our heap is the same size as a Playstation 2 Memory Card!
// with a 4KB page size, we can have 2048 pages
#define HEAP_CAPACITY 8 * 1024 * 1024
#define PAGE_SIZE 4096

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct page {
  page_header* block;
  void* data;
} page;

typedef struct page_header {
  int size;      // how many pages the block is
  void* next;    // pointer to next page as part of this block
  bool is_free;  // true if block is free
  bool on_disk;  // true if page is on disk
} page_header;

typedef struct primary_memory_page_list {
  page* blocks[2048];
  int count;
} primary_memory_page_list_t;

typedef struct disk_page_list {
  page* blocks[2048];
  int count;
} disk_page_list_t;
