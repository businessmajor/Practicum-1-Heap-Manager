/**
 * @file practicum1.c
 * @author Edgar Alan David and Stephano Barrios-Pompei
 * @brief This program demonstrates an implementation of a programmed managed
 * heap. It allows programs to allocate memory for use from an 8 MB heap sourced
 * from the static store.
 * @version 0.1
 * @date 2022-11-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/******************************
 ******MACROS AND STRUCTS******
 ******************************/
// define start of memory and adjustment for header overhead
#define BLOCK_DATA(ptr) ((void*)((unsigned long)ptr + sizeof(page)))
#define BLOCK_HEADER(ptr) ((void*)((unsigned long)ptr - sizeof(page)))
// our heap is the same size as a Playstation 2 Memory Card!
// with a 4KB page size, we can have 2048 pages
#define HEAP_CAPACITY 8 * 1024 * 1024  // 8 MB heap
#define PAGE_SIZE 4096                 // 4 KB page size
#define MAX_PAGES 2048  // 2048 pages (4 KB each) fit in our heap (8 MB)
#define UPPER_LIMIT_FOR_TEST 4000
#define LOWER_LIMIT_FOR_TEST 1028

typedef struct page {
  int page_id;   // unique page id
  size_t size;   // how many bytes allocated in the page
  bool is_free;  // true if page is free
  bool on_disk;  // true if page is on disk
} page;

// keep track of pages in primary memory (RAM) and disk memory
typedef struct pte {
  int page_id;
  int frame;
} pte;

page* pm_malloc(size_t size);

/******************************
 *******GLOBAL VARIABLES*******
 ******************************/
// Pre-allocate 8MB "heap" memory from the static store with room for metadata
page heap[MAX_PAGES];
size_t heap_pages_in_use = 0;  // keep track of how many PAGES are allocated in
                               // heap (1 page = 4096 KB allocated)
pte* page_table;  // keep track of pages in primary and secondary memory
page disk_list[MAX_PAGES];  // keep track of pages on disk (secondary memory)
int page_id = 1;  // unique page id for each page in heap (start as 1 to avoid
                  // confusion with NULL or 0. 0 is NOT a valid page_id)

pte* hash_arr[MAX_PAGES];  // create hash_arr with defined size
pte* dummy_item;
int page_fault_cnt;  // page fault count
int cap = 5;         // sample storage capacity value = 5
int p[100];          // page sequence

/**
 * Hash function for page table
 *
 * @param int page_id
 * @return int
 */
int hash_code(int page_id) {
  return page_id % (HEAP_CAPACITY);
}

/**
 * Initialize page table
 *
 */
pte* initialize_page_table() {
  dummy_item = (pte*)pm_malloc(sizeof(pte));
  dummy_item->page_id = -1;
  dummy_item->frame = -1;

  // initialize page table
  for (int i = 0; i < MAX_PAGES; i++) {
    hash_arr[i] = NULL;
  }

  return dummy_item;
}

/**
 * Search for the frame number with a given page key
 *
 * @param int page_id
 * @return pte*
 */
pte* get_frame(int page_id) {
  // get the hash code
  int hash_idx = hash_code(page_id);

  // move in array that is empty
  while (hash_arr[hash_idx] != NULL) {
    if (hash_arr[hash_idx]->page_id == page_id)
      return hash_arr[hash_idx];

    // next page
    ++hash_idx;

    // return to start of table
    hash_idx %= HEAP_CAPACITY;
  }

  return NULL;
}

/**
 * Insert page and frame number in pte
 *
 * @param page_id
 * @param frame
 */
void insert_page_frame(int page_id, int frame) {
  // allocate heap memory for each item in pte
  pte* pair = (pte*)malloc(sizeof(pte));

  // assign page & frame values
  pair->page_id = page_id;
  pair->frame = frame;

  // get the hash code
  int hash_idx = hash_code(page_id);

  // look for an empty or remove page line
  while (hash_arr[hash_idx] != NULL && hash_arr[hash_idx]->page_id != -1) {
    // next page
    ++hash_idx;

    // return to start of table
    hash_idx %= HEAP_CAPACITY;
  }

  hash_arr[hash_idx] = pair;
}

/**
 * Delete page and frame line in pte
 *
 * @param item
 * @return pte*
 */
