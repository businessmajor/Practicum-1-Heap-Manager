#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef PAGE_TABLE_H
#define HEAP_CAPACITY 8 * 1024 * 1024  // 8 MB heap
#define PAGE_SIZE 4096                 // 4 KB page size
#define MAX_PAGES 2048  // 2048 pages (4 KB each) fit in our heap (8 MB)

#endif

// header has important metadata
typedef struct page_header {
  void* address;  // pointer to page start address (start of header)
  size_t size;    // how many bytes allocated in the page
  void* next;     // pointer to next page as part of this block
  bool is_free;   // true if page is free
  bool on_disk;   // true if page is on disk
} page_header;

// a page has a header info and space for data
// this struct is 16 bytes
typedef struct page {
  page_header* header;
  void* data;
} page;

// keep track of pages in primary memory (RAM) and disk memory
typedef struct page_table {
  int page;
  int frame;
} page_table;
