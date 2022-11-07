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
  void* start;   // pointer to page start address
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
 * */
void initialize_heap() {
  // Pre-allocate 8MB "heap" memory from the static store
  heap[HEAP_CAPACITY];
  // Allocate all virtual memory structures within the block as well.
  // allocate memory for disk page list
  disk_page_list = (page_list*)pm_malloc(sizeof(page_list));

  // initialize primary and disk page list
  // primary_memory_page_list->count = 0;
  // disk_page_list->count = 0;
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





// Codes for PAGE_TABLE here:
typedef struct page_table {
        int page;
        int frame;
} page_table;

// create hashArray with defined size
page_table *hashArray[HEAP_CAPACITY];
page_table *dummyItem;

// simple hash function that will place the key
int hashCode(int page) {
        return page % HEAP_CAPACITY;
}

// search for the frame number given page key
page_table *get_frame(int page) {

        // get the hash code
        int hashIndex = hashCode(page);

        // move in array that is empty
        while (hashArray[hashIndex] != NULL) {

                if (hashArray[hashIndex]->page == page)
                        return hashArray[hashIndex];

                // next page
                ++hashIndex;

                // return to start of table
                hashIndex %= HEAP_CAPACITY;
        }

        return NULL;
}

// insert page and frame number in page_table
void insert_page(int page, int frame) {

        // allocate heap memory for each item in page_table
        page_table *item = (page_table*)malloc(sizeof(page_table));

        // assign page & frame values
        item->page = page;
        item->frame = frame;

        // get the hash code
        int hashIndex = hashCode(page);

        // look for an empty or remove page line
        while (hashArray[hashIndex] != NULL && hashArray[hashIndex]->page != -1) {

                // next page
                ++hashIndex;

                // return to start of table
                hashIndex %= HEAP_CAPACITY;
        }

        hashArray[hashIndex] = item;

}

// delete page & frame line
page_table *delete_pf_pair(page_table *item) {

        // get the page value
        int page = item->page;

        // get hash code
        int hashIndex = hashCode(page);

        // check in the array until empty
        while (hashArray[hashIndex] != NULL) {

                if (hashArray[hashIndex]->page == page) {
                        page_table *temp = hashArray[hashIndex];

                        // assign a dummy item at removed position
                        hashArray[hashIndex] = dummyItem;

			// free heap memory - cant free because this is page table
			// free(item);
                        return temp;
                }

                // check next cell position
                ++hashIndex;

                // return to start of table
                hashIndex %= HEAP_CAPACITY;
        }

        return NULL;
}

// show items in page_table
void show_page_table() {

        int i = 0;

        for (i = 0; i < HEAP_CAPACITY; i++) {

                if (hashArray[i] != NULL)
                        printf(" (%d,%d)", hashArray[i]->page, hashArray[i]->frame);
                else
                        printf(" ~~ ");
        }

        printf("\n");
}

// checks if item is found in page table
void page_found_display(page_table *item) {

	if (item != NULL)
                printf("Page: %d | Frame: %d\n", item->page, item->frame);
        else
                printf("Frame not found!\n");
}

// code implementation for FIFO page replacement
int page_fault_cnt;	// page fault count
int cap = 4; 		// sample storage capacity value = 4
int p[100];		// page sequence
int pg_seq_size = 8;    // sample page sequence size = 8
int pg_in_storage[100];	// pages that are in storage


// helper method - initialize the capacity of page storage to value -1
void initialize_cap() {
	page_fault_cnt = 0;
	int i;
	for (i = 0; i < cap; i++) 
		p[i] = -1;
}

// helper method - check for page hits true/false
bool isPageHit(int data) {

	bool pageHit = false;
	int i;

	// iterage over the capacity
	for (i = 0; i < cap; i++) {
		if (p[i] == data) {
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

// FIFO page replacement
void fifo() {

	// initialize capacity/storage
	initialize_cap();
	int i, j;

	// traverse the pages
	for (i = 0; i < pg_seq_size; i++) {

		printf("\nFor %d :", pg_in_storage[i]);
		
		if (isPageHit(pg_in_storage[i]) == false) {

			// 	- page hit: 1. if memory cell is empty; 2. page is not in storage
			for (j = 0; j < cap-1; j++)
				p[j] = p[j + 1];
			
			p[j] = pg_in_storage[i];

			// 		- increment page fault count
			page_fault_cnt++;
			showPages_cap();
		} else {
			// 	- page miss: page is in storage 
			printf("page miss!");
		}
	}
}

int main() {
  // allocate memory until our heap is full
  // call pm_malloc
  // if pm_malloc sees we are full, it will call move_to_disk
  // print heap list, move to disk, print heap again
  // demonstrate pm_malloc works when we have enough memory in heap and
  // demonstrate we can offload pages to "disk" when out of space

  // Hashmap checks:
       	page_table *item1;
	page_table *item2;
	page_table *item3;
	page_table *item4;
	page_table *item5;
	page_table *item6;
	page_table *item7;

        insert_page(1, 20);
        insert_page(42, 71);
        insert_page(2, 31);
        insert_page(13, 22);
        insert_page(37, 5);
//        insert_page(2, 25);
        insert_page(29, 34);
	insert_page(32, 4);

	// show_page_table();

	// check page table functions here:
	page_found_display(get_frame(37));	// check frame value of page 37
	page_found_display(get_frame(2));	// check frame value of page 2
	page_found_display(get_frame(32));	// check for frame value of page 32

	item7 = get_frame(32);
	delete_pf_pair(item7);			// delete page 32 page-frame pair
	page_found_display(get_frame(32));	// check for frame value of page 32


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
//	delete_pf_pair(get_frame(2));
	delete_pf_pair(item6);

	page_found_display(get_frame(37));	// check for frame value of page 32

	// FIFO checks:
	// input pages data
	pg_in_storage[0] = 4; pg_in_storage[1] = 13; pg_in_storage[2] = 25; pg_in_storage[3] = 31;
	pg_in_storage[4] = 7; pg_in_storage[5] = 25; pg_in_storage[6] = 21; pg_in_storage[7] = 31;

	// FIFO method
	fifo();
	printf("\nTotal number of page faults: %d", page_fault_cnt);  
	printf("\n");

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