pte* delete_pf_pair(int page_id) {
  // get hash code
  int hash_idx = hash_code(page_id);

  // check in the array until empty
  while (hash_arr[hash_idx] != NULL) {
    if (hash_arr[hash_idx]->page_id == page_id) {
      pte* temp = hash_arr[hash_idx];

      // assign a dummy item at removed position
      hash_arr[hash_idx] = dummy_item;

      // free heap memory - cant free because this is page table
      // free(item);
      return temp;
    }

    // check next cell position
    ++hash_idx;

    // return to start of table
    hash_idx %= HEAP_CAPACITY;
  }

  return NULL;
}

/**
 * @brief Print contents of pte
 *
 */
void show_page_table() {
  int i = 0;

  for (i = 0; i < HEAP_CAPACITY; i++) {
    if (hash_arr[i] != NULL)
      printf(" (%d,%d)", hash_arr[i]->page_id, hash_arr[i]->frame);
    else
      printf(" ~~ ");
  }

  printf("\n");
}

// checks if item is found in page table
void page_found_display(pte* item) {
  if (item != NULL)
    printf("Page: %d | Frame: %d\n", item->page_id, item->frame);
  else
    printf("Frame not found!\n");
}

// helper method - initialize the capacity of page storage to value -1
void initialize_cap() {
  // initialize page fault count
  page_fault_cnt = 0;
  int i;

  for (i = 0; i < cap; i++)
    p[i] = -1;
}

// helper method - check for page hits true/false
bool isPageHit(int value) {
  bool pageHit = false;
  int i;

  // iterage over the capacity & check for identical page
  for (i = 0; i < cap; i++) {
    if (p[i] == value) {
      pageHit = true;
      break;
    }
  }

  return pageHit;
}

// helper method - show the pages on storage
void showPages_cap() {
  int i;
  for (i = 0; i < cap; i++) {
    if (p[i] != -1)
      printf(" %d", p[i]);
  }
}

/**
 * FIFO Page replacement algorithm
 *
 */
void fifo() {
  // initialize capacity/storage
  initialize_cap();
  int i, j;

  // traverse 15 pages
  for (i = 0; i < 10; i++) {
    printf("\nFIFO - For %d :", heap[i].page_id);

    // simulated page hit for FIFO and LRU
    if (i == 7) {
      heap[i].page_id = 29;
    }

    if (isPageHit(heap[i].page_id)) {
      printf("page hit!");
    } else {
      // ^returns 1. if memory cell is empty; 2. page is not in storage
      for (j = 0; j < cap - 1; j++)
        p[j] = p[j + 1];

      p[j] = heap[i].page_id;

      // increment page fault count
      page_fault_cnt++;
      showPages_cap();
    }
  }
}

// LRU page replacement
void lru() {
  int i, j, k, m;
  int l[100];
  int min, idx, x;
  bool found;

  // initialize capacity/storage
  initialize_cap();

  // traverse the pages
  for (i = 0; i < 10; i++) {
    printf("\nLRU - For %d :", heap[i].page_id);

    if (isPageHit(heap[i].page_id)) {
      printf("page hit!");
    } else {
      // loop thru the
      for (j = 0; j < cap; j++) {
        x = p[j];
        found = false;

        for (k = i - 1; k >= 0; k--) {
          if (x == heap[k].page_id) {
            l[j] = k;
            found = true;
            break;
          } else
            found = false;
        }

        if (!found)
          l[j] = -1;
      }

      min = HEAP_CAPACITY / PAGE_SIZE;

      for (m = 0; m < cap; m++) {
        if (l[m] < min) {
          min = l[m];
          idx = m;
        }
      }

      // change the least recently used
      p[idx] = heap[i].page_id;

      // - increment page fault count
      page_fault_cnt++;

      showPages_cap();
    }
  }
}

/**
 * Allocate specified amount memory.
 * ASSUMPTION: Nobody will request more than 4080 KB (page size - metadata)
 *
 * @param   size    Amount of bytes to allocate.
 * @return  Pointer to the requested amount of memory.
 **/
