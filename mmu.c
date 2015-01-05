/*
 * mmu.c

 *
 *  Created on: 9 déc. 2014
 *      Author: warnant
 */
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "hardware.h"
#include "mmu.h"
#include "swap.h"

static void init_hw_lib() {
    HW_CONFIG = getenv("HW_CONFIG");
    if (HW_CONFIG == NULL) {
        HW_CONFIG = DFT_HW_CONFIG;
        fprintf(stderr,
                "Warning : The env variable HW_CONFIG is not set on your system. The default value %s has been chosen for the current execution.\n",
                HW_CONFIG);
    }
}

void init_master() {
    init_hw_lib();

    if (init_hardware(HW_CONFIG) == 0) {
        fprintf(stderr, "Error for hardware initialization\n");
        exit(EXIT_FAILURE);
    }

    if(init_swap(NULL) == -1) {
        fprintf(stderr, "Error for swap memory initialization\n");
        exit(EXIT_FAILURE);
    }

    unsigned i;
    for (i = 0; i < PM_PAGES; ++i)
        pm_mapping[i].pm_vpage = i;
    for (i = 0; i < VM_PAGES; ++i)
        vm_mapping[i].vm_ppage = i % (PM_PAGES - 1);

    IRQVECTOR[MMU_IRQ] = mmuhandler;


}

void mmuhandler() {
    unsigned vaddr;
    struct tlb_entry_s tlbe;

    /* Get the address which has caused the fault */
    vaddr = _in(MMU_FAULT_ADDR);


    /* Virtual address control : validity */
    if (vaddr < (int) virtual_memory || vaddr > (((int) virtual_memory) + VM_SIZE - 1)) {
        fprintf(stderr, "Super segmentation fault\n");
        exit(EXIT_FAILURE);
    }

    /* Get the current virtual page number */
    current_vpage = (vaddr >> 12) & 0xFFF;

    #ifdef DEBUG
    printf("current_vpage[vaddr]=%d\n", current_vpage);
    #endif


#ifdef DEBUG
    printf("Is virtual page mapped=%d\n", vm_mapping[current_vpage].vm_mapped);
#endif

    /* Check if there is a current association vpage - ppage */
    if (vm_mapping[current_vpage].vm_mapped) {

        /* Delete the old entries */
        tlbe.tlbe_ppage = current_ppage; /* Destroy all pages with physical page number set to 1 */
        _out(TLB_DEL_ENTRY, *(int *) (&tlbe));

#ifdef DEBUG
        printf("current_ppage=%d\n", current_ppage);
        printf("vm_mapping[current_vpage].vm_ppage=%d", vm_mapping[current_vpage].vm_ppage);
#endif

        tlbe.tlbe_vpage = current_vpage;
        tlbe.tlbe_ppage = vm_mapping[current_vpage].vm_ppage;
        tlbe.tlbe_access = 0x7;
        tlbe.tlbe_active = 0x1;

        _out(TLB_ADD_ENTRY, *(int *) (&tlbe));
    } else {

#ifdef DEBUG
        printf("Next physical page : %d\n", current_ppage);
#endif
        /* If there is no association, we take a new physical page with the round robin algorithm */
        current_ppage = (rr++ % (PM_PAGES - 1)) + 1;


        if (store_to_swap(pm_mapping[current_ppage].pm_vpage, current_ppage) == -1) {
            fprintf(stderr, "Store to swap fault occured when the round robin algorithm was executed.\n");
            exit(EXIT_FAILURE);
        }

        vm_mapping[pm_mapping[current_ppage].pm_vpage].vm_mapped = 0;

        if (fetch_from_swap(current_vpage, current_ppage) == -1) {
            fprintf(stderr, "Fetch from swap fault occured when the round robin algorithm was executed.\n");
            exit(EXIT_FAILURE);
        }

        /* Create a new association in the memory arrays */
        vm_mapping[current_vpage].vm_mapped = 1;
        vm_mapping[current_vpage].vm_ppage = current_ppage;

        pm_mapping[current_ppage].pm_mapped = 1;
        pm_mapping[current_ppage].pm_vpage = current_vpage;

#ifdef DEBUG
        printf("pm_mapping[current_ppage]=%d,vm_mapping[current_vpage]=%d\n", vm_mapping[current_vpage].vm_ppage, pm_mapping[current_ppage].pm_vpage);
#endif

        /* Delete the old entries */
        tlbe.tlbe_ppage = current_ppage; /* Destroy all pages with physical page number set to 1 */
        _out(TLB_DEL_ENTRY, *(int *) (&tlbe));

        /* Add a new TLB entry */
        tlbe.tlbe_vpage = current_vpage;
        tlbe.tlbe_ppage = current_ppage;
        tlbe.tlbe_access = 0x7;
        tlbe.tlbe_active = 0x1;

#ifdef DEBUG
        printf("current_vpage=%d,current_ppage=%d\n", current_vpage, current_ppage);
#endif

        _out(TLB_ADD_ENTRY, *(int *) (&tlbe));
    }
}

