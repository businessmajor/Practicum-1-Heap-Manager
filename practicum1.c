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

#include "page_table.c"

page_table_t* hash_arr[MAX_PAGES];  // create hash_arr with defined size
page_table_t* dummy_item;
int page_fault_cnt;           // page fault count
int cap = 5;                  // sample storage capacity value = 5
int p[100];                   // page sequence
int pg_seq_size_fifo = 15;    // sample page sequence size = 15
int pg_seq_size_lru = 15;     // sample page sequence size = 15
int pg_in_storage_fifo[100];  // fifo pages that are in storage
int pg_in_storage_lru[100];   // lru pages that are in storage
void fifo();
void lru();
void move_to_disk();

/******************************
 *******GLOBAL VARIABLES*******
 ******************************/
// Pre-allocate 8MB "heap" memory from the static store with room for metadata
page heap[MAX_PAGES];
size_t heap_pages_in_use = 0;  // keep track of how many PAGES are allocated in
                               // heap (1 page = 4096 KB allocated)
page disk_backing_store[MAX_PAGES];
page_table_t*
    page_table;   // keep track of pages in primary and secondary memory
int page_id = 0;  // unique page id for each page in heap (starts at
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
  // printf("pm_malloc called with size: %zu\n", size);
  // check null size, too small, or too big (max allocable is 4096 - metadata)
  if (!size || size < 1 || size > PAGE_SIZE - sizeof(page)) {
    // printf(
    //     "Bad size! Either null, less than 1, or greater than max allocable "
    //     "(4080 bytes)\n");
    return NULL;
  }
  // check if we have enough space (in pages) in heap
  // page to disk swap
  if (heap_pages_in_use + 1 > MAX_PAGES) {
    printf("No more space in heap! Swapping pages to disk...\n");
    // swap pages to disk
    move_to_disk();
  }
  heap_pages_in_use++;

  // find first free page in heap
  // iterate through heap until we find first free page
  // if we find a free page, allocate it and return it
  // if we don't find a free page, return NULL
  // first fit algorithm
  for (int i = 0; i < MAX_PAGES; i++) {
    page* curr = &heap[i];
    if (curr->is_free) {  // <---- SEGFAULT (was) HERE ;)
      curr->is_free = false;
      curr->size = size;
      curr->on_disk = false;
      if (!curr->page_id) {
        curr->page_id = page_id;
        page_id++;
      }
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
  // initialize page table
  page_table = initialize_page_table();
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
  // initialize page table
  // initialize_page_table();
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Heap initialized.\n");
  // printf("Address: %p\n", (void*)heap);
  printf("Start of heap address: \t%p\n", (void*)&heap[0]);
  printf("End of heap address: \t%p\n", (void*)&heap[8388608]);
  printf("Max capacity: \t\t%d bytes\n", HEAP_CAPACITY);
  printf("Page size: \t\t%d bytes\n", PAGE_SIZE);
  printf("Max number of pages: \t%d pages\n", HEAP_CAPACITY / PAGE_SIZE);
  printf("Heap pages in use: %zu\n", heap_pages_in_use);
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
 */
double internal_fragmentation() {
  int i = 0;
  double fragmentation = 0.0;
  page* curr = heap;
  // size_t heap_pages_in_use = 0;

  while (curr && i < MAX_PAGES) {
    fragmentation += curr->size;
    // heap_pages_in_use++;
    curr++;
    i++;
  }
  fragmentation = fragmentation / heap_pages_in_use * 100.0;
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
  int i = 0;
  double fragmentation = 0.0;
  page* curr = &heap[i];
  size_t largest_free_block = 0;
  size_t all_free_memory = 0;

  while (curr && i < MAX_PAGES) {
    if (curr->size > largest_free_block) {
      largest_free_block = curr->size;
    }
    all_free_memory += curr->size;
    ++curr;
    i++;
  }

  fragmentation = (1 - (largest_free_block / all_free_memory)) * 100.0;
  return fragmentation;
}

/**
 * Choose which algorithm to use to move page to disk.
 *
 * @param choice
 */
void choose_algorithm(int choice) {
  switch (choice) {
    case 1:
      fifo();
      break;
    case 2:
      lru();
      break;
    default:
      printf("Invalid choice. Please try again.\n");
      move_to_disk();
      break;
  }
  return;
}

void move_to_disk() {
  printf("Choose which algorithm to use to move page to disk:\n");
  printf("1) FIFO\n");
  printf("2) LRU\n");
  int choice;
  scanf("%d", &choice);
  choose_algorithm(choice);
}

/**
 * Print out contents of heap
 */
void print_allocated_statistics() {
  int i = 0;
  int bytes = 0;
  page* curr = &heap[i];
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Allocated heap pages:\n");
  while (curr && i < MAX_PAGES) {
    if (!curr->is_free) {
      printf("Page %d) <%p> \t(size: %ld)\n", curr->page_id, (void*)curr,
             curr->size);
      bytes += curr->size;
    }
    ++curr;
    ++i;
  }
  printf("\nTotal bytes allocated: \t%d\n", bytes);
  printf("Total allocated pages: \t%zu\n", heap_pages_in_use);
  printf("Wasted bytes: \t\t%lu\n", (heap_pages_in_use * PAGE_SIZE) - bytes);
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

int main() {
  /*

  // Hashmap checks:
  page_table_t* item1;
  page_table_t* item2;
  page_table_t* item3;

  item1->page_id = 1;
  item1->frame = 2;
  item2->page_id = 2;
  item2->frame = 3;
  item3->page_id = 3;
  item3->frame = 4;

  insert_page(item1->page_id, item1->frame);
  insert_page(42, 71);
  insert_page(2, 31);
  insert_page(13, 22);
  insert_page(37, 5);
  // insert_page(2, 25);
  insert_page(29, 34);
  insert_page(32, 4);

  // show_page_table();

  // check page table functions here:
  page_found_display(get_frame(37));  // check frame value of page 37
  page_found_display(get_frame(2));   // check frame value of page 2
  page_found_display(get_frame(32));  // check for frame value of page 32

  item7 = get_frame(32);
  delete_pf_pair(item7);              // delete page 32 page-frame pair
  page_found_display(get_frame(32));  // check for frame value of page 32

  // free heap memory when done with page_table
  item1 = get_frame(1);
  item2 = get_frame(42);
  item3 = get_frame(2);
  item4 = get_frame(13);
  item5 = get_frame(37);
  item6 = get_frame(29);
  delete_pf_pair(item1);
  delete_pf_pair(item2);
  delete_pf_pair(item3);
  delete_pf_pair(item4);
  delete_pf_pair(item5);
  // delete_pf_pair(get_frame(2));
  delete_pf_pair(item6);

  page_found_display(get_frame(37));  // check for frame value of page 32

  // FIFO checks:
  // input pages data
  pg_in_storage_fifo[0] = 3;
  pg_in_storage_fifo[1] = 8;
  pg_in_storage_fifo[2] = 2;
  pg_in_storage_fifo[3] = 3;
  pg_in_storage_fifo[4] = 9;
  pg_in_storage_fifo[5] = 1;
  pg_in_storage_fifo[6] = 6;
  pg_in_storage_fifo[7] = 3;
  pg_in_storage_fifo[8] = 8;
  pg_in_storage_fifo[9] = 9;
  pg_in_storage_fifo[10] = 3;
  pg_in_storage_fifo[11] = 6;
  pg_in_storage_fifo[12] = 2;
  pg_in_storage_fifo[13] = 1;
  pg_in_storage_fifo[14] = 3;

  // FIFO method
  fifo();
  printf("\nTotal number of page faults: %d", page_fault_cnt);
  printf("\n");

  // LRU checks:
  pg_in_storage_lru[0] = 3;
  pg_in_storage_lru[1] = 8;
  pg_in_storage_lru[2] = 2;
  pg_in_storage_lru[3] = 3;
  pg_in_storage_lru[4] = 9;
  pg_in_storage_lru[5] = 1;
  pg_in_storage_lru[6] = 6;
  pg_in_storage_lru[7] = 3;
  pg_in_storage_lru[8] = 8;
  pg_in_storage_lru[9] = 9;
  pg_in_storage_lru[10] = 3;
  pg_in_storage_lru[11] = 6;
  pg_in_storage_lru[12] = 2;
  pg_in_storage_lru[13] = 1;
  pg_in_storage_lru[14] = 3;

  // LRU method
  lru();
  printf("\nLRU: Total number of page faults: %d", page_fault_cnt);
  printf("\n");

  return 0;
*/
  ////////////////////////////////////////////////

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

  printf("\nFragmentation statistics:\n");
  printf("Internal fragmentation: %f%%\n", internal_fragmentation());
  printf("External fragmentation: %f%%\n", external_fragmentation());

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
page disk_page_list[HEAP_CAPACITY];  // array implementation
page page_table[HEAP_CAPACITY];      // hash map implementation
*/