page* pm_malloc(size_t size) {
  // printf("pm_malloc called with size: %zu\n", size);
  // check null size, too small, or too big (max allocable is 4096 - metadata)
  if (!size || size < 1 || size > PAGE_SIZE - sizeof(page)) {
    // printf(
    //     "Bad size! Either null, less than 1, or greater than max allocable
    //     "
    //     "(4080 bytes)\n");
    return NULL;
  }
  // check if we have enough space (in pages) in heap
  if (heap_pages_in_use + 1 > MAX_PAGES) {
    // printf("heap full\n");
    return NULL;
  }
  heap_pages_in_use++;

  // first fit algorithm
  for (int i = 0; i < MAX_PAGES; i++) {
    page* curr = &heap[i];
    if (curr->is_free) {  // <---- SEGFAULT (was) HERE ;)
      curr->is_free = false;
      curr->size = size + sizeof(page);
      curr->on_disk = false;
      curr->page_id = page_id;
      page_id++;
      // printf("Allocated page %d at address %p\n", curr->page_id,
      // (void*)curr);
      return curr;
    }
  }
  printf("No free pages in heap\n");
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
  heap_pages_in_use--;

  return;
}

/**
 * Initialize the heap.
 *
 */
void initialize_heap() {
  // page_table = initialize_page_table();

  // segment heap into pages
  for (int i = 0; i < MAX_PAGES; i++) {
    page* curr = &heap[i];
    curr->is_free = true;
    curr->size = 0;
    // curr->address = NULL;
    curr->on_disk = false;
    curr->page_id = i;
    // curr->data = NULL;
  }

  // heap will be an array of pages
  // track pages in primary memory by accessing heap
  // track pages in secondary memory by accessing swap file
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Heap initialized.\n");
  printf("Start of heap address: \t%p\n", (void*)&heap[0]);
  printf("End of heap address: \t%p\n", (void*)&heap[8388608]);
  printf("Max capacity: \t\t%d bytes\n", HEAP_CAPACITY);
  printf("Page size: \t\t%d bytes\n", PAGE_SIZE);
  printf("Max number of pages: \t%d pages\n", HEAP_CAPACITY / PAGE_SIZE);
  printf("Heap pages in use: %zu\n", heap_pages_in_use);
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("\n\n");
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
  int i = 0;
  double waste = 0;
  // page* curr = heap;
  //  size_t heap_pages_in_use = 0;

  while (i < MAX_PAGES) {
    waste += (PAGE_SIZE - heap[i].size);
    i++;
  }

  return waste / (HEAP_CAPACITY)*100;
}

/*
void move_to_disk() {
  // find page to move to disk
  // find first page in heap that is not free

  // move page to disk
  // update page table
  // update disk page list
  // update heap
  // update heap_pages_in_use
  // update page fault count

  printf("Paged out page %d: <%p> (size: %ld)\n", page_table[i]->page_id,
         (void*)page_table[i], page_table[i]->size);
  return;
}
*/

/**
 * Print out contents of heap
 */
