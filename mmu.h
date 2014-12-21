/*
 * mmu.h
 *
 *  Created on: 9 déc. 2014
 *      Author: warnant
 */

#ifndef MMU_H_
#define MMU_H_

#include <stdio.h>

/* MMU register commands */
#define MMU_IRQ		13
#define MMU_CMD		0x66
#define MMU_FAULT_ADDR 	0xCD
#define TLB_ADD_ENTRY	0xCE
#define TLB_DEL_ENTRY	0xDE
#define TLB_SIZE	32
#define TLB_ENTRIES	0x800

/* Memory (virtual and physical) constants */
#define PM_PAGES (1 << 8)
#define VM_PAGES (1 << 12)
#define PAGE_SIZE 4096
#define PM_SIZE (4096 * PM_PAGES)
#define VM_SIZE (4096 * VM_PAGES)

/* Define this DEBUG constant to print all data */
#define DEBUG

/* Define the structure of the basic virtual memory mapping */
struct vm_mapping_s {
	unsigned vm_mapped :1;
	unsigned vm_ppage :8;
};

/* Define the structure of the basic virtual memory mapping */
struct pm_mapping_s {
	unsigned pm_mapped :1;
	unsigned pm_vpage :12;
};

/* Define the structure of one TLB entry */
/* Noctice : RFU is an unused Field */
struct tlb_entry_s {
	unsigned tlbe_rfu :8;
	unsigned tlbe_vpage :12;
	unsigned tlbe_ppage :8;
	unsigned tlbe_access :3;
	unsigned tlbe_active :1;
};


char* HW_CONFIG;

/* Define the current virtual and physical page numbers */
static unsigned current_vpage = 0;
static unsigned current_ppage = 0;

/* Define the current round robin count */
static unsigned rr = 0;

/* Define the virtual memory mapping table */
static struct vm_mapping_s vm_mapping[VM_PAGES];

/* Define the physical memory mapping table */
static struct pm_mapping_s pm_mapping[PM_PAGES];


/*
* init_master()
 * This method init the hardware library and the memory management unit
*/
void init_master();

/*
* mmuhandler()
* This method is invoked when a MMU fault is thrown in the main program.
* mmuhandler handles virtual and physical memory
*/
void mmuhandler();

#endif /* MMU_H_ */
