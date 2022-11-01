# Practicum-Heap-Manager

<h2> Motivation </h2>
<p>Virtual memory provides memory for processes above the limit of physical memory in a computer by swapping unused pages to a secondary storage device, such as a disk (mechanical or solid-state). In this practicum, we have implemented a simple virtual memory system with a page replacement algorithm.</p>

<h2> Summary </h2>
<p>Together, we have settled on a revised, combined design from Assignment A6.1 and will implement it in C.<br><br>

  1. We are preallocating our heap memory from the static store using <i>char phys_mem[N]</i> to allocate 8 megabytes for our heap.</p>

<h2> What we need </h2>
<p>We need to create the following:<br><br>

  1. Functions<br>
  -pm_malloc()<br>
  -pm_free()<br>
  -
  2. Data structures<br>
  -queue to keep track of page order<br>
  -memory block<br>
  -heap<br>
  -page table<br>
</p>

<h2>Assumptions and Notes</h2>
<p>1. We assume the maximum amount of memory that can be allocated at a time is equal to a single page (4096 bytes.)<br>
   2. We will not be taking thread safety or concurrency into account.<br>
   3. We will not be concerned with the degree of internal fragmentation with the disk files and file system.</p>
