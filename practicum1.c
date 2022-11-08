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
page* heap;
size_t heap_pages_in_use = 0;  // keep track of how many PAGES are allocated in
                               // heap (1 page = 4096 KB allocated)
page_table_t*
    page_table;           // keep track of pages in primary and secondary memory
int page_id = MAX_PAGES;  // unique page id for each page in heap (starts at
                          // MAX_PAGES because primary memory starts with pages
                          // 0 - 2047 and disk starts with pages 2048 - 4095)

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
    printf("bad size\n");
    return NULL;
  }
  // check if we have enough space (in pages) in heap
  if (heap_pages_in_use + 1 > MAX_PAGES) {
    printf("heap full\n");
    return NULL;
  }
  printf("yeehaw\n");
  heap_pages_in_use++;

  // find first free page in heap
  // iterate through heap until we find first free page
  // if we find a free page, allocate it and return it
  // if we don't find a free page, return NULL
  // first fit algorithm
  for (int i = 0; i < MAX_PAGES; i++) {
    printf("yee0haw\n");
    page* curr = &heap[i * PAGE_SIZE];
    printf("Current page address: %p\n", (void*)curr);
    printf("ye1ehaw\n");
    if (curr->is_free) {  // <---- SEGFAULT HERE
      printf("yeeh2aw\n");
      curr->is_free = false;
      printf("yeeha3w\n");
      curr->size = size;
      printf("yee4haw\n");
      // curr->header->address = curr;
      curr->on_disk = false;
      printf("yeeh6aw\n");
      if (!curr->page_id) {
        printf("ye7ehaw\n");
        curr->page_id = page_id;
        printf("ye8ehaw\n");
        page_id++;
      }
      printf("yee9haw\n");
      return curr;
    }
    printf("yee10haw\n");
  }
  printf("yee11haw\n");
  return NULL;
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
  block->is_free = true;
  block->size = 0;
  block->on_disk = false;
  --heap_pages_in_use;

  return;
}

/**
 * Initialize the heap.
 *
 */
void initialize_heap() {
  // Pre-allocate 8MB "heap" memory from the static store with room for metadata
  heap[MAX_PAGES];

  // segment heap into pages
  for (int i = 0; i < MAX_PAGES; i++) {
    printf("1\n");
    page* curr = &heap[i];
    printf("2\n");
    curr->is_free = true;  // <---- SEGFAULT HERE
    printf("3\n");
    curr->size = 0;
    printf("4\n");
    // curr->address = NULL;
    curr->on_disk = false;
    printf("5\n");
    curr->page_id = i;
    printf("6\n");
    // curr->data = NULL;
  }

  // heap will be an array of pages
  // track pages in primary memory by accessing heap
  // track pages in secondary memory by accessing swap file
  // initialize page table
  // initialize_page_table();

  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Heap initialized.\n");
  // printf("Address: %p\n", (void*)heap);
  printf("Start of heap address: \t%p\n", (void*)&heap[0]);
  printf("End of heap address: \t%p\n", (void*)&heap[8388608]);
  printf("Max capacity: \t\t%d bytes\n", HEAP_CAPACITY);
  printf("Page size: \t\t%d bytes\n", PAGE_SIZE);
  printf("Max number of pages: \t%d\n", HEAP_CAPACITY / PAGE_SIZE);
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("\n\n");
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
  size_t heap_pages_in_use = 0;

  while (block != NULL) {
    fragmentation += block->size;
    heap_pages_in_use += block->size;
    block = block->next;
  }

  fragmentation = fragmentation / heap_pages_in_use * 100.0;
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
  // update heap_pages_in_use
  // update page fault count
  return;
}

/**
 * Print out contents of heap
 */
void print_heap() {
  page* curr = heap;
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Heap contents:\n");
  int i = 0;
  while (curr) {
    printf("(%d) <%p> (size: %ld)\n", i, (void*)curr, curr->size);
    ++curr;
    i++;
  }
}

int main() {
  // allocate memory until our heap is full
  // call pm_malloc
  // if pm_malloc sees we are full, it will call move_to_disk
  // print heap list, move to disk, print heap again
  // demonstrate pm_malloc works when we have enough memory in heap and
  // demonstrate we can offload pages to "disk" when out of space

  // initialize heap
  printf("Initializing heap...\n");
  initialize_heap();
  // allocate memory until our heap is full
  /**

  for (int i = 0; i < 10; i++) {
    printf("Page %d address: %p\n", i, (void*)&heap[i * PAGE_SIZE]);
    printf("%p", (void*)pm_malloc(i * 50));
  }
  */
  // printf("%p", (void*)pm_malloc(0));
  // printf("%p", (void*)pm_malloc(4000));
  // printf("\n\n");
  // printf("Heap pages in use: %d\n", heap_pages_in_use);
  // print_heap();

  // print heap list, move to disk, print heap again

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