void print_allocated_statistics() {
  int i = 0;
  int bytes = 0;
  page* curr = &heap[i];
  printf("Allocation Statistics:\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  while (curr && i < MAX_PAGES) {
    if (!curr->is_free) {
      printf("Page %d) <%p> \t(size: %ld)\n", curr->page_id, (void*)curr,
             curr->size);
      bytes += curr->size;
    }
    ++curr;
    ++i;
  }
  if (bytes == 0) {
    printf("No pages allocated.\n\n");
  }
  printf("\nTotal bytes allocated: \t%d\n", bytes);
  printf("Total allocated pages: \t%zu\n", heap_pages_in_use);
  printf("Wasted bytes: \t\t%lu\n", (heap_pages_in_use * PAGE_SIZE) - bytes);
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

/**
 * Special case: Heap is full, print out contents of heap
 */
void alloc_and_print_max_statistics() {
  printf("\n");
  int i = 0;
  int bytes = 0;
  page* curr = &heap[i];

  while (i < MAX_PAGES) {
    // generate a random number
    curr =
        pm_malloc(rand() % (UPPER_LIMIT_FOR_TEST - LOWER_LIMIT_FOR_TEST + 1) +
                  LOWER_LIMIT_FOR_TEST);
    bytes += curr->size;
    ++curr;
    ++i;
    if (i % 200 == 0) {
      printf("Bytes allocated so far: %d\n", bytes);
    }
  }

  printf("Allocation Statistics:\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Total bytes allocated: \t%d\n", bytes);
  printf("Total allocated pages: \t%zu\n", heap_pages_in_use);
  printf("Wasted bytes: \t\t%lu\n", (heap_pages_in_use * PAGE_SIZE) - bytes);
  printf("Internal fragmentation: %.4f%%\n", internal_fragmentation());
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

int main() {
  // initialize heap
  printf("Initializing heap...\n");
  initialize_heap();

  printf("Testing memory allocation...\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Calling pm_malloc(0) should return NULL (0x0): ");
  printf("%p\n\n", (void*)pm_malloc(0));

  printf("Allocating 10 pages of memory...\n");
  for (int i = 0; i < 10; i++) {
    page* block = pm_malloc(i * 69 + 420);
    printf("Allocated page %d: <%p> (size: %ld)\n", block->page_id,
           (void*)block, block->size);
  }
  printf("Heap pages in use: %zu\n\n", heap_pages_in_use);
  print_allocated_statistics();
  // printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

  printf("Allocating 10 more pages of memory...\n");
  for (int i = 0; i < 10; i++) {
    page* block = pm_malloc(i * 60 + 32);
    printf("Allocated page %d: <%p> (size: %ld)\n", block->page_id,
           (void*)block, block->size);
  }
  printf("Heap pages in use: %zu\n\n", heap_pages_in_use);
  print_allocated_statistics();

  printf("Freeing heap...\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  for (int i = 0; i < MAX_PAGES; i++) {
    if (heap[i].is_free) {
      continue;
    }
    pm_free(&heap[i]);
    printf("Freed page %d: <%p> (size: %ld)\n", heap[i].page_id,
           (void*)&heap[i], heap[i].size);
  }
  printf("Heap pages in use: %zu\n\n", heap_pages_in_use);
  print_allocated_statistics();

  printf("\nPaging out using page table...\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  // fill the heap to maximum capacity
  alloc_and_print_max_statistics();

  // move_to_disk();

  // Hashmap checks:
  pte* item1;
  pte* item2;
  pte* item3;

  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Inserting page-frame values in page table\n");
  insert_page_frame(heap[3].page_id, 20);
  insert_page_frame(heap[8].page_id, 71);
  insert_page_frame(heap[0].page_id, 31);
  insert_page_frame(heap[2].page_id, 22);
  insert_page_frame(heap[5].page_id, 5);
  insert_page_frame(heap[11].page_id, 34);
  insert_page_frame(heap[18].page_id, 4);
  insert_page_frame(heap[1].page_id, 29);

  // show_page_table();

  // check page table functions here:
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Checking values of page table\n");
  page_found_display(
      get_frame(heap[3].page_id));  // check frame value of page 24
  page_found_display(
      get_frame(heap[8].page_id));  // check frame value of page 29
  page_found_display(
      get_frame(heap[0].page_id));  // check frame value of page 21
  page_found_display(
      get_frame(heap[2].page_id));  // check frame value of page 23
  page_found_display(
      get_frame(heap[5].page_id));  // check frame value of page 26
  page_found_display(
      get_frame(heap[11].page_id));  // check frame value of page 32
  page_found_display(
      get_frame(heap[18].page_id));  // check frame value of page 39
  page_found_display(
      get_frame(heap[1].page_id));  // check frame value of page 22
  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Delete pages #29, #26, #39 in page table\n");
  item1 = get_frame(heap[8].page_id);
  item2 = get_frame(heap[5].page_id);
  item3 = get_frame(heap[18].page_id);
  delete_pf_pair(item1->page_id);  // delete page 29 page-frame pair
  delete_pf_pair(item2->page_id);  // delete page 26 page-frame pair
  delete_pf_pair(item3->page_id);  // delete page 39 page-frame pair

  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Re-checking values of page table\n");
  page_found_display(
      get_frame(heap[3].page_id));  // check frame value of page 24
  page_found_display(
      get_frame(heap[8].page_id));  // check frame value of page 29
  page_found_display(
      get_frame(heap[0].page_id));  // check frame value of page 21
  page_found_display(
      get_frame(heap[2].page_id));  // check frame value of page 23
  page_found_display(
      get_frame(heap[5].page_id));  // check frame value of page 26
  page_found_display(
      get_frame(heap[11].page_id));  // check frame value of page 32
  page_found_display(
      get_frame(heap[18].page_id));  // check frame value of page 39
  page_found_display(
      get_frame(heap[1].page_id));  // check frame value of page 22

  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("FIFO page replacement method");
  // FIFO method
  fifo();
  printf("\nFIFO - Total number of page faults: %d", page_fault_cnt);
  printf("\n");

  printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("LRU page replacement method");
  // LRU method
  lru();
  printf("\nLRU: Total number of page faults: %d", page_fault_cnt);
  printf("\n");

  return 0;
}
