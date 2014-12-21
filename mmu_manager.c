#include "hardware.h"
#include "mmu.h"

extern void user_process();

/*
* init_mmu()
* This method initializes the memory management unit
 */
void init_mmu() {

	init_master();

	IRQVECTOR[MMU_IRQ] = mmuhandler;

	_mask(0x1001);
}


int main(int argc, char **argv) {
	init_mmu();
	user_process();
}

