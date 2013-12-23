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

	RadixSlot **_slotDescriptors;
	unsigned _slotsCount;
} RadixSorter;

void radixsort_init(RadixSorter *rs, unsigned keyLimit);
void radixsort_add(RadixSorter *rs, RadixItem *item);
RadixItem *radix_cut(RadixSorter *rs, unsigned key, void *data);
RadixItem **radixsort_dump(RadixSorter *rs);
void radixsort_destroy(RadixSorter *rs);
void radixsort_stat(RadixSorter *rs);

#endif /* RADIXSORT_H_ */
