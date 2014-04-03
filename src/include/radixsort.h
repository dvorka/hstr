/*
 ============================================================================
 Name        : radixsort.h
 Author      : martin.dvorak@midforger.com
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

#define RADIX_SLOT_SIZE 1000

#define RADIX_DEBUG_LEVEL_NONE  0
#define RADIX_DEBUG_LEVEL_WARN  1
#define RADIX_DEBUG_LEVEL_DEBUG 2

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
	unsigned _debug;
} RadixSorter;

void radixsort_init(RadixSorter *rs, unsigned keyLimit);
void radixsort_set_debug_level(RadixSorter *rs, unsigned debugLevel);
void radixsort_add(RadixSorter *rs, RadixItem *item);
RadixItem *radix_cut(RadixSorter *rs, unsigned key, void *data);
RadixItem **radixsort_dump(RadixSorter *rs);
void radixsort_destroy(RadixSorter *rs);
void radixsort_stat(RadixSorter *rs, bool listing);

#endif /* RADIXSORT_H_ */
