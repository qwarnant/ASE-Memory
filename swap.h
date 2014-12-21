/*
 * swap.h
 *
 *  Created on: 16 déc. 2014
 *      Author: warnant
 */

#ifndef SWAP_H_
#define SWAP_H_

#include <stdio.h>

/* Define the swap file on the file system */
FILE *swap_file;

/*
* init_swap()
* This method initializes the swap file on the file system
*/
char init_swap(const char*);
/*
* store_to_swap
* This method store a page of memory on the swap file on the disk
*/
char store_to_swap(int, int);
/*
* store_to_swap
* This method loads a page of memory on the swap file on the disk
*/
char fetch_from_swap(int, int);

#endif /* SWAP_H_ */
