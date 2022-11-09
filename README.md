# Practicum-1-Heap-Manager

<h2> Motivation </h2>
<p>Virtual memory provides memory for processes above the limit of physical memory in a computer by swapping unused pages to a secondary storage device, such as a disk (mechanical or solid-state). In this practicum, we have implemented a simple virtual memory system with a page replacement algorithm.</p>

<h2> Summary </h2>
<p>We are preallocating our heap memory from the static store using <i>page heap[HEAP_CAPACITY]</i> to allocate 8 megabytes for our heap.<br>
   Pages can be allocated and freed from this heap. There is functionality for a FIFO page replacement algorithm.</p>

<h2> How to compile </h2>
<p>To compile, enter <i>make practicum1</i> into the command line.<br>
   To run, enter <i>./practicum1</i> into the command line.<br>
</p>

<h2>Assumptions and Notes</h2>
<p>1. We assume the maximum amount of memory that can be allocated at a time is equal to a single page less metadata (4096 - 16 bytes.)<br>
   2. We will not be taking thread safety or concurrency into account.<br>
   3. We will not be concerned with the degree of internal fragmentation with the disk files and file system.<br>
   4. The heap is instantiated as an 8 MB static array of pages.<br>
   &ensp;&ensp;&ensp;&nbsp; a. A page is a struct that contains metadata (page_id, size, is_free, on_disk.)<br>
   &ensp;&ensp;&ensp;&nbsp; b. We define a page as being 4096 bytes, including 16 bytes of metadata, for 4080 effective bytes.<br>
   </p>


<h2>References</h2>
<p>1. https://www.programming9.com/programs/c-programs/285-page-replacement-programs-in-c<br>
   2. https://www.edn.com/design/systems-design/4333346/Handling-memory-fragmentation<br>
   3. https://gee.cs.oswego.edu/dl/html/malloc.html<br>
   4. https://medium.com/@andrestc/implementing-malloc-and-free-ba7e7704a473<br>
   5. https://www.tutorialspoint.com/data_structures_algorithms/hash_table_program_in_c.htm</p>
