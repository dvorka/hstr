#ifndef RADIXSORT_H_
#define RADIXSORT_H_

typedef struct radixitem {
	unsigned key;
	void *data;
	struct radixitem *next;
} RadixItem;

#define SIX2FOUR_SIZE 1000

typedef struct {
	unsigned size;
	unsigned maxValue;
	RadixItem **six2four[SIX2FOUR_SIZE];
} RadixSorter;

void radixsort_init(RadixSorter *rs);
void radixsort_add(RadixSorter *rs, RadixItem *item);
RadixItem *radix_cut(RadixSorter *rs, unsigned key, void *data);
RadixItem **radixsort_dump(RadixSorter *rs);

#endif /* RADIXSORT_H_ */
