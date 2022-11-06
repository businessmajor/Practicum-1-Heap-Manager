/******************************
 ******MACROS AND GLOBAL*******
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

// a page has header info and space for data
typedef struct page {
  page_header* header;
  void* data;
} page;

// header has important metadata
typedef struct page_header {
  int size;      // how many bytes allocated in the page
  void* next;    // pointer to next page as part of this block
  bool is_free;  // true if block is free
  bool on_disk;  // true if page is on disk
} page_header;

// keep track of pages in primary memory (RAM)
typedef struct page_list {
  page* blocks[2048];
  int count;
} page_list;


/******************************
 *******GLOBAL VARIABLES*******
 ******************************/
page_list* primary_memory_page_list;  // track what pages are in primary memory (RAM
page_list* disk_page_list;  // track what pages are in disk memory
page_list* free_list;       // track of pages that are free in heap
page* heap;
size_t heap_size = 0;  // keep track of how many PAGES are allocated in heap (1 = 4096 KB allocated)


/**
 * Insert a specified block of memory into the free list.
 *
 * @param   block  pointer to the block we wish to insert
 */
int free_list_insert(page* block) {}

/**
 * Remove a specified block of memory from the free list.
 *
 * @param   block   pointer to the block we wish to remove
 */
void free_list_remove(page* block) {}

/**
 * Return the number of blocks in the free list.
 *
 * @return  count of blocks in free list
 */
size_t free_list_length() {
  if (free_list == NULL) {
    return 0;
  }
  page* block = free_list;
  size_t length = 1;
  while (block != NULL) {
    length++;
    block = block->next;
  }
  return length;
}

/**
 * Print out contents of free list
 */
void free_list_print() {
  page* curr = free_list;
  printf("free list:\n");
  int i = 0;
  while (curr) {
    printf("(%d) <%p> (size: %ld)\n", i, curr, curr->size);
    curr = curr->next;
    i++;
  }
}

/**
 * Allocate specified amount memory.
 * ASSUMPTION: Nobody will request more than 4096 KB (page size)
 * 
 * 
 * @param   size    Amount of bytes to allocate.
 * @return  Pointer to the requested amount of memory.
 **/
void* pm_malloc(size_t size) {
  if (!size || size < 1 || size > PAGE_SIZE) {
    return NULL;
  }
  // check if we have enough space
  assert(heap_size + size <= HEAP_CAPACITY);
  heap_size += size;
  void* ret_address = heap + heap_size;

  /*
  const page block = {
      .address = result,
      .size = size,
  };

  // find out how many pages to alloc based on 4096 byte pages
  size_t pages = size / PAGE_SIZE;
  if (size % PAGE_SIZE) {
    pages++;
  }
  page_count += pages;

  page_list.blocks[page_list.count++] = block;
  */

  return ret_address;
}

/**
 * Free previously allocated memory block
 *
 *
 * @param   block   Pointer to block to release.
 * @return  Whether or not the release completed successfully.
 */
bool pm_free(page* block) {
  if (block == NULL) {
    return false;
  }
  // find block in page_list
  // remove block from page_list
  // add block to free_list
  return true;
}

void initialize_heap() {
  // Pre-allocate 8MB "heap" memory from the static store
  heap[HEAP_CAPACITY];
  // Allocate all virtual memory structures within the block as well.
  // allocate memory for primary memory page list
  //primary_memory_page_list = (primary_memory_page_list_t*)pm_malloc(sizeof(primary_memory_page_list_t));
  // allocate memory for disk page list
  //disk_page_list = (disk_page_list_t*)pm_malloc(sizeof(disk_page_list_t));
  
  // initialize primary and disk page list
  primary_memory_page_list->count = 0;
  disk_page_list->count = 0;
  // initialize heap
  heap->header = (page_header*)pm_malloc(sizeof(page_header));
  heap->header->size = HEAP_CAPACITY / PAGE_SIZE; // 2048 pages
  heap->header->next = NULL;
  heap->header->is_free = true;
  heap->header->on_disk = false;
  heap->data = BLOCK_DATA(heap);
  // add heap to primary memory page list
  primary_memory_page_list->blocks[primary_memory_page_list->count] = heap;
  primary_memory_page_list->count++;
}

/**
 * Compute internal fragmentation in heap.
 *
 * https://www.edn.com/design/systems-design/4333346/Handling-memory-fragmentation
 *
 * @return  Degree of internal fragmentation as a percent of the whole memory
 * heap.
 */
double internal_fragmentation() {
  double fragmentation = 0.0;
  page* block = heap;
  size_t heap_size = 0;

  while (block != NULL) {
    fragmentation += block->size;
    heap_size += block->size;
    block = block->next;
  }

  fragmentation = fragmentation / heap_size * 100.0;
  return fragmentation;
}

/**
 * Compute external fragmentation in heap.
 *
 * https://www.edn.com/design/systems-design/4333346/Handling-memory-fragmentation
 *
 * @return  Degree of external fragmentation as a percent of the whole memory
 * heap.
 */
double external_fragmentation() {
  double fragmentation = 0.0;
  page* block = heap;
  size_t largest_free_block = 0;
  size_t all_free_memory = 0;

  while (block != NULL) {
    if (block->size > largest_free_block) {
      largest_free_block = block->size;
    }
    all_free_memory += block->size;
    block = block->next;
  }

  fragmentation = (1 - (largest_free_block / all_free_memory)) * 100.0;
  return fragmentation;
}
