
#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "scheduler.H"
#include "thread.H"
/* Extern definition of the pointer to the Scheduler Class to call the functions */
extern Scheduler * SYSTEM_SCHEDULER;

/* ISSUE OPERATION of blocking disk */

void BlockingDisk::issue_operation(DISK_OPERATION _op, unsigned long _block_no) {

  outportb(0x1F1, 0x00); /* send NULL to port 0x1F1         */
  outportb(0x1F2, 0x01); /* send sector count to port 0X1F2 */
  outportb(0x1F3, (unsigned char)_block_no); 
                         /* send low 8 bits of block number */
  outportb(0x1F4, (unsigned char)(_block_no >> 8)); 
                         /* send next 8 bits of block number */
  outportb(0x1F5, (unsigned char)(_block_no >> 16)); 
                         /* send next 8 bits of block number */
  outportb(0x1F6, ((unsigned char)(_block_no >> 24)&0x0F) | 0xE0 | (disk_id << 4));
                         /* send drive indicator, some bits, 
                            highest 4 bits of block no */

  outportb(0x1F7, (_op == READ) ? 0x20 : 0x30);

}


/* DISK OPERATIONS */

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
/* Reads 512 Bytes in the given block of the given disk drive and copies them 
   to the given buffer. No error check! */
  /* Populate the queue elements */
  bq[i1]->_op = READ;
  bq[i1]->_block_no = _block_no;
  bq[i1]->_buf = _buf;
  
  i1++;

  issue_operation(READ, _block_no);
  
  /* Pass on the CPU to the next thread */
  SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
  SYSTEM_SCHEDULER->yield();
}

void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
/* Writes 512 Bytes from the buffer to the given block on the given disk drive. */
  /* Populate the queue elements */
  bq[i1]->_op = WRITE;
  bq[i1]->_block_no = _block_no;
  bq[i1]->_buf = _buf;
  
  i1++;

  issue_operation(WRITE, _block_no);

  /* Pass on the CPU to the next thread */
  SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
  SYSTEM_SCHEDULER->yield();
}

/*bool BlockingDisk::is_ready() {
  return (inportb(0x1F7) & 0x08);
} */ 

/*void BlockingDisk::operate() {

  int j;
  unsigned short tmpw;

  if(bq[0]->_op == READ) {
	  // read data from port 
	  for (j = 0; j < 256; j++) {
	    tmpw = inportw(0x1F0);
	    bq[0]->_buf[j*2]   = (unsigned char)tmpw;
	    bq[0]->_buf[j*2+1] = (unsigned char)(tmpw >> 8);
	  }
  }
  else { 
	  // write data to port 
	  for (j = 0; j < 256; j++) {
	    tmpw = bq[0]->_buf[2*j] | (bq[0]->_buf[2*j+1] << 8);
	    outportw(0x1F0, tmpw);
	  }
  }
  for(j=0;j<4;j++) {
	*bq[j] = *bq[j+1];
  }
  i1--;
}*/



