
/*
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
        pg_in_storage_fifo[0] = 3; pg_in_storage_fifo[1] = 8;
pg_in_storage_fifo[2] = 2; pg_in_storage_fifo[3] = 3; pg_in_storage_fifo[4] = 9;
pg_in_storage_fifo[5] = 1; pg_in_storage_fifo[6] = 6; pg_in_storage_fifo[7] = 3;
        pg_in_storage_fifo[8] = 8; pg_in_storage_fifo[9] = 9;
pg_in_storage_fifo[10] = 3; pg_in_storage_fifo[11] = 6; pg_in_storage_fifo[12] =
2; pg_in_storage_fifo[13] = 1; pg_in_storage_fifo[14] = 3;

        // FIFO method
        fifo();
        printf("\nTotal number of page faults: %d", page_fault_cnt);
        printf("\n");

                // LRU checks:
        pg_in_storage_lru[0] = 3; pg_in_storage_lru[1] = 8; pg_in_storage_lru[2]
= 2; pg_in_storage_lru[3] = 3; pg_in_storage_lru[4] = 9; pg_in_storage_lru[5] =
1; pg_in_storage_lru[6] = 6; pg_in_storage_lru[7] = 3; pg_in_storage_lru[8] = 8;
pg_in_storage_lru[9] = 9; pg_in_storage_lru[10] = 3; pg_in_storage_lru[11] = 6;
        pg_in_storage_lru[12] = 2; pg_in_storage_lru[13] = 1;
pg_in_storage_lru[14] = 3;

        // LRU method
        lru();
        printf("\nLRU: Total number of page faults: %d", page_fault_cnt);
        printf("\n");

return 0;
}
*/
/*
TEST for difference function:
pm_malloc:
- test for size=0 value
- test for values of is_free, size, start and on_disk. change these parameters
values accordingly
- test assumption of head_size + 1 <= HEAP_CAPACITY / PAGE_SIZE
- test for non-presence of page --> presence of page

pm_free:
- test is_free value changes

page moved to disk and heap
- test the sequence of pages i.e. 5 --> 8 --> 6
- test for presence or non-presence of pages
- test specific page value from disk to heap vice-versa

*/
