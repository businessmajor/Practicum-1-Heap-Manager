#include "page_table.h"

page_table_t* hash_arr[MAX_PAGES];  // create hash_arr with defined size
page_table_t* dummyItem;
int page_fault_cnt;      // page fault count
int cap = 4;             // sample storage capacity value = 4
int p[100];              // page sequence
int pg_seq_size = 8;     // sample page sequence size = 8
int pg_in_storage[100];  // pages that are in storage

/**
 * Hash function for page table
 *
 * @param page page key
 * @return int
 */
int hash_code(int page) {
  return page % HEAP_CAPACITY;
}

/**
 * Search for the frame number with a given page key
 *
 * @param page page key
 * @return page_table_t*
 */
page_table_t* get_frame(int page) {
  // get the hash code
  int hash_idx = hash_code(page);

  // move in array that is empty
  while (hash_arr[hash_idx] != NULL) {
    if (hash_arr[hash_idx]->page_id == page)
      return hash_arr[hash_idx];

    // next page
    ++hash_idx;

    // return to start of table
    hash_idx %= HEAP_CAPACITY;
  }

  return NULL;
}

/**
 * Insert page and frame number in page_table_t
 *
 * @param page
 * @param frame
 */
void insert_page(page* page, int frame) {
  // allocate heap memory for each item in page_table_t
  page_table_t* item = (page_table_t*)malloc(sizeof(page_table_t));

  // assign page & frame values
  item->page_id = page;
  item->frame = frame;

  // get the hash code
  int hash_idx = hash_code(page);

  // look for an empty or remove page line
  while (hash_arr[hash_idx] != NULL && hash_arr[hash_idx]->page_id != -1) {
    // next page
    ++hash_idx;

    // return to start of table
    hash_idx %= HEAP_CAPACITY;
  }

  hash_arr[hash_idx] = item;
}

/**
 * Delete page and frame line in page_table_t
 *
 * @param item
 * @return page_table_t*
 */
page_table_t* delete_pf_pair(page_table_t* item) {
  // get the page value
  int page = item->page_id;

  // get hash code
  int hash_idx = hash_code(page);

  // check in the array until empty
  while (hash_arr[hash_idx] != NULL) {
    if (hash_arr[hash_idx]->page_id == page) {
      page_table_t* temp = hash_arr[hash_idx];

      // assign a dummy item at removed position
      hash_arr[hash_idx] = dummyItem;

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
 * @brief Print contents of page_table_t
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
void page_found_display(page_table_t* item) {
  if (item != NULL)
    printf("Page: %d | Frame: %d\n", item->page_id, item->frame);
  else
    printf("Frame not found!\n");
}

// code implementation for FIFO page replacement
int page_fault_cnt;      // page fault count
int cap = 4;             // sample storage capacity value = 4
int p[100];              // page sequence
int pg_seq_size = 8;     // sample page sequence size = 8
int pg_in_storage[100];  // pages that are in storage

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

/**
 * FIFO Page replacement algorithm
 *
 */
void fifo() {
  // initialize capacity/storage
  initialize_cap();
  int i, j;

  // traverse the pages
  for (i = 0; i < pg_seq_size; i++) {
    printf("\nFor %d :", pg_in_storage[i]);

    if (isPageHit(pg_in_storage[i]) == false) {
      // 	- page hit: 1. if memory cell is empty; 2. page is not in
      // storage
      for (j = 0; j < cap - 1; j++)
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

/**
int main() {
  // allocate memory until our heap is full
  // call pm_malloc
  // if pm_malloc sees we are full, it will call move_to_disk
  // print heap list, move to disk, print heap again
  // demonstrate pm_malloc works when we have enough memory in heap and
  // demonstrate we can offload pages to "disk" when out of space

// Hashmap checks:
page_table_t* item1;
page_table_t* item2;
page_table_t* item3;
page_table_t* item4;
page_table_t* item5;
page_table_t* item6;
page_table_t* item7;

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
//	delete_pf_pair(get_frame(2));
delete_pf_pair(item6);

page_found_display(get_frame(37));  // check for frame value of page 32

// FIFO checks:
// input pages data
pg_in_storage[0] = 4;
pg_in_storage[1] = 13;
pg_in_storage[2] = 25;
pg_in_storage[3] = 31;
pg_in_storage[4] = 7;
pg_in_storage[5] = 25;
pg_in_storage[6] = 21;
pg_in_storage[7] = 31;

// FIFO method
fifo();
printf("\nTotal number of page faults: %d", page_fault_cnt);
printf("\n");

return 0;
}
*/