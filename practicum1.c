/******************************
 ******MACROS AND GLOBAL*******
 ******************************/
// define start of memory and adjustment for header overhead
#define BLOCK_DATA(ptr) ((void*)((unsigned long)ptr + sizeof(page)))
#define BLOCK_HEADER(ptr) ((void*)((unsigned long)ptr - sizeof(page)))
// our heap is the same size as a Playstation 2 Memory Card!
// with a 4KB page size, we can have 2048 pages
#define HEAP_CAPACITY 8 * 1024 * 1024  // 8 MB heap
#define PAGE_SIZE 4096                 // 4 KB page size
#define MAX_PAGES 2048  // 2048 pages (4 KB each) fit in our heap (8 MB)

#include "page_table.h"

/******************************
 *******GLOBAL VARIABLES*******
 ******************************/
char* heap;
size_t heap_size = 0;  // keep track of how many PAGES are allocated in heap
                       // (1 page )= 4096 KB allocated)
page_table* page_table;  // keep track of pages in primary memory
page_table* secondary_page_table;  // keep track of pages in secondary
                                            // memory (swap file)

//
// KEEP TRACK OF FREE BLOCKS BY ITERATING THROUGH THE HEAP
//

// I THINK WE SHOULD ITERATE THROUGH HEAP AND FIND FREE PAGE BY CHECKING
// page->is_free. USING FIRST FIT ALGORITHM (ITERATE THROUGH HEAP UNTIL WE FIND
// FIRST FREE PAGE)

// WHEN WE CALL FREE, WE CHECK block->start TO FIND ADDRESS, FREE IT, AND
// NOTHING ELSE (DO NOT COMPACT FOR FRAGMENTATION) TOO MUCH COMPUTATION TO SHIFT
// EVERY PAGE UP IF WE FREE A PAGE IN THE MIDDLE OF HEAP JUST TAKE NEXT PAGE
// NEXT TIME IT CALLS PM_MALLOC

/**
 * Allocate specified amount memory.
 * ASSUMPTION: Nobody will request more than 4000 KB (page size - metadata)
 *
 * @param   size    Amount of bytes to allocate.
 * @return  Pointer to the requested amount of memory.
 **/
page* pm_malloc(size_t size) {
  // check null size, too small, or too big (max allocable is 4096 - metadata)
  if (!size || size < 1 || size > PAGE_SIZE - sizeof(page)) {
    return NULL;
  }
  // check if we have enough space (in pages) in heap
  assert(heap_size + 1 <= MAX_PAGES);
  heap_size++;

  page* ret_address = heap + (heap_size * PAGE_SIZE);
  ret_address->header->is_free = false;
  ret_address->header->size = size;
  ret_address->header->address = ret_address;
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
  block->header->size = 0;
  block->header->address = NULL;
  block->header->next = NULL;
  block->header->on_disk = false;
  block->data = NULL;
  heap_size--;
  return;

  // find block in page_table

  // remove block from page_table
  // add block to free_list
  return;
}

/**
 * Initialize the heap.
 *
 */
void initialize_heap() {
  // Pre-allocate 8MB "heap" memory from the static store with room for metadata
  heap[HEAP_CAPACITY];
  // heap will be an array of pages
  // track pages in primary memory by accessing heap


  printf("Heap initialized.\n");
  // printf("Address: %p\n", (void*)heap);
  // printf("Address: %p\n", (void*)&heap[0]);
  // printf("Address: %p\n", (void*)&heap[1]);
  // printf("Size: %lu bytes\n", sizeof(heap));
  // printf("Max capacity: %d bytes\n", HEAP_CAPACITY);
  // printf("Page size: %d bytes\n", PAGE_SIZE);
  // printf("Number of pages: %d\n", HEAP_CAPACITY / PAGE_SIZE);
}

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

void move_to_disk() {
  // find page to move to disk
  // move page to disk
  // update page table
  // update disk page list
  // update heap
  // update heap_size
  // update page fault count
  return;
}

int main() {
  // allocate memory until our heap is full
  // call pm_malloc
  // if pm_malloc sees we are full, it will call move_to_disk
  // print heap list, move to disk, print heap again
  // demonstrate pm_malloc works when we have enough memory in heap and
  // demonstrate we can offload pages to "disk" when out of space

  // initialize heap
  initialize_heap();
  // allocate memory until our heap is full

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
