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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// header has important metadata
typedef struct page_header {
  void* start;   // pointer to start of page
  size_t size;   // how many bytes allocated in the page
  void* next;    // pointer to next page as part of this block
  bool is_free;  // true if block is free
  bool on_disk;  // true if page is on disk
} page_header;

// a page has header info and space for data
typedef struct page {
  page_header* header;
  void* data;
} page;

// keep track of pages in primary memory (RAM), disk memory, and free list
typedef struct page_list {
  page* blocks[2048];
  int count;
} page_list;

/******************************
 *******GLOBAL VARIABLES*******
 ******************************/
page_list*
    primary_memory_page_list;  // track what pages are in primary memory (RAM
page_list* disk_page_list;     // track what pages are in disk memory
page_list* free_list;          // track of pages that are free in heap
page* heap;
size_t heap_size = 0;  // keep track of how many PAGES are allocated in heap (1
                       // = 4096 KB allocated)

//
// SHOULD WE KEEP TRACK OF FREE BLOCKS VIA FREE LIST, OR SHOULD WE JUST ITERATE
// THROUGH THE HEAP??
//

// I THINK WE SHOULD ITERATE THROUGH HEAP AND FIND FREE PAGE BY CHECKING
// page->is_free. USING FIRST FIT ALGORITHM (ITERATE THROUGH HEAP UNTIL WE FIND
// FIRST FREE PAGE)

// WHEN WE CALL FREE, WE CHECK block->start TO FIND ADDRESS, FREE IT, AND
// NOTHING ELSE (DO NOT COMPACT FOR FRAGMENTATION) TOO MUCH COMPUTATION TO SHIFT
// EVERY PAGE UP IF WE FREE A PAGE IN THE MIDDLE OF HEAP JUST TAKE NEXT PAGE
// NEXT TIME IT CALLS PM_MALLOC

// I LEFT FREE_LIST CODE IN CASE WE DECIDE TO USE IT TO KEEP TRACK

/**
 * Insert a specified block of memory into the free list.
 *
 * @param   block  pointer to the block we wish to insert
int free_list_insert(page* block) {}
 */

/**
 * Remove a specified block of memory from the free list.
 *
 * @param   block   pointer to the block we wish to remove
void free_list_remove(page* block) {}
*/

/**
 * Return the number of blocks in the free list.
 *
 * @return  count of blocks in free list
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
*/

/**
 * Print out contents of free list
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
*/

/**
 * Allocate specified amount memory.
 * ASSUMPTION: Nobody will request more than 4096 KB (page size)
 *
 * @param   size    Amount of bytes to allocate.
 * @return  Pointer to the requested amount of memory.
 **/
page* pm_malloc(size_t size) {
  if (!size || size < 1) {
    return NULL;
  }
  // check if we have enough space (in pages) in heap
  assert(heap_size + 1 <= HEAP_CAPACITY / PAGE_SIZE);
  heap_size++;
  page* ret_address = heap + (heap_size * PAGE_SIZE);
  ret_address->header->is_free = false;
  ret_address->header->size = size;
  ret_address->header->start = ret_address;
  ret_address->header->next = NULL;
  ret_address->header->on_disk = false;
  ret_address->data = BLOCK_DATA(ret_address);

  return ret_address;
}

/**
 * Free previously allocated memory block
 *
 *
 * @param   block   Pointer to block to release.
 * @return  Whether or not the release completed successfully.
 */
void pm_free(page* block) {
  if (block == NULL) {
    return;
  }
  block->header->is_free = true;

  // find block in page_list
  // remove block from page_list
  // add block to free_list
  return;
}

/**
 *
void initialize_heap() {
  // Pre-allocate 8MB "heap" memory from the static store
  heap[HEAP_CAPACITY];
  // Allocate all virtual memory structures within the block as well.
  // allocate memory for primary memory page list
  //primary_memory_page_list = (page_list*)pm_malloc(sizeof(page_list));
  // allocate memory for disk page list
  //disk_page_list = (page_list*)pm_malloc(sizeof(page_list));

  // initialize primary and disk page list
  //primary_memory_page_list->count = 0;
  //disk_page_list->count = 0;
  // initialize heap
  heap->header = (page_header*)pm_malloc(sizeof(page_header));
  heap->header->size = HEAP_CAPACITY / PAGE_SIZE;  // 2048 pages
  heap->header->next = NULL;
  heap->header->is_free = true;
  heap->header->on_disk = false;
  heap->data = BLOCK_DATA(heap);
  // add heap to primary memory page list
  primary_memory_page_list->blocks[primary_memory_page_list->count] = heap;
  primary_memory_page_list->count++;
}
*/

// I CAN TAKE CARE OF THESE FUNCTIONS- WILL USE THEM TO CALCULATE HOW MUCH
// FRAGMENTATION WE HAVE

/**
 * Compute internal fragmentation in heap.
 *
 * https://www.edn.com/design/systems-design/4333346/Handling-memory-fragmentation
 *
 * @return  Degree of internal fragmentation as a percent of the whole memory
 * heap.

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
*/

/**
 * Compute external fragmentation in heap.
 *
 * https://www.edn.com/design/systems-design/4333346/Handling-memory-fragmentation
 *
 * @return  Degree of external fragmentation as a percent of the whole memory
 * heap.

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
 */

// MOVE PAGES TO DISK USING FIFO OR LRU STRATEGY
void move_to_disk() {}

int main() {
  // allocate memory until our heap is full
  // call pm_malloc
  // if pm_malloc sees we are full, it will call move_to_disk
  // print heap list, move to disk, print heap again
  // demonstrate pm_malloc works when we have enough memory in heap and
  // demonstrate we can offload pages to "disk" when out of space
  return 0;
}

// WHAT WE NEED
/*
pm_malloc();
pm_free();
void move_to_disk();
void move_to_heap();
page* heap[HEAP_CAPACITY];            // array implementation
page* disk_page_list[HEAP_CAPACITY];  // array implementation
page* page_table[HEAP_CAPACITY];      // hash map implementation

// WHAT IS LEFT
//pm_free();
void move_to_disk();                  // FIFO
void move_to_heap();                  // FIFO
page* disk_page_list[HEAP_CAPACITY];  // array implementation
page* page_table[HEAP_CAPACITY];      // hash map implementation
*/