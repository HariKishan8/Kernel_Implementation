/*
    File: vm_pool.H

    Author: Hari Kishan Srikonda
	    Department of Electrical and Computer Engineering
            Texas A&M University
    Date  : 10/21/14

    Description: Management of the Virtual Memory Pool

*/

#include "vm_pool.H"
#include "console.H"
#include "page_table.H"

//const unsigned int PageTable::PAGE_SIZE ; /* in bytes */
//const unsigned int PageTable::ENTRIES_PER_PAGE ; /* in entries, duh! */

/* Initializes the data structures needed for the management of this
    * virtual-memory pool.
    * _base_address is the logical start address of the pool.
    * _size is the size of the pool in bytes.
    * _frame_pool points to the frame pool that provides the virtual
    * memory pool with physical memory frames.
    * _page_table points to the page table that maps the logical memory
    * references to physical addresses. */
VMPool::VMPool(unsigned long _base_address, unsigned long _size, FramePool *_frame_pool,        PageTable *_page_table){ 
  frame_pool = _frame_pool;
  page_table = _page_table;
  base_address = _base_address;
  size = _size;
  count=0;
  VM_address = NULL;
  VM_size = NULL;
  page_table->register_vmpool(this);
}   

/* Allocates a region of _size bytes of memory from the virtual
    * memory pool. If successful, returns the virtual address of the
    * start of the allocated region of memory. If fails, returns 0. */
unsigned long VMPool::allocate(unsigned long _size){
  page_table->register_vmpool(this);
  if(VM_address==NULL && VM_size==NULL){
	VM_address = (unsigned long *) ((frame_pool->get_frame())*(page_table->PAGE_SIZE));
  	VM_size = (unsigned long *) ((frame_pool->get_frame())*(page_table->PAGE_SIZE));
  }
  if (_size > size)
	return 0;
  VM_size[count] = _size;
  VM_address[count] = base_address;
  for(unsigned int i=0;i<count;i++){
	VM_address[count] += VM_size[i];
  }
  size -= _size;
  count++;
  return VM_address[count-1];
}
  
/* Releases a region of previously allocated memory. The region
    * is identified by its start address, which was returned when the
    * region was allocated. */   
void VMPool::release(unsigned long _start_address){
  unsigned int i,j;
  for(i=0;i<count;i++){
	if(VM_address[i] == _start_address){
		size+= VM_size[i];
		j = VM_size[i]/(page_table->PAGE_SIZE);
		VM_address[i] = NULL;
		while(j<0){
			// freeing the page				
			page_table->free_page(((unsigned long)(&VM_address[i]))+page_table->ENTRIES_PER_PAGE+j);		
			j--;
		}
		break;
	}
  }
}

/* Returns FALSE if the address is not valid. An address is not valid
    * if it is not part of a region that is currently allocated. */  
BOOLEAN VMPool::is_legitimate(unsigned long _address){
  if((_address == ((unsigned long)VM_address)) || (((unsigned long)VM_size) == _address))
	return true;
  // checking if addresss lies within the allocated space base_address to base_address + size
  if((_address >= base_address) && (_address < base_address+size)){	
	return true;
  }
  return false;
}





 
