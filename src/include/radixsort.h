/*
 ============================================================================
 Name        : radixsort.h
 Author      : martin.dvorak@mindforger.com
 Copyright   : Apache 2.0
 Description : Radix sort
 ============================================================================
*/

#ifndef RADIXSORT_H_
#define RADIXSORT_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stddef.h>
#include "hstr_utils.h"

#define SLOT_SIZE 1000

typedef struct radixitem {
	unsigned key;
	void *data;
	struct radixitem *next;
} RadixItem;

typedef struct radixslot {
	unsigned min;
	unsigned max;
	unsigned size;
} RadixSlot;

typedef struct {
	unsigned size;
	unsigned maxKey;
	unsigned keyLimit;
	RadixItem ***topDigits;

	bool optFloorAndInsertBigKeys;
	bool optIgnoreBigKeys;

	RadixSlot **_slotDescriptors;
	unsigned _slotsCount;
	unsigned _topIndexLimit;
} RadixSorter;

void radixsort_init(RadixSorter *rs, unsigned keyLimit);
void radixsort_add(RadixSorter *rs, RadixItem *item);
RadixItem *radix_cut(RadixSorter *rs, unsigned key, void *data);
RadixItem **radixsort_dump(RadixSorter *rs);
void radixsort_destroy(RadixSorter *rs);
void radixsort_stat(RadixSorter *rs, bool listing);

#endif /* RADIXSORT_H_ */
